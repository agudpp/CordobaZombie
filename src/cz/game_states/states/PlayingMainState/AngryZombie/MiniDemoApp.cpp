/*
 * MiniDemoApp.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#include "MiniDemoApp.h"

#include <string.h>

#include <OgreResourceGroupManager.h>
#include <OgreMeshManager.h>
#include <OgrePlane.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreOverlayManager.h>
#include <OgreStringConverter.h>

#include <debug/DebugUtil.h>
#include <debug/PrimitiveDrawer.h>
#include <input/InputHelper.h>
#include <input/InputKeyboard.h>
#include <ResourceHandler.h>
#include <physics/BulletImporter.h>
#include <cursor/MouseCursor.h>

#include "WorldObject.h"



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
// Internal HUD
//
MiniDemoApp::HUD::HUD() :
    mOverlay(0)
{
    memset(mTextAreas, 0, sizeof(Ogre::TextAreaOverlayElement*) * T_COUNT);
}
MiniDemoApp::HUD::~HUD()
{
    destroy();
}

bool
MiniDemoApp::HUD::build(void)
{
    // get the overlay
    mOverlay = Ogre::OverlayManager::getSingleton().getByName("MiniDemoHUD");
    if (!mOverlay) {
        debugERROR("Overlay MiniDemoHUD not found\n");
        return false;
    }
    mOverlay->show();

    // get the text areas
    mTextAreas[Texts::TOTAL_GOOD] = reinterpret_cast<Ogre::TextAreaOverlayElement*>(
        mOverlay->getChild("MiniDemoHUD/TotalGood"));
    if (!mTextAreas[Texts::TOTAL_GOOD]) {
        debugERROR("Text area MiniDemoHUD/TotalGood not found\n");
        return false;
    }
    mTextAreas[Texts::TOTAL_BAD] = reinterpret_cast<Ogre::TextAreaOverlayElement*>(
        mOverlay->getChild("MiniDemoHUD/TotalBad"));
    if (!mTextAreas[Texts::TOTAL_BAD]) {
        debugERROR("Text area MiniDemoHUD/TotalBad not found\n");
        return false;
    }
    mTextAreas[Texts::GOOD] = reinterpret_cast<Ogre::TextAreaOverlayElement*>(
        mOverlay->getChild("MiniDemoHUD/Good"));
    if (!mTextAreas[Texts::GOOD]) {
        debugERROR("Text area MiniDemoHUD/Good not found\n");
        return false;
    }
    mTextAreas[Texts::BAD] = reinterpret_cast<Ogre::TextAreaOverlayElement*>(
        mOverlay->getChild("MiniDemoHUD/Bad"));
    if (!mTextAreas[Texts::BAD]) {
        debugERROR("Text area MiniDemoHUD/Bad not found\n");
        return false;
    }

    // everything perfect
    return true;
}
void
MiniDemoApp::HUD::destroy(void)
{
    if (mOverlay == 0) {
        return;
    }
    Ogre::OverlayManager::getSingleton().destroy(mOverlay);
    mOverlay = 0;
    memset(mTextAreas, 0, sizeof(Ogre::TextAreaOverlayElement*) * T_COUNT);
}
void
MiniDemoApp::HUD::setVisible(bool visible)
{
    ASSERT(mOverlay);
    if (visible) {
        mOverlay->show();
    } else {
        mOverlay->hide();
    }
}
void
MiniDemoApp::HUD::setData(const GameLogicData& data)
{
    ASSERT(mOverlay);
    // set the data
    mTextAreas[Texts::TOTAL_GOOD]->setCaption(
        Ogre::StringConverter::toString(data.totalGoodBoxes));

    mTextAreas[Texts::TOTAL_BAD]->setCaption(
        Ogre::StringConverter::toString(data.totalBadBoxes));

    mTextAreas[Texts::GOOD]->setCaption(
        Ogre::StringConverter::toString(data.goodBoxes));

    mTextAreas[Texts::BAD]->setCaption(
        Ogre::StringConverter::toString(data.badBoxes));

}



////////////////////////////////////////////////////////////////////////////////
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
Projectile*
MiniDemoApp::buildPorjectile(const Ogre::Vector3& pos)
{
    Ogre::Entity* ent = mData.sceneMngr->createEntity("zombie.mesh");
    Ogre::SceneNode* node = mData.sceneMngr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(ent);
    node->setPosition(pos);
    Projectile* result = new Projectile(node, ent);
    result->reset(pos);

    return result;
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
    bool resetScene = false;
    if (mData.inputHelper->isKeyReleased(input::KeyCode::KC_1)) {
        // simple scene
        mSceneHandler.setSceneType(SceneHandler::SceneType::SIMPLE);
        resetScene = true;
    } else if (mData.inputHelper->isKeyReleased(input::KeyCode::KC_2)) {
        // simple scene
        mSceneHandler.setSceneType(SceneHandler::SceneType::NORMAL);
        resetScene = true;
    } else if (mData.inputHelper->isKeyReleased(input::KeyCode::KC_3)) {
        // simple scene
        mSceneHandler.setSceneType(SceneHandler::SceneType::COMPLEX);
        resetScene = true;
    }
    if (resetScene) {
        resetCurrentScene();
    }

    // check if we fire something
    if (mData.inputHelper->isMouseReleased(input::MouseButtonID::MB_Left)) {
        // get the basic information of the camera and everything else
        const Ogre::Vector3 camPos = mData.camera->getRealPosition();
        Ogre::Vector3 force(mData.camera->getDerivedDirection());
        force.normalise();

        // check what we have to fire a box or a ragdoll (cheat)
        if (mData.inputHelper->isMousePressed(input::MouseButtonID::MB_Right)) {
            // RAGDOLL!
            if (mProjectiles.size() == NUM_PROJECTILES) {
                debugWARNING("We cannot throw any more projectiles!\n");
                return;
            }
            std::shared_ptr<Projectile> projectile(buildPorjectile(camPos));
            mProjectiles.push_back(projectile);
            force *= 8700;
            projectile->applyForce(force);
        } else {
            // we have to fire a head
            const Ogre::Vector3 halfSize(2.5,2.5,2.5);
            Ogre::AxisAlignedBox bb(camPos - halfSize, camPos + halfSize);
            PhysicObject* po = new PhysicObject;
            physics::BulletImporter::createBox(po->bulletObject(), bb, 10, false);

            // create the head
            Ogre::SceneNode* headNode = mData.sceneMngr->getRootSceneNode()->createChildSceneNode();
            headNode->setPosition(bb.getCenter());
            Ogre::Entity* headEnt = mData.sceneMngr->createEntity("cabeza.mesh");
            headNode->attachObject(headEnt);
            po->bulletObject().motionState.setNode(headNode);
            po->bulletObject().entity = headEnt;

            mSceneHandler.addPhysicObject(po);
            force *= 4700;
            btVector3 btForce(force.x, force.y, force.z);
            po->bulletObject().rigidBody->applyCentralImpulse(btForce);
        }
    }

}

////////////////////////////////////////////////////////////////////////////////
void
MiniDemoApp::runningState(void)
{
    // update the input
    handlePlayerInput(mTimeFrame);

    // update the physics world
    mDynamicWorld.simulate(mTimeFrame);

    // update all the projectiles
    for (unsigned int i = 0; i < mProjectiles.size(); ++i) {
        std::shared_ptr<Projectile>& p = mProjectiles[i];
        p->update(mTimeFrame);
//        if (!p->update(mTimeFrame)) {
//            // remove this
//            p = mProjectiles.back();
//            mProjectiles.pop_back();
//            --i;
//        }
    }

    // update all the elements
    mPhysicsHandler.performCollisions(mTimeFrame);

    // configure the statistics informer data
    mInformerData.numRagdolls = mProjectiles.size();
    mInformerData.totalPhysicsObjects = mInformerData.numRagdolls +
        mSceneHandler.getObjectsCount();
    mData.informer->update(mTimeFrame, mInformerData);

    if (mGameLogicData != mGameLogicDataInternal) {
        mGameLogicDataInternal = mGameLogicData;

        // check if we win | loose
        if (mGameLogicData.loose()) {
            // we loose
            debugRED("GAME OVER!\n");
            resetCurrentScene();
        } else if (mGameLogicData.win()) {
            // we win
            debugGREEN("WE WINNN!!!\n");
            resetCurrentScene();
        }

        mHud.setData(mGameLogicData);
    }

}

////////////////////////////////////////////////////////////////////////////////
void
MiniDemoApp::pauseState(void)
{
    ASSERT(mPauseOverlay);
    ASSERT(mHelpOverlay);

    // now we have to check if we will continue or we will go out
    if (mData.inputHelper->isKeyReleased(input::KeyCode::KC_ESCAPE)) {
        // if we are showing the help then we return to the pause overlay
        if (mHelpOverlay->isVisible()) {
            mHelpOverlay->hide();
            mPauseOverlay->show();
        } else {
            // we are in the pause overlay!
            mRunning = false;
            mPauseOverlay->hide();
            mHelpOverlay->hide();
        }
        return;
    }

    // check if we press z then we need to show the help information
    if (mData.inputHelper->isKeyReleased(input::KeyCode::KC_Z)) {
        mHelpOverlay->show();
        mPauseOverlay->hide();
    }

    // if we press the click we will continue
    if (mData.inputHelper->isMouseReleased(input::MouseButtonID::MB_Left)) {

        // if we are showing the help then go back to the pause
        if (mHelpOverlay->isVisible()) {
            mHelpOverlay->hide();
            mPauseOverlay->show();
        } else {
            mPauseOverlay->hide();
            mHud.setVisible(true);
            mInternalState = State::RUNNING;
        }
        return;
    }
}

////////////////////////////////////////////////////////////////////////////////
void
MiniDemoApp::finishingState(void)
{

}

////////////////////////////////////////////////////////////////////////////////
void
MiniDemoApp::resetCurrentScene(void)
{
    mProjectiles.clear();
    mSceneHandler.configureCurrentScene();
    mInformerData.reset();
    mGameLogicData.reset();
    mHud.setData(mGameLogicData);

    mGameLogicData.totalBadBoxes = mSceneHandler.badBoxesCount();
    mGameLogicData.totalGoodBoxes = mSceneHandler.goodBoxesCount();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MiniDemoApp::MiniDemoApp() :
    mDynamicWorld(btVector3(0,0,-50))
,   mFloorNode(0)
,   mInternalState(RUNNING)
,   mRunning(true)
,   mPauseOverlay(0)
,   mHelpOverlay(0)
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

    Projectile::setDynamicWorld(&mDynamicWorld);
    WorldObject::setGameLogicData(&mGameLogicData);
}

////////////////////////////////////////////////////////////////////////////////
bool
MiniDemoApp::getResourceToLoad(rrh::ResourceGroup& resource)
{
    PRECONDITION_CHECK;

    debugERROR("For now we will load everything to avoid problems, then we will"
        " load the resources correctly here.\n");
    std::string fullPath;
    mData.rscHandler->getResourceFullPath("mini_demo/resources.cfg", fullPath);
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

    // configure / load the hud
    if (!mHud.build()) {
        return false;
    }

    // load the pause overlay
    mPauseOverlay = Ogre::OverlayManager::getSingleton().getByName("MiniDemoPause");
    if (mPauseOverlay == 0) {
        debugERROR("We couldn't load the overlay MiniDemoPause\n");
        return false;
    }
    mPauseOverlay->hide();

    // load the help overlay
    mHelpOverlay = Ogre::OverlayManager::getSingleton().getByName("MiniDemoHelp");
    if (mHelpOverlay == 0) {
        debugERROR("We couldn't load the overlay MiniDemoHelp\n");
        return false;
    }
    mHelpOverlay->hide();

    // build the scene
    resetCurrentScene();

    // hide the cursor
    mData.mouseCursor->setVisible(false);

    mRunning = true;

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

    mHud.destroy();

    // destroy pause overlay
    if (mPauseOverlay) {
        Ogre::OverlayManager::getSingleton().destroy(mPauseOverlay);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
MiniDemoApp::update(float timeFrame)
{
    mTimeFrame = timeFrame;

    switch (mInternalState) {
        case State::RUNNING:
            runningState();
            break;
        case State::PAUSE:
            pauseState();
            break;
        case State::FINISH_CURRENT_GAME:
            finishingState();
            break;
        default:
            debugERROR("Invalid state? %d\n", mInternalState);
            return false;
    }


    // check if escaped was pressed
    if (mData.inputHelper->isKeyReleased(input::KeyCode::KC_ESCAPE)) {
        mInternalState = State::PAUSE;
        if (mPauseOverlay) {
            mPauseOverlay->show();
        }
        mHud.setVisible(false);
    }

    return mRunning;
}

} /* namespace demo_app */
