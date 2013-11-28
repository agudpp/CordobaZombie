/*
 * FrontEndTester.cpp
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
#include <functional>

#include <tinyxml/tinyxml.h>

#include <debug/PrimitiveDrawer.h>
#include <xml/XMLHelper.h>
#include <fx/effects/billboards/BillboardLoader.h>

#include "FrontEndTester.h"


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
    buttons.push_back(input::KeyCode::KC_3);
    buttons.push_back(input::KeyCode::KC_4);
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
FrontEndTester::loadFrontEndElements(void)
{
    // create the main overlay element for the FrontEnd
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
    Ogre::Overlay* overlay = om.getByName("FrontEndTest");
    Ogre::PanelOverlayElement* panel =
        static_cast<Ogre::PanelOverlayElement*>(overlay->getChild("Test/Button1"));
    mButton = new ui::FESimpleButton(&mFrontEndManager, core::AABB(0.5,0.5,0.4,0.8));

    overlay->show();
    panel->show();


    // configure the uv coords
    core::StackVector<ui::UVCoord, 4> coords;
    coords.push_back(ui::UVCoord(0, 0.333333f, 0.f, 1.f));
    coords.push_back(ui::UVCoord(0.333333f, 0.66666, 0.f, 1.f));
    coords.push_back(ui::UVCoord(0.6666f, 1.f, 0.f, 1.f));
    coords.push_back(ui::UVCoord(0.6666f, 1.f, 0.f, 1.f));
    mButton->build(panel, coords);
    mFrontEndManager.add(mButton);

    // callback element
    struct CBElement {
        void fun(ui::FESimpleButton* b, ui::FESimpleButton::Event e) {
            debugBLUE("Button event pressed: %d\n", e);
        }
    };
    static CBElement cb;
    mButton->setButtonPressedCallback(std::bind(&CBElement::fun,
                                               &cb,
                                               std::placeholders::_1,
                                               std::placeholders::_2));
}

////////////////////////////////////////////////////////////////////////////////
void
FrontEndTester::configureEffects(void)
{
    // configure alpha
    mAlphaEffect.setTime(5);


    // configure slide
    mSlideEffect.setDuration(5);
    mSlideEffect.setTranslationPositions(core::Vector2(0,0),
                                         core::Vector2(0.8,0.8));
}

////////////////////////////////////////////////////////////////////////////////
void
FrontEndTester::handleCameraInput()
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

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_LEFT)) {
        mTranslationVec.x -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_RIGHT)) {
        mTranslationVec.x += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_UP)) {
        mTranslationVec.z -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_DOWN)) {
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
//    if (mInputHelper.isKeyPressed(input::KeyCode::KC_1)) {
//        mOrbitCamera.setCameraType(tool::OrbitCamera::CameraType::FreeFly);
//    } else if (mInputHelper.isKeyPressed(input::KeyCode::KC_2)) {
//        mOrbitCamera.setCameraType(tool::OrbitCamera::CameraType::Orbit);
//    }

}

FrontEndTester::FrontEndTester() :
    core::AppTester(cz::GlobalData::lastTimeFrame)
,   mNode(0)
,   mEntity(0)
,   mOrbitCamera(mCamera, mSceneMgr, cz::GlobalData::lastTimeFrame)
,   mSelHelper(*mSceneMgr, *mCamera, mMouseCursor)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mFrontEndManager(mInputHelper, mMouseCursor)
,   mButton(0)
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

FrontEndTester::~FrontEndTester()
{
    // TODO Auto-generated destructor stub
}

/* Load additional info */
void
FrontEndTester::loadAditionalData(void)
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
        200000, 20, 20, true, 1, 9000, 9000, Ogre::Vector3::UNIT_Y);
    // Create an entity (the floor)
    Ogre::Entity *ent = mSceneMgr->createEntity("floor", "FloorPlane");
    ent->setMaterialName("GrassSample");
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);

    mSelHelper.setBaseIntersectionPlane(p);

    // load front end objects
    loadFrontEndElements();

    // configure effects
    configureEffects();
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
FrontEndTester::update()
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

    // apply some logic here
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_1)) {
        static bool active = false;
        mButton->setActive(active);
        debugGREEN("Setting active to %d\n", active);
        active = !active;
    }
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_2)) {
        static bool visible = false;
        mButton->setVisible(visible);
        debugGREEN("Setting visible to %d\n", visible);
        visible = !visible;
    }
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_3)) {
        static bool enable = false;
        mButton->setEnabled(enable);
        debugGREEN("Setting enable to %d\n", enable);
        enable = !enable;
    }

    // Apply effects
    if(mInputHelper.isKeyReleased(input::KeyCode::KC_UP)) {
        debugGREEN("Setting complement of Slide\n");
        mSlideEffect.complement();
        mButtonEffect.setFESimpleButton(mButton);
        mButtonEffect.setOverlayEffect(&mSlideEffect);
        mEffectHandler.add(&mButtonEffect);
    } else if (mInputHelper.isKeyReleased(input::KeyCode::KC_DOWN)) {
        debugGREEN("Setting complement of alpha effect\n");
        mAlphaEffect.complement();
        mButtonEffect.setFESimpleButton(mButton);
        mButtonEffect.setOverlayEffect(&mAlphaEffect);
        mEffectHandler.add(&mButtonEffect);
    }


    // update the camera
    handleCameraInput();

    // update the front end manager
    mFrontEndManager.update();

    // update the effects
    mEffectHandler.update(cz::GlobalData::lastTimeFrame);

}

}
