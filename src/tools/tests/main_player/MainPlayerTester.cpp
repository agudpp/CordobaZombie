/*
 * MainPlayerTester.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreMath.h>
#include <OgreAnimationState.h>
#include <OgreResourceManager.h>
#include <OgreFontManager.h>

#include <memory>
#include <cstring>

#include <tinyxml/tinyxml.h>

#include <debug/PrimitiveDrawer.h>
#include <xml/XMLHelper.h>
#include <fx/effects/billboards/BillboardLoader.h>

#include "MainPlayerTester.h"


// helper stuff
//
namespace {

// Construct the mouse input keys we will use
//
static std::vector<input::MouseButtonID>
getMouseButtons(void)
{
    std::vector<input::MouseButtonID> buttons;
    buttons.reserve(10); // just in case :p

    buttons.push_back(input::MouseButtonID::MB_Left);
    buttons.push_back(input::MouseButtonID::MB_Right);

    return buttons;
}


// Construct the keyboard keys we will use
//
static std::vector<input::KeyCode>
getKeyboardKeys(void)
{
    std::vector<input::KeyCode> buttons;
    buttons.reserve(18); // just in case :p

    buttons.push_back(input::KeyCode::KC_ESCAPE);
    buttons.push_back(input::KeyCode::KC_A);
    buttons.push_back(input::KeyCode::KC_S);
    buttons.push_back(input::KeyCode::KC_D);
    buttons.push_back(input::KeyCode::KC_W);
    buttons.push_back(input::KeyCode::KC_LEFT);
    buttons.push_back(input::KeyCode::KC_DOWN);
    buttons.push_back(input::KeyCode::KC_RIGHT);
    buttons.push_back(input::KeyCode::KC_UP);
    buttons.push_back(input::KeyCode::KC_1);
    buttons.push_back(input::KeyCode::KC_2);
    buttons.push_back(input::KeyCode::KC_E);
    buttons.push_back(input::KeyCode::KC_L);
    buttons.push_back(input::KeyCode::KC_ADD);
    buttons.push_back(input::KeyCode::KC_SPACE);

    return buttons;
}

inline Ogre::Quaternion
globalOgreRotation(const Ogre::Bone *bone, const Ogre::SceneNode* parentNode)
{
    return parentNode->_getDerivedOrientation() * bone->_getDerivedOrientation();
}
inline Ogre::Vector3
globalOgrePosition(const Ogre::Bone *bone, const Ogre::SceneNode* parentNode)
{
    return parentNode->getPosition() + parentNode->getOrientation() *
        bone->_getDerivedPosition();
}

}


namespace tests {

////////////////////////////////////////////////////////////////////////////////
void
MainPlayerTester::loadPlayer(void)
{
//    {
//        Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
//        Ogre::Entity* ent = mSceneMgr->createEntity("manos.mesh");
//        node->translate(10,10,10);
//        node->attachObject(ent);
//
//        Ogre::SkeletonInstance* s = ent->getSkeleton();
//
//        core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
//        pd.createBox(globalOgrePosition(s->getBone("palma"), node),
//                     Ogre::Vector3(2,2,2),
//                     Ogre::ColourValue::Red);
//        pd.createBox(globalOgrePosition(s->getBone("B_HEAD"), node),
//                     Ogre::Vector3(2,2,2),
//                     Ogre::ColourValue::Red);
//        pd.create3DAxis(node->getPosition(), node->getOrientation(), 3);
//        pd.create3DAxis(globalOgrePosition(s->getBone("B_HEAD"), node),
//                        s->getBone("B_HEAD")->getOrientation(),
//                        3);
//        mAnim = ent->getAnimationState("9mm_quieto");
//        mAnim->setLoop(true);
//        mAnim->setEnabled(true);
//    }

    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Entity* ent = mSceneMgr->createEntity("manos.mesh");
    node->attachObject(ent);

    mNode = node;
    mAnim = ent->getAnimationState("9mm_quieto");
    mAnim->setLoop(true);
    mAnim->setEnabled(true);

    Ogre::Quaternion offsetRot;
    offsetRot = Ogre::Quaternion(Ogre::Degree(270), Ogre::Vector3::UNIT_Z);
    mCamController.setData(mNode,
                           mCamera,
                           ent,
                           "B_HEAD",
                           Ogre::Vector3::ZERO,
                           offsetRot);

    // set cam limits
    mCamController.setConstraints(Ogre::Radian(0), Ogre::Radian(M_PI),
                                  Ogre::Radian(-M_PI_2), Ogre::Radian(M_PI_2));

//    mCamera->detachFromParent();
//    Ogre::Quaternion offsetRot;
//    offsetRot = Ogre::Quaternion(Ogre::Degree(270), Ogre::Vector3::UNIT_Z);/* *
//        Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3::UNIT_Y);*/
//    ent->attachObjectToBone("B_HEAD", mCamera, offsetRot);
//
    // get the weapon
    Ogre::Entity* weapon = mSceneMgr->createEntity("pistola.mesh");
    ent->attachObjectToBone("palma", weapon);

}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayerTester::handleCameraInput()
{
    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    //  float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;
    Ogre::Real zoom = mOrbitCamera.zoom();

    // HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
    // ARROWS TO MOVE THE CAMERA
    // NOTE: we are getting the cameraPosition and modifying the position
    // without using translation, this is because we want to move always
    // in the same axis whatever be the direction of the camera.


    // MOUSE
    const OIS::MouseState& lMouseState = mMouse->getMouseState();

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_LEFT) ||
        mInputHelper.isKeyPressed(input::KeyCode::KC_A)) {
        mTranslationVec.x -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_RIGHT) ||
        mInputHelper.isKeyPressed(input::KeyCode::KC_D)) {
        mTranslationVec.x += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_UP) ||
        mInputHelper.isKeyPressed(input::KeyCode::KC_W)) {
        mTranslationVec.z -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_DOWN) ||
        mInputHelper.isKeyPressed(input::KeyCode::KC_S)) {
        mTranslationVec.z += 1.0f;
    }

    if (mNode) {
        if(mTranslationVec != Ogre::Vector3::ZERO) {
            mNode->translate(mTranslationVec);
        }

        // check tracking camera
        static int lastX = 0, lastY = 0;
        const float lMouseX = float(input::Mouse::relX());
        const float lMouseY = float(input::Mouse::relY());
        if (mInputHelper.isMousePressed(input::MouseButtonID::MB_Right)) {
            const float factor = -0.01 * 1.5f;
            mCamController.rotate(Ogre::Radian(lMouseX * factor),
                Ogre::Radian(lMouseY * factor));
//            mNode->roll(Ogre::Radian(lMouseX * factor), Ogre::Node::TS_WORLD);
//            mNode->pitch(Ogre::Radian(lMouseY * factor));
        }
    } else {
        if(mTranslationVec != Ogre::Vector3::ZERO) {
            mOrbitCamera.moveCamera(mTranslationVec);
        }
        if(zoom != mOrbitCamera.zoom()) {
            mOrbitCamera.setZoom(zoom);
        }

        const float lMouseZ = input::Mouse::relZ();
        float scrollZoom = mOrbitCamera.zoom();
        if (lMouseZ > 0.0f) {
            scrollZoom += 5.f;
        } else if (lMouseZ < 0.0f) {
            scrollZoom -= 5.f;
        }
        if(scrollZoom != mOrbitCamera.zoom()) {
            mOrbitCamera.setZoom(scrollZoom);
        }

        // check tracking camera
        static int lastX = 0, lastY = 0;
        const float lMouseX = float(input::Mouse::relX());
        const float lMouseY = float(input::Mouse::relY());
        if (mInputHelper.isMousePressed(input::MouseButtonID::MB_Right)) {
            const float factor = -0.01 * 1.5f;
            mOrbitCamera.rotateCamera(Ogre::Radian(lMouseX * factor),
                                        Ogre::Radian(lMouseY * factor));
        }

        // check for the type of camera we want to use
        if (mInputHelper.isKeyPressed(input::KeyCode::KC_1)) {
            mOrbitCamera.setCameraType(tool::OrbitCamera::CameraType::FreeFly);
        } else if (mInputHelper.isKeyPressed(input::KeyCode::KC_2)) {
            mOrbitCamera.setCameraType(tool::OrbitCamera::CameraType::Orbit);
        }

    }

}

