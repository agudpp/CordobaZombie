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

    // handling the input for the attaching
    buttons.push_back(input::KeyCode::KC_R);
    buttons.push_back(input::KeyCode::KC_T);
    buttons.push_back(input::KeyCode::KC_Y);
    buttons.push_back(input::KeyCode::KC_F);
    buttons.push_back(input::KeyCode::KC_G);
    buttons.push_back(input::KeyCode::KC_H);
    buttons.push_back(input::KeyCode::KC_U);
    buttons.push_back(input::KeyCode::KC_I);
    buttons.push_back(input::KeyCode::KC_O);
    buttons.push_back(input::KeyCode::KC_J);
    buttons.push_back(input::KeyCode::KC_K);
    buttons.push_back(input::KeyCode::KC_L);

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
    mEntity = ent;

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

    // get the weapon

    Ogre::Quaternion rotQuat(Ogre::Quaternion::IDENTITY);
    rotQuat = rotQuat * Ogre::Quaternion(Ogre::Degree(112.996), Ogre::Vector3::UNIT_X);
    rotQuat = rotQuat * Ogre::Quaternion(Ogre::Degree(-84.4396), Ogre::Vector3::UNIT_Y);
    rotQuat = rotQuat * Ogre::Quaternion(Ogre::Degree(-2.2), Ogre::Vector3::UNIT_Z);

    Ogre::Entity* weapon = mSceneMgr->createEntity("pistola.mesh");
    ent->attachObjectToBone("palma", weapon, rotQuat, Ogre::Vector3(0.29,-1.79,-4.54));
    mWeaponEnt = weapon;

}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayerTester::handleAttachingInput(void)
{
    // keys:
    // R, T, Y will increase X,Y,Z values in offsetPosition respectively.
    // F,G,H  will decrease X,Y,Z respectively.
    // U,I,O / J,K,L will increase / decrease the rotation angles in axis X,Y,Z
    // respectively

    Ogre::Vector3 offset(mOffsetPos);
    Ogre::Vector3 rot(mOffsetRot);

    // position
    static const float factor = 0.01f;
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_R)) {
        offset.x += factor;
    }
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_F)) {
        offset.x -= factor;
    }
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_T)) {
        offset.y += factor;
    }
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_G)) {
        offset.y -= factor;
    }
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_Y)) {
        offset.z += factor;
    }
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_H)) {
        offset.z -= factor;
    }

    // rotation
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_U)) {
        rot.x += factor;
    }
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_J)) {
        rot.x -= factor;
    }
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_I)) {
        rot.y += factor;
    }
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_K)) {
        rot.y -= factor;
    }
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_O)) {
        rot.z += factor;
    }
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_L)) {
        rot.z -= factor;
    }

    // check if we need to update something
    if (offset != mOffsetPos || rot != mOffsetRot) {
        ASSERT(mEntity);
        ASSERT(mWeaponEnt);
        mEntity->detachObjectFromBone(mWeaponEnt);

        // attach it with the new rotation and offset
        Ogre::Quaternion rotQuat(Ogre::Quaternion::IDENTITY);
        rotQuat = rotQuat * Ogre::Quaternion(Ogre::Degree(rot.x), Ogre::Vector3::UNIT_X);
        rotQuat = rotQuat * Ogre::Quaternion(Ogre::Degree(rot.y), Ogre::Vector3::UNIT_Y);
        rotQuat = rotQuat * Ogre::Quaternion(Ogre::Degree(rot.z), Ogre::Vector3::UNIT_Z);
        mEntity->attachObjectToBone("palma", mWeaponEnt, rotQuat, offset);

        // update the texts
        Ogre::String str = "Rotation: " + Ogre::StringConverter::toString(rot.x) +
            ", " + Ogre::StringConverter::toString(rot.y) + ", " +
            Ogre::StringConverter::toString(rot.z);
        mTextTable.setText(0, str);
        str = "Position: " + Ogre::StringConverter::toString(offset.x) +
            ", " + Ogre::StringConverter::toString(offset.y) + ", " +
            Ogre::StringConverter::toString(offset.z);
        mTextTable.setText(1, str);
    }
    mOffsetPos = offset;
    mOffsetRot = rot;
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
,   mOffsetPos(Ogre::Vector3::ZERO)
,   mOffsetRot(Ogre::Vector3::ZERO)
,   mWeaponEnt(0)
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

    // Load the text tables
    // first position will be rotation
    // last position will be position
    mTextTable.setNumRows(2, 0.01, 0, 0);
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

    // update the attaching position
    handleAttachingInput();

    if (mAnim)    mAnim->addTime(cz::GlobalData::lastTimeFrame);
}

}
