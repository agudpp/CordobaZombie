/*
 * EffectsPlayer.cpp
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

#include "EffectsPlayer.h"


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

}


namespace tests {

////////////////////////////////////////////////////////////////////////////////
void
EffectsPlayer::loadBillboardGroups(void)
{
    TiXmlDocument* doc = core::XMLHelper::loadXmlDocument("BillboardLoader.xml");
    if (!doc) {
        debugERROR("Error loading .xml file\n");
        return;
    }
    if (cz::BillboardLoader::loadAndConfigure(doc->RootElement(), mBillboardGroups)) {
        debugBLUE("BillboardGroup loaded OK\n");
    } else {
        debugERROR("BillboardGroup loaded with some errors\n");
    }
    delete doc;

    /*
    cz::BillboardStack* stack1 = new cz::BillboardStack;
    cz::BillboardStack* stack2 = new cz::BillboardStack;

    stack1->setVisible(true);
    stack2->setVisible(true);

    // configure stack
    cz::BillboardStack::Args args;
    args.materialName = "MouseCursors";
    args.direction = Ogre::Vector3::UNIT_Z;
    args.upVector = Ogre::Vector3::UNIT_Y;
    args.width = 15.f;
    args.height = 12.f;
    args.numBillboards = 15;
    stack1->configure(args);

    args.materialName = "blood_particle";
    args.width = 5.f;
    args.height = 7.f;
    stack2->configure(args);

    cz::BillboardGroupHandler::GroupInfo gi;
    gi.coords = cz::UVCoord(0.f, 0.25f, 0.0f, 1.f);
    gi.stack = stack1;
    mBillboardGroups.addGroup(gi);
    gi.coords = cz::UVCoord(0.25f, 0.50f, 0.0f, 1.f);
    gi.stack = stack1;
    mBillboardGroups.addGroup(gi);
    gi.coords = cz::UVCoord(0.50f, 0.75f, 0.0f, 1.f);
    gi.stack = stack1;
    mBillboardGroups.addGroup(gi);
    gi.coords = cz::UVCoord(0.75f, 1.0f, 0.0f, 1.f);
    gi.stack = stack1;
    mBillboardGroups.addGroup(gi);
    gi.coords = cz::UVCoord(0.f, 1.0f, 0.0f, 1.f);
    gi.stack = stack2;
    mBillboardGroups.addGroup(gi);*/

}

////////////////////////////////////////////////////////////////////////////////
void
EffectsPlayer::handleCameraInput()
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

EffectsPlayer::EffectsPlayer() :
    core::AppTester(cz::GlobalData::lastTimeFrame)
,   mNode(0)
,   mEntity(0)
,   mOrbitCamera(mCamera, mSceneMgr, cz::GlobalData::lastTimeFrame)
,   mSelHelper(*mSceneMgr, *mCamera, mMouseCursor)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
{
    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);

    // configure global data
    cz::GlobalData::sceneMngr = mSceneMgr;

    mMouseCursor.setCursor(ui::MouseCursor::Cursor::NORMAL_CURSOR);
    mMouseCursor.setVisible(true);
    mMouseCursor.setWindowDimensions(mWindow->getWidth(), mWindow->getHeight());


}

EffectsPlayer::~EffectsPlayer()
{
    // TODO Auto-generated destructor stub
}

/* Load additional info */
void
EffectsPlayer::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // everything fine... configure the blood effect
    cz::BloodParticles* bloodElems = mBloodQueue.getEffects();
    for (unsigned int i = 0, size = mBloodQueue.numAllocatedEffects(); i < size; ++i) {
        bloodElems[i].construct();
        bloodElems[i].configure(Ogre::Vector3::ZERO, Ogre::Vector3::ZERO, 15.f);
    }

    // load floor
    Ogre::Plane p;
    p.normal = Ogre::Vector3(0, 0, 1);
    p.d = 0;
    Ogre::MeshManager::getSingleton().createPlane("FloorPlane",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, p, 200000,
        200000, 20, 20, true, 1, 9000, 9000, Ogre::Vector3::UNIT_Y);
    // Create an entity (the floor)
    Ogre::Entity *ent = mSceneMgr->createEntity("floor", "FloorPlane");
    ent->setMaterialName("GrassSample");
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);

    // load billboards
    loadBillboardGroups();
    mSelHelper.setBaseIntersectionPlane(p);
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
EffectsPlayer::update()
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

    // if we press the space we want to show the blood effect
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_SPACE)) {
        cz::BloodParticles* effect = mBloodQueue.getAvailable();
        if (effect) {
            mEffectHandler.add(effect);
        }
    }

    // if we click the floor we will show a blood or some thing else
    if (mInputHelper.isMouseReleased(input::MouseButtonID::MB_Left)) {
        // check if we hit some position
        Ogre::Vector3 hitPos;
        if (mSelHelper.getPlaneIntersection(hitPos)) {
            debugGREEN("Showing billboard\n");
            unsigned int random = std::rand() % mBillboardGroups.groups().size();
            hitPos.z += 0.5f;
            mBillboardGroups.get(random, hitPos, Ogre::Vector3::UNIT_Z);
        }
    }


    // update the camera
    handleCameraInput();

    mEffectHandler.update(cz::GlobalData::lastTimeFrame);
}

}
