/*
 * ResourcesLoadingTest.cpp
 *
 *  Created on: 1/11/2013
 *      Author: raul
 */


#include <memory>
#include <cstring>


#include <global_data/GlobalData.h>
#include <os_utils/OSHelper.h>
#include <OgreFontManager.h>
#include <game_states/MainStateInformation.h>
#include <game_states/IMainState.h>
#include <game_states/states/IntroMainState/IntroMainState.h>

#include "IntroMainStateTest.h"


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



IntroMainStateTest::IntroMainStateTest() :
    core::AppTester(cz::GlobalData::lastTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mFrontEnd(mInputHelper, mCursor)
,   mOgreVideoPlayer(-1, 1, 1, -1, mSceneMgr, mWindow->getHeight(), mWindow->getWidth())
{

	debug("\n\nTESTING...\n\n");


	cz::GlobalData::camera = mCamera;
	cz::GlobalData::sceneMngr = mSceneMgr;

	cz::OgreCommon ogreInfo;
	ogreInfo.camera = cz::GlobalData::camera;
	ogreInfo.sceneMngr = cz::GlobalData::sceneMngr;
	ogreInfo.renderWindow = mWindow;
	ogreInfo.root = mRoot;

	// Set state info
	cz::IMainState::setOgreData(ogreInfo);
	cz::IMainState::setVideoPlayer(&mOgreVideoPlayer);

	cz::CommonHandlers handlers;
	handlers.frontEndManager = &mFrontEnd;
	handlers.inputHelper = &mInputHelper;
	handlers.effectHandler = &mEffectHandler;
	cz::IMainState::setCommonHandlers(handlers);

	// Set resource Handler for intro state
    char *ENV = 0;
    ASSERT(core::OSHelper::getEnvVar("CZ01_RC_PATH",ENV));
	mRcHandler.setResourceRootPath(std::string(ENV));
	cz::IMainState::setRcHandler(&mRcHandler);

    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);

    mIntroMainState = new cz::IntroMainState();

    debugGREEN("Ready to init the state\n");
    if (!initializeState()) {
        debugERROR("Error initializing the state\n");
    } else {
        debugGREEN("Everything loaded fine\n");
    }

}



IntroMainStateTest::~IntroMainStateTest()
{
    // TODO Auto-generated destructor stub
	delete mIntroMainState;
}



bool
IntroMainStateTest::initializeState(void)
{
	cz::MainStateInformation info;
	if(!mIntroMainState->configureState(info)){
		debugERROR("Can't configure state\n");
		return false;
	}

	cz::ResourceGroupList rgl;
	if(!mIntroMainState->getResourcesToLoad(rgl)){
		debugERROR("Can't get list of resources to load.\n");
		return false;
	}

	for( rrh::ResourceGroup &rg : rgl ){
		if(!mRcHandler.loadResourceGroup(rg)){
			debugERROR("Can't load resource group.\n");
			return false;
		}
	}

	if(!mIntroMainState->readyToGo()){
		debugERROR("Can't prepare the state to go.\n");
		return false;
	}

	Ogre::String path;
	if (! mRcHandler.getResourcePathSomeGroup(Ogre::String("menu.ogg"), path)){
		debug("getResourcePathSomeGroup may not be working\n");
	}else{
		debug("El path al menu.ogg: %s\n",path.c_str());
	}

	debugGREEN("Everything loaded right\n");
	return true;
}

bool
IntroMainStateTest::closeState(void)
{
	if(!mIntroMainState->unload()){
		debugERROR("Can't unload state.\n");
		return false;
	}

	cz::ResourceGroupList rgl;
	if(!mIntroMainState->getResourcesToUnload(rgl)){
		debugERROR("Can't get list of resources to unload.\n");
		return false;
	}

	for( rrh::ResourceGroup &rg : rgl ){
		mRcHandler.unloadResourceGroup(rg);
	}

	return true;
}





/* Load additional info */
void
IntroMainStateTest::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }
}


///////////////////////////////////////////////////////////////////////////////

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
IntroMainStateTest::update()
{

	int err = mIntroMainState->update(cz::GlobalData::lastTimeFrame);

	// update the input system
    mInputHelper.update();


    if (!err || mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;

        if(!closeState()){
        	debugERROR("Problem while closing state.\n");
        }

        return;
    }

    if (mOgreVideoPlayer.isPlaying() && mOgreVideoPlayer.isVisible()) {
        mOgreVideoPlayer.update(cz::GlobalData::lastTimeFrame);
    }

}

}