MainPlayerTester::MainPlayerTester() :
    core::AppTester(cz::GlobalData::lastTimeFrame)
,   mNode(0)
,   mEntity(0)
,   mOrbitCamera(mCamera, mSceneMgr, cz::GlobalData::lastTimeFrame)
,   mSelHelper(*mSceneMgr, *mCamera, mMouseCursor)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mAnim(0)
{
    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);

    // configure global data
    cz::GlobalData::sceneMngr = mSceneMgr;
    cz::GlobalData::camera = mCamera;

    mMouseCursor.setCursor(ui::MouseCursor::Cursor::NORMAL_CURSOR);
    mMouseCursor.setVisible(true);
    mMouseCursor.setWindowDimensions(mWindow->getWidth(), mWindow->getHeight());


}

MainPlayerTester::~MainPlayerTester()
{
    // TODO Auto-generated destructor stub
}

/* Load additional info */
void
MainPlayerTester::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // load floor
    Ogre::Plane p;
    p.normal = Ogre::Vector3(0, 0, 1);
    p.d = 0;
    Ogre::MeshManager::getSingleton().createPlane("FloorPlane",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, p, 200000,
        200000, 20, 20, true, 1, 200, 200, Ogre::Vector3::UNIT_Y);
    // Create an entity (the floor)
    Ogre::Entity *ent = mSceneMgr->createEntity("floor", "FloorPlane");
    ent->setMaterialName("GrassSample");
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);

    mSelHelper.setBaseIntersectionPlane(p);

    // load the player and weapons
    loadPlayer();
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
MainPlayerTester::update()
{
    // update the input system
    mInputHelper.update();

    // update mouse cursor
    mMouseCursor.updatePosition(input::Mouse::absX(), input::Mouse::absY());

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

    // update the camera
    handleCameraInput();

    if (mAnim)    mAnim->addTime(cz::GlobalData::lastTimeFrame);
}

}
