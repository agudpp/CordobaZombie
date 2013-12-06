/*
 * SceneHandler.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#include "SceneHandler.h"

#include <sstream>
#include <cmath>

#include <OgreManualObject.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include <physics/BulletImporter.h>
#include <debug/OgreNameGen.h>




#define CHECK_PRECONDITION  \
    ASSERT(mData.player);\
    ASSERT(mData.physicsHandler);\
    ASSERT(mData.dynamicWorld);\
    ASSERT(mData.sceneManager);


namespace demo_app {

////////////////////////////////////////////////////////////////////////////////
void
SceneHandler::createBox(physics::BulletObject& result,
                        const Ogre::AxisAlignedBox& bb,
                        float mass)
{
    // get the random material for the box
    std::stringstream ss;
    ss << "MiniDemo/Box";
    ss << std::rand() % 5; // we have 4 mats

    // Create the manual object
    Ogre::ManualObject* manual =
        mData.sceneManager->createManualObject(core::OgreNameGen::getFreshName());

    // construct the manual
    manual->begin(ss.str());

    const Ogre::Vector3 halfSize = bb.getSize() * 0.5f;
    const Ogre::Vector3 offset = Ogre::Vector3::ZERO;

    // create the different vertices, first the top four and then the bottom
    // four
    manual->position(Ogre::Vector3(halfSize.x, halfSize.y, halfSize.z) + offset);
    manual->textureCoord(0,0);
    manual->position(Ogre::Vector3(-halfSize.x, halfSize.y, halfSize.z) + offset);
    manual->textureCoord(1,0);
    manual->position(Ogre::Vector3(-halfSize.x, halfSize.y, -halfSize.z) + offset);
    manual->textureCoord(0,1);
    manual->position(Ogre::Vector3(halfSize.x, halfSize.y, -halfSize.z) + offset);
    manual->textureCoord(1,1);

    // bottom
    manual->position(Ogre::Vector3(halfSize.x, -halfSize.y, halfSize.z) + offset);
    manual->textureCoord(1,1);
    manual->position(Ogre::Vector3(-halfSize.x, -halfSize.y, halfSize.z) + offset);
    manual->textureCoord(0,1);
    manual->position(Ogre::Vector3(-halfSize.x, -halfSize.y, -halfSize.z) + offset);
    manual->textureCoord(1,0);
    manual->position(Ogre::Vector3(halfSize.x, -halfSize.y, -halfSize.z) + offset);
    manual->textureCoord(0,0);


    // set the triangles
    manual->triangle(2, 1, 0); manual->triangle(0, 3, 2);   // top
    manual->triangle(4, 5, 6); manual->triangle(7, 4, 6);   // bottom
    manual->triangle(1, 2, 6); manual->triangle(1, 6, 5);   // left
    manual->triangle(2, 7, 6); manual->triangle(2, 3, 7);
    manual->triangle(3, 4, 7); manual->triangle(4, 3, 0);
    manual->triangle(4, 0, 1); manual->triangle(5, 4, 1);

    manual->end(); // and building the manual

    // create a scene node
    Ogre::SceneNode* node = mData.sceneManager->getRootSceneNode()->createChildSceneNode();
    node->attachObject(manual);

    // set the node to the bullet object
    result.motionState.setNode(node);

    // construct bullet stuff
    //
    btVector3 localInertia(0,0,0);
    btTransform startTransform;
    startTransform.setIdentity();

    const Ogre::Vector3 center = bb.getCenter();
    btCollisionShape* shape = new btBoxShape(btVector3(halfSize.x,
                                                       halfSize.y,
                                                       halfSize.z));
    if (mass != 0.f) {
        shape->calculateLocalInertia(mass,localInertia);
    }
    startTransform.setOrigin(btVector3(center.x,center.y,center.z));
    result.motionState.setWorldTransform(startTransform);
    btRigidBody::btRigidBodyConstructionInfo handrbInfo(mass,
                                                        &(result.motionState),
                                                        shape,
                                                        localInertia);
    btRigidBody* handbody = new btRigidBody(handrbInfo);
    result.rigidBody = handbody;
    result.shape = shape;

    handbody->setUserPointer(&result);
}



///////////////////////////////////////////////////////////////////////////////
void
SceneHandler::addBulletObjectToHolder(physics::BulletObject& object)
{
    if (object.motionState.node()) {
        mHolder.addNode(object.motionState.node());
    }
    mHolder.addShape(object.shape);
    mHolder.addRigidBody(object.rigidBody);
}

///////////////////////////////////////////////////////////////////////////////
void
SceneHandler::buildWorldPhysicsLimits(const Ogre::AxisAlignedBox& bb)
{
    // construct 5 boxes to set the limits
    // floor
    Ogre::AxisAlignedBox floorBox(bb);
    floorBox.setMaximumZ(0);
    floorBox.setMinimumZ(-100);
    physics::BulletObject* floorObject = physics::BulletImporter::createBox(floorBox, 0, false);
    mData.dynamicWorld->addObject(*floorObject);
    mHolder.addBulletObject(floorObject);
    addBulletObjectToHolder(*floorObject);

    // walls.
    debugERROR("TODO!!\n");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SceneHandler::SceneHandler() :
    mCurrentSceneType(SceneType::SIMPLE)
{
}

///////////////////////////////////////////////////////////////////////////////
SceneHandler::~SceneHandler()
{
    // this will reset all the current information
    mHolder.clearAll();
    mPhysicsHandler.clear();
}

///////////////////////////////////////////////////////////////////////////////
void
SceneHandler::setData(const Data& data)
{
    mData = data;
    CHECK_PRECONDITION;

    // configure the holder
    mHolder.dynamicWorld = mData.dynamicWorld;
}

///////////////////////////////////////////////////////////////////////////////
void
SceneHandler::configureCurrentScene(void)
{
    CHECK_PRECONDITION;

    // this will reset all the current information
    mHolder.clearAll();
    mPhysicsHandler.clear();

    // now we will build everything we need. For now we will create the boxes
    // only
    PhysicsHandler::PhysicObjectVec& objs = mPhysicsHandler.physicObjectsVec();

    // construct the world limits
    Ogre::AxisAlignedBox worldLimits(Ogre::Vector3(-9999, -9999, -9999),
                                     Ogre::Vector3(9999, 9999, 9999));
    buildWorldPhysicsLimits(worldLimits);

    Ogre::AxisAlignedBox elementsBB(Ogre::Vector3::ZERO, Ogre::Vector3::ZERO);
    // we will construct all the boxes and we will build different number of
    // boxes depending on the scene type
    unsigned int numBoxesX = 0, numBoxesY = 0;
    switch(mCurrentSceneType) {
    case SceneType::SIMPLE:
        numBoxesX = 20;
        numBoxesY = 10;
        break;
    case SceneType::NORMAL:
        numBoxesX = 20;
        numBoxesY = 25;
        break;
    case SceneType::COMPLEX:
        numBoxesX = 40;
        numBoxesY = 40;
        break;
    }

    Ogre::Vector3 size(20,20,20);
    float currentZ = 10;
    for (unsigned int i = 0; i < numBoxesX; ++i) {
        for (unsigned int j = 0; j < numBoxesY; ++j) {
            Ogre::Vector3 min(j*size.x+2,0,currentZ);
            Ogre::Vector3 max((j+1)*size.x,size.y,currentZ + size.z);

            PhysicObject* po = new PhysicObject;;
            physics::BulletObject& bo = po->bulletObject();

            Ogre::AxisAlignedBox bb(min, max);
            createBox(bo, bb, 10);
            addBulletObjectToHolder(bo);
            mData.dynamicWorld->addObject(bo);

            mHolder.addPhysicObject(po);
            objs.push_back(po);

            elementsBB.merge(min);
            elementsBB.merge(max);
        }
        currentZ += size.z + 2;
    }

    // configure the player position
    const float distance = elementsBB.getSize().length();
    Ogre::Vector3 center = elementsBB.getCenter();
    center.z = 40;
    mData.player->setPositions(center,
                               Ogre::Vector3(distance,distance,0));
//    mPlayer.setRotationLimitsRadians(0, M_PI/2.f);
}

///////////////////////////////////////////////////////////////////////////////
void
SceneHandler::addPhysicObject(PhysicObject* po)
{
    CHECK_PRECONDITION;
    ASSERT(po);

    mHolder.addPhysicObject(po);
    addBulletObjectToHolder(po->bulletObject());
    mData.dynamicWorld->addObject(po->bulletObject());
}

///////////////////////////////////////////////////////////////////////////////
bool
SceneHandler::setSceneType(SceneType t)
{
    CHECK_PRECONDITION;
    mCurrentSceneType = t;
    return true;
}
///////////////////////////////////////////////////////////////////////////////
unsigned int
SceneHandler::getObjectsCount(void) const
{
    return mData.physicsHandler->physicObjectsVec().size();
}


} /* namespace demo_app */
