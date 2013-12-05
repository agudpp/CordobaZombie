/*
 * MiniDemoApp.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#include "MiniDemoApp.h"

#include <OgreResourceGroupManager.h>
#include <OgreMeshManager.h>
#include <OgrePlane.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include <debug/DebugUtil.h>
#include <debug/PrimitiveDrawer.h>
#include <input/InputHelper.h>
#include <input/InputKeyboard.h>
#include <ResourceHandler.h>
#include <physics/BulletImporter.h>



// define useful macro precondition
#define PRECONDITION_CHECK \
    ASSERT(mData.sceneMngr);\
    ASSERT(mData.camera);\
    ASSERT(mData.frontEndManager);\
    ASSERT(mData.mouseCursor);\
    ASSERT(mData.inputHelper);\
    ASSERT(mData.effectHandler);\
    ASSERT(mData.rscHandler);\
    ASSERT(mData.soundHandler);\
    ASSERT(mData.informer);


namespace demo_app {


////////////////////////////////////////////////////////////////////////////////
void
MiniDemoApp::loadFloor(void)
{
    Ogre::Plane p;
    p.normal = Ogre::Vector3(0, 0, 1);
    p.d = 0;
    Ogre::MeshManager::getSingleton().createPlane("FloorPlane",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, p, 200000,
        200000, 20, 20, true, 1, 9000, 9000, Ogre::Vector3::UNIT_Y);

    Ogre::Entity* planeEnt = 0;
    try {
        planeEnt = mData.sceneMngr->getEntity("floor");
    } catch(...) {
        planeEnt = mData.sceneMngr->createEntity("floor", "FloorPlane");
    }
    try {
        mFloorNode = mData.sceneMngr->getSceneNode("floorNode");
    } catch(...) {
        mFloorNode = mData.sceneMngr->getRootSceneNode()->createChildSceneNode("floorNode");
    }
    // Create an entity (the floor)
    planeEnt->setMaterialName("GrassSample");
    mFloorNode->attachObject(planeEnt);

//    core::PrimitiveDrawer::instance().create3DAxis(Ogre::Vector3::ZERO, 15);
}

////////////////////////////////////////////////////////////////////////////////
void
MiniDemoApp::handlePlayerInput(float frameTime)
{
    // check tracking camera
    const float lMouseX = float(input::Mouse::relX());
    const float lMouseY = float(input::Mouse::relY());
    float moveSides = 0;

    // moving to the left / right
    if (mData.inputHelper->isKeyPressed(input::KeyCode::KC_A) ||
        mData.inputHelper->isKeyPressed(input::KeyCode::KC_LEFT)) {
        moveSides = 1.f * 200.f * frameTime;
    }
    if (mData.inputHelper->isKeyPressed(input::KeyCode::KC_D) ||
        mData.inputHelper->isKeyPressed(input::KeyCode::KC_RIGHT)) {
        moveSides = -1.f * 200.f * frameTime;
    }

    const float factor = -0.005 * 1.5f;
    mPlayer.rotateCamera(Ogre::Radian(moveSides * factor),
                         Ogre::Radian(lMouseY * factor),
                         Ogre::Radian(lMouseX * factor));

    // check if we need to reset the scene!
    if (mData.inputHelper->isKeyReleased(input::KeyCode::KC_SPACE)) {
        mSceneHandler.configureCurrentScene();
    }

    // check if we fire something
    if (mData.inputHelper->isMouseReleased(input::MouseButtonID::MB_Left)) {
        Ogre::Ray ray = mData.camera->getCameraToViewportRay(0.5f, 0.5f);
        const Ogre::Vector3 halfSize(2.5,2.5,2.5);
        const Ogre::Vector3 camPos = mData.camera->getRealPosition();
        Ogre::AxisAlignedBox bb(camPos - halfSize, camPos + halfSize);
        PhysicObject* po = new PhysicObject;
        physics::BulletImporter::createBox(po->bulletObject(), bb, 10);
        mSceneHandler.addPhysicObject(po);

        Ogre::Vector3 force(mData.camera->getDerivedDirection());
        force.normalise();
        force *= 2700;
        btVector3 btForce(force.x, force.y, force.z);
        po->bulletObject().rigidBody->applyCentralImpulse(btForce);
    }

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MiniDemoApp::MiniDemoApp() :
    mFloorNode(0)
{

}

////////////////////////////////////////////////////////////////////////////////
MiniDemoApp::~MiniDemoApp()
{

}

////////////////////////////////////////////////////////////////////////////////
void
MiniDemoApp::setData(const DemoData& data)
{
    // check that all demo data is right
    mData = data;

    PRECONDITION_CHECK;
}

////////////////////////////////////////////////////////////////////////////////
bool
MiniDemoApp::getResourceToLoad(rrh::ResourceGroup& resource)
{
    PRECONDITION_CHECK;

    debugERROR("For now we will load everything to avoid problems, then we will"
        " load the resources correctly here.\n");
    std::string fullPath;
    mData.rscHandler->getResourceFullPath("./resources.cfg", fullPath);
    resource.setOgreResourceFile(fullPath);

    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
MiniDemoApp::getResourceToUnload(rrh::ResourceGroup& resource)
{
    // we remove the same that we load
    return getResourceToLoad(resource);
}

////////////////////////////////////////////////////////////////////////////////
bool
MiniDemoApp::load(void)
{
    PRECONDITION_CHECK;

    // configure the internal members
    mPlayer.configure(mData.sceneMngr, mData.camera);

    SceneHandler::Data sceneData;
    sceneData.dynamicWorld = &mDynamicWorld;
    sceneData.physicsHandler = &mPhysicsHandler;
    sceneData.player = &mPlayer;
    sceneData.sceneManager = mData.sceneMngr;
    mSceneHandler.setData(sceneData);

    // configure the floor to be used
    loadFloor();

    // we will load everything here. Parse the SceneHandler and everything else
    debugERROR("TODO\n");
    // reset the informer
    mData.informer->reset();

    // build the scene
    mSceneHandler.configureCurrentScene();

    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
MiniDemoApp::unload(void)
{
    PRECONDITION_CHECK;

    // destroy the floor
    if (mFloorNode) {
        mFloorNode->removeAndDestroyAllChildren();
        mData.sceneMngr->getRootSceneNode()->removeAndDestroyChild(mFloorNode->getName());
        mFloorNode = 0;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
MiniDemoApp::update(float timeFrame)
{
    // update the input
    handlePlayerInput(timeFrame);

    // update the physics world
    mDynamicWorld.simulate(timeFrame);

    return true;
}

} /* namespace demo_app */
