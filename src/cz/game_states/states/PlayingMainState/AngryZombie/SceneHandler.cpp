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

#include "WorldObject.h"




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
void
SceneHandler::buildStaticWorldObjects(float boxWidth,
                                      float space,
                                      unsigned int count,
                                      float &tarimaZ)
{
    // create the "tarima"
    Ogre::Entity* tarima = mData.sceneManager->createEntity("tarima.mesh");
    Ogre::Vector3 size = tarima->getBoundingBox().getSize();

    tarimaZ = size.z;

    // we need to check how many tarimas we have to build
    const float total = count * boxWidth + (count-1) * space;
    unsigned int tcount = total / size.x;
    if (tcount * size.x < total) {
        tcount++;
    }

    // the boxes will be put in a manner that the center of the size total is the
    // (0,0,0).
    //
    const float tarimasTotal = tcount * size.x;
    const float step = size.x;
    const float start = -tarimasTotal / 2.f + step/2.f;
    for (unsigned int i = 0; i < tcount; ++i) {
        // create the scene node and the entity
        Ogre::SceneNode* node = mData.sceneManager->getRootSceneNode()->createChildSceneNode();
        if (tarima == 0) {
            tarima = mData.sceneManager->createEntity("tarima.mesh");
        }
        node->attachObject(tarima);
        node->setPosition(start + step*i, 0, size.z/2.f);

        // add the node to be tracked, this will destroy the entity also
        mHolder.addNode(node);
        tarima = 0;
    }

    // now we need to create the collision object
    PhysicObject* po = new PhysicObject;;
    physics::BulletObject& bo = po->bulletObject();

    Ogre::AxisAlignedBox bb(Ogre::Vector3(-tarimasTotal/2.f, -size.y/2.f, 0),
                            Ogre::Vector3(tarimasTotal/2.f, size.y/2.f, size.z));
    physics::BulletImporter::createBox(bo, bb, 0, false);
    addBulletObjectToHolder(bo);
    mData.dynamicWorld->addObject(bo);
    mHolder.addPhysicObject(po);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SceneHandler::SceneHandler() :
    mCurrentSceneType(SceneType::SIMPLE)
,   mGoodBoxes(0)
,   mBadBoxes(0)
{
}

///////////////////////////////////////////////////////////////////////////////
SceneHandler::~SceneHandler()
{
    // this will reset all the current information
    mHolder.clearAll();
    mData.physicsHandler->clear();
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
    mData.physicsHandler->clear();

    // now we will build everything we need. For now we will create the boxes
    // only
    PhysicsHandler::PhysicObjectVec& objs = mData.physicsHandler->physicObjectsVec();

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
        numBoxesX = 10;
        numBoxesY = 10;
        break;
    case SceneType::NORMAL:
        numBoxesX = 20;
        numBoxesY = 10;
        break;
    case SceneType::COMPLEX:
        numBoxesX = 40;
        numBoxesY = 20;
        break;
    }

    Ogre::Entity* boxEnt = mData.sceneManager->createEntity("caja.mesh");
    Ogre::Vector3 size = boxEnt->getBoundingBox().getSize();

    // calculate the size of the boxes in the X axis
    const float space = 2.f;
    const float total = numBoxesX * size.x + (numBoxesX-1) * space;
    const float start = - total / 2.f - space;

    // construct the "tarimas"
    //
    float currentZ = 20;
    buildStaticWorldObjects(size.x, space, numBoxesX, currentZ);

    // build all the boxes
    const Ogre::AxisAlignedBox& bbBox = boxEnt->getBoundingBox();
    mGoodBoxes = mBadBoxes = 0;
    const float maxSide = currentZ;
    for (unsigned int i = 0; i < numBoxesY; ++i) {
        for (unsigned int j = 0; j < numBoxesX; ++j) {
            Ogre::Vector3 min(start + (j * size.x) + space, -size.y/2.f, currentZ);
            Ogre::Vector3 max(start + (j+1) * size.x, size.y / 2.f, currentZ + size.z);
            Ogre::AxisAlignedBox bb(min, max);

            // get the random material for the box
            std::stringstream ss;
            ss << "MiniDemo/Box";
            unsigned int matID = std::rand() % 5;
            ss << matID; // we have 4 mats

            // check if the material is good or bad
            const bool isGood = matID == 4; // the material number 5 is the chosen one

            if (isGood) {
                ++mGoodBoxes;
            } else {
                ++mBadBoxes;
            }

            PhysicObject* po = new WorldObject(maxSide, isGood);
            physics::BulletObject& bo = po->bulletObject();
            physics::BulletImporter::createBox(bo, bbBox, 10, false);

            // create the scene node
            ASSERT(bo.motionState.node() == 0);
            ASSERT(bo.entity == 0);
            Ogre::SceneNode* node = mData.sceneManager->getRootSceneNode()->createChildSceneNode();
            if (boxEnt == 0) {
                boxEnt = mData.sceneManager->createEntity("caja.mesh");
            }

            boxEnt->setMaterialName(ss.str());
            node->attachObject(boxEnt);
            node->setPosition(bb.getCenter());
            bo.setTransform(bb.getCenter(), Ogre::Quaternion::IDENTITY);
            bo.motionState.setNode(node);
            bo.entity = boxEnt;
            boxEnt = 0;

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
    return goodBoxesCount() + badBoxesCount() + 2; // +2 = tarimas
}


} /* namespace demo_app */
