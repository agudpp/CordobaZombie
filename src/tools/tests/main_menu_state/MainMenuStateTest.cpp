/*
 * ResourcesLoadingTest.cpp
 *
 *  Created on: 1/11/2013
 *      Author: raul
 */

#include <memory>
#include <cstring>
#include <functional>

#include <global_data/GlobalData.h>
#include <os_utils/OSHelper.h>
#include <OgreFontManager.h>
#include <game_states/MainStateInformation.h>
#include <game_states/IMainState.h>
#include <game_states/states/MenuMainState/MenuMainState.h>
#include <game_states/states/MenuMainState/helper/MainMenuHelper.h>

#include "MainMenuStateTest.h"

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
    buttons.push_back(input::KeyCode::KC_L);
    buttons.push_back(input::KeyCode::KC_U);

    return buttons;
}

}

namespace tests {

////////////////////////////////////////////////////////////////////////////////
void
MainMenuStateTest::loadButtonFromOverlay(void)
{
    // Load the resources where the overlay is
    rrh::ResourceGroup rg;
    Ogre::String rscpath;
    mRcHandler.getResourceFullPath("test/main_menu_state/main/resources.cfg", rscpath);
    rg.setOgreResourceFile(rscpath);
    mRcHandler.loadResourceGroup(rg);

    // get the overlay
    Ogre::Overlay* overlay = Ogre::OverlayManager::getSingleton().getByName("MainMenuOverlay");
    if (overlay == 0) {
        debugERROR("NO OVERLAY FOUND WITH NAME " "MainMenuOverlay" "\n");
        return;
    }
    overlay->show();

    // now load the button using the helper
    static ui::FESimpleButton button;
    if (!cz::MainMenuHelper::configureButton(button, overlay, "MainMenuTest/Button1")) {
        debugERROR("Error when configuring the button\n");
        return;
    }

    // set the callback to the dummy method
    button.setButtonPressedCallback(std::bind(&MainMenuStateTest::dummy,
                                              this,
                                              std::placeholders::_1,
                                              std::placeholders::_2));

    mFrontEndManager.add(&button);

    // everything fine
    debugGREEN("Button loaded correctly\n");
}

////////////////////////////////////////////////////////////////////////////////
void
MainMenuStateTest::dummy(ui::FESimpleButton*, ui::FESimpleButton::Event)
{
    debugBLUE("CALLING THIS METHOD\n");
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MainMenuStateTest::MainMenuStateTest() :
    core::AppTester(cz::GlobalData::lastTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mMainState(0)
,   mFrontEndManager(mInputHelper, mMouseCursor)
{
    cz::GlobalData::camera = mCamera;
    cz::GlobalData::sceneMngr = mSceneMgr;

    cz::OgreCommon ogreInfo;
    ogreInfo.camera = cz::GlobalData::camera;
    ogreInfo.sceneMngr = cz::GlobalData::sceneMngr;
    ogreInfo.renderWindow = mWindow;
    ogreInfo.root = mRoot;

    // Set state info
    cz::IMainState::setOgreData(ogreInfo);

    cz::CommonHandlers handlers;
    handlers.frontEndManager = &mFrontEndManager;
    cz::IMainState::setCommonHandlers(handlers);

    mMouseCursor.setCursor(ui::MouseCursor::Cursor::NORMAL_CURSOR);
    mMouseCursor.setVisible(true);
    mMouseCursor.setWindowDimensions(mWindow->getWidth(), mWindow->getHeight());


    // Set resource Handler for intro state
    char *ENV = 0;
    ASSERT(core::OSHelper::getEnvVar("CZ01_RC_PATH",ENV));
    mRcHandler.setResourceRootPath(std::string(ENV));
    cz::IMainState::setRcHandler(&mRcHandler);

    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);

    mMainState = new cz::MenuMainState();

    initializeState();

}
////////////////////////////////////////////////////////////////////////////////
MainMenuStateTest::~MainMenuStateTest()
{
    delete mMainState;
}
////////////////////////////////////////////////////////////////////////////////
bool
MainMenuStateTest::initializeState(void)
{
    if (!mMainState) {
        debugERROR("NULL state\n");
        return false;
    }

    cz::MainStateInformation info;
    if (!mMainState->configureState(info)) {
        debugERROR("Can't configure state\n");
        return false;
    }

    cz::ResourceGroupList rgl;
    if (!mMainState->getResourcesToLoad(rgl)) {
        debugERROR("Can't get list of resources to load.\n");
        return false;
    }

    for (rrh::ResourceGroup &rg : rgl) {
        if (!mRcHandler.loadResourceGroup(rg)) {
            debugERROR("Can't load resource group.\n");
            return false;
        }
    }

    if (!mMainState->readyToGo()) {
        debugERROR("Can't prepare the state to go.\n");
        return false;
    }

    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
MainMenuStateTest::closeState(void)
{
    if (!mMainState) {
        debugERROR("NULL state\n");
        return false;
    }

    if (!mMainState->unload()) {
        debugERROR("Can't unload state.\n");
        return false;
    }

    cz::ResourceGroupList rgl;
    if (!mMainState->getResourcesToUnload(rgl)) {
        debugERROR("Can't get list of resources to unload.\n");
        return false;
    }

    for (rrh::ResourceGroup &rg : rgl) {
        mRcHandler.unloadResourceGroup(rg);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
MainMenuStateTest::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) {
        iter.getNext()->load();
    }

//    loadButtonFromOverlay();
}

///////////////////////////////////////////////////////////////////////////////
void
MainMenuStateTest::update()
{
    // update the input system
    mInputHelper.update();

    // update mouse cursor
   mMouseCursor.updatePosition(input::Mouse::absX(), input::Mouse::absY());


    int err = mMainState->update(cz::GlobalData::lastTimeFrame);

    if (!err || mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;

        if (!closeState()) {
            debugERROR("Problem while closing state.\n");
        }

        return;
    }


    // update the front end manager
    mFrontEndManager.update();

}

}
