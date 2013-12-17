/*
 * GameRunner.cpp
 *
 *  Created on: Nov 28, 2013
 *      Author: agustin
 */

#include "GameRunner.h"

#include <OgreTimer.h>
#include <OgreRenderWindow.h>
#include <OgreWindowEventUtilities.h>
#include <OgreRoot.h>
#include <OgreString.h>

#include <debug/DebugUtil.h>
#include <input/InputMouse.h>
#include <input/InputKeyboard.h>
#include <global_data/GlobalData.h>
#include <sound/SoundHandler.h>
#include <video/OgreVideoPlayer.h>
#include <ResourceGroup.h>


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
    buttons.push_back(input::MouseButtonID::MB_Middle);

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
    buttons.push_back(input::KeyCode::KC_D);
    buttons.push_back(input::KeyCode::KC_LEFT);
    buttons.push_back(input::KeyCode::KC_RIGHT);
    buttons.push_back(input::KeyCode::KC_1);
    buttons.push_back(input::KeyCode::KC_2);
    buttons.push_back(input::KeyCode::KC_3);
    buttons.push_back(input::KeyCode::KC_Z);

    return buttons;
}

}


namespace cz {


////////////////////////////////////////////////////////////////////////////////
bool
GameRunner::initAll(void)
{
    return  initGlobalResources() &&
            initMouseCursor() &&
            initFrontEnd() &&
            initInputHelper() &&
            initGlobalData() &&
            initMainStateMachine();
}

////////////////////////////////////////////////////////////////////////////////
bool
GameRunner::initGlobalResources(void)
{
    rrh::ResourceHandler* rh = mEngine.resourcesData().handler;
    ASSERT(rh && "Something is not fine, we need to have at this point the "
        "ResourceHandler instance already allocated");

    Ogre::String fullPath;
    rh->getResourceFullPath("GlobalResources.cfg", fullPath);
    rrh::ResourceGroup rg;
    rg.setOgreResourceFile(fullPath);
    return rh->loadResourceGroup(rg);
}

////////////////////////////////////////////////////////////////////////////////
bool
GameRunner::initMouseCursor(void)
{
    // configure the mouse cursor
    mMouseCursor.build();
    mMouseCursor.setCursor(ui::MouseCursor::Cursor::NORMAL_CURSOR);
    mMouseCursor.setVisible(false);
    mMouseCursor.setWindowDimensions(mEngine.ogreData().renderWindow->getWidth(),
                                     mEngine.ogreData().renderWindow->getHeight());
    mMouseCursor.updatePosition(mEngine.ogreData().renderWindow->getWidth() / 2,
                                mEngine.ogreData().renderWindow->getHeight() / 2);

    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
GameRunner::initFrontEnd(void)
{
    // nothing to do
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
GameRunner::initInputHelper(void)
{
    // configure the input
    input::Mouse::setMouse(mEngine.inputData().mouse);
    input::Keyboard::setKeyboard(mEngine.inputData().keyboard);

    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
GameRunner::initGlobalData(void)
{
    GlobalData::camera = mEngine.ogreData().camera;
    GlobalData::sceneMngr = mEngine.ogreData().sceneMngr;
    GlobalData::rscHandler = mEngine.resourcesData().handler;
    GlobalData::lastTimeFrame = 0.f;

    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
GameRunner::initMainStateMachine(void)
{
    // set the global information for the states
    OgreCommon ogre;
    ogre.camera = mEngine.ogreData().camera;
    ogre.renderWindow = mEngine.ogreData().renderWindow;
    ogre.root = mEngine.ogreData().root;
    ogre.sceneMngr = mEngine.ogreData().sceneMngr;
    IMainState::setOgreData(ogre);

    CommonHandlers handlers;
    handlers.frontEndManager = &mFrontEndManager;
    handlers.inputHelper = &mInputHelper;
    handlers.mouseCursor = &mMouseCursor;
    handlers.effectHandler = &mEffectHandler;
    IMainState::setCommonHandlers(handlers);

    IMainState::setRcHandler(mEngine.resourcesData().handler);
    IMainState::setVideoPlayer(mEngine.videoData().player);
    IMainState::setSoundHandler(mEngine.soundData().handler);

    mStatesBuilder.setOgreCommon(ogre);

    return mTransitionTable.build(mStatesBuilder);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
GameRunner::GameRunner(engine::Engine& engine) :
    mEngine(engine)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mMouseCursor(false)
,   mFrontEndManager(mInputHelper, mMouseCursor)
,   mMainStateMachine(mTransitionTable, *engine.resourcesData().handler)
{

}

////////////////////////////////////////////////////////////////////////////////
GameRunner::~GameRunner()
{

}

////////////////////////////////////////////////////////////////////////////////
bool
GameRunner::run(void)
{
    // first of all we will initialize everything
    if (!initAll()) {
        debugERROR("Error initializing the game stuff\n");
        return false;
    }

    // now we will run forever until we exit the game
    mContinueRunning = true;
    Ogre::Timer timer;
    float timeStamp = 0;
    Ogre::Real accumTimer = 0.0f;
    unsigned int numFrames = 0;

    // Get pointers
    Ogre::Root* ogreRoot = mEngine.ogreData().root;
    mm::SoundHandler* soundHandler = mEngine.soundData().handler;
    mm::OgreVideoPlayer* videoPlayer = mEngine.videoData().player;

    // prepare the state machine
    mMainStateMachine.start();

    // here is the main loop
    while (mContinueRunning) {
        timeStamp = timer.getMilliseconds();

        // update the input helper stuff
        mInputHelper.update();

        // update the state machine here
        mContinueRunning = mMainStateMachine.update(GlobalData::lastTimeFrame);

        // update the handlers / helpers here if needed

        // mouse cursor
        if (mMouseCursor.isVisible()) {
            mMouseCursor.updatePosition(input::Mouse::absX(), input::Mouse::absY());
        }

        // update the effect handler
        mEffectHandler.update(GlobalData::lastTimeFrame);

        // front end manager (if no elements this will do nothing)
        mFrontEndManager.update();

        // sounds of the game (this is global, doesn't matter the current state
        // we can always be playing sounds, that's why we put this here)
        soundHandler->update(GlobalData::lastTimeFrame);

        // check if we have some video to reproduce
        if (videoPlayer->isPlaying() && videoPlayer->isVisible()) {
            videoPlayer->update(GlobalData::lastTimeFrame);
        }

        // render the frame
        if (!mContinueRunning || !ogreRoot->renderOneFrame()) {
            break;
        }

        // This must be called when we use the renderOneFrame approach
        Ogre::WindowEventUtilities::messagePump();

        // calculate the new time frame
        GlobalData::lastTimeFrame = (timer.getMilliseconds() - timeStamp) * 0.001;
    }


    return true;
}


} /* namespace cz */
