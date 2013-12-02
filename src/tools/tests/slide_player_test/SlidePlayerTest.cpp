/*
 * SlidePlayerTest.cpp
 *
 *  Created on: 1/11/2013
 *      Author: agustin
 */


#include <memory>
#include <cstring>


#include <global_data/GlobalData.h>
#include <os_utils/OSHelper.h>
#include <OgreFontManager.h>
#include <game_states/MainStateInformation.h>
#include <game_states/IMainState.h>
#include <game_states/states/IntroMainState/IntroMainState.h>

#include "SlidePlayerTest.h"


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
    buttons.push_back(input::KeyCode::KC_LEFT);
    buttons.push_back(input::KeyCode::KC_RIGHT);

    return buttons;
}

}


namespace tests {

////////////////////////////////////////////////////////////////////////////////
void
SlidePlayerTest::configureSlidePlayer(void)
{
    // add the resource location
    Ogre::String fullPath;
    mRcHandler.getResourceFullPath("test/slide_player/resources.cfg", fullPath);
    rrh::ResourceGroup rg;
    rg.setOgreResourceFile(fullPath);
    if (!mRcHandler.loadResourceGroup(rg)) {
        debugERROR("Error loading %s\n", fullPath.c_str());
    }

    // configure the slide player
    cz::SlidePlayer::Info info;
    info.effectHandler = &mEffectHandler;
    info.slideTime = 1.f;

    if (!mSlidePlayer.configure(info)) {
        debugERROR("Error configuring the slide player\n");
        return;
    }

    // set the slides
    cz::SlidePlayer::SlidesVec slides;
    slides.push_back("SlidePlayerTest/Slide1");
    slides.push_back("SlidePlayerTest/Slide2");
    slides.push_back("SlidePlayerTest/Slide3");
    slides.push_back("SlidePlayerTest/Slide4");
    slides.push_back("SlidePlayerTest/Slide5");
    if (!mSlidePlayer.setSlides(slides)) {
        debugERROR("Error setting the slides\n");
        return;
    }

    mSlidePlayer.setVisible(true);
}

////////////////////////////////////////////////////////////////////////////////
SlidePlayerTest::SlidePlayerTest() :
    core::AppTester(cz::GlobalData::lastTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mFrontEnd(mInputHelper, mCursor)
{
    cz::GlobalData::camera = mCamera;
    cz::GlobalData::sceneMngr = mSceneMgr;

    // Set resource Handler for intro state
    char *ENV = 0;
    ASSERT(core::OSHelper::getEnvVar("CZ01_RC_PATH",ENV));
    mRcHandler.setResourceRootPath(std::string(ENV));

    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);
}


////////////////////////////////////////////////////////////////////////////////
SlidePlayerTest::~SlidePlayerTest()
{
}



////////////////////////////////////////////////////////////////////////////////
void
SlidePlayerTest::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }


    // Load the global resource file
    Ogre::String fullPath;
    mRcHandler.getResourceFullPath("./GlobalResources.cfg", fullPath);
    rrh::ResourceGroup rg;
    rg.setOgreResourceFile(fullPath);
    if (!mRcHandler.loadResourceGroup(rg)) {
        debugERROR("Error loading %s\n", fullPath.c_str());
    }

    configureSlidePlayer();
}


///////////////////////////////////////////////////////////////////////////////
void
SlidePlayerTest::update()
{
    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

    // change the slides
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_LEFT)) {
        mSlidePlayer.prev();
    }
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_RIGHT)) {
        mSlidePlayer.next();
    }

    mEffectHandler.update(cz::GlobalData::lastTimeFrame);

}

}
