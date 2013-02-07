/*
 * GameLoopState.cpp
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#include "GameLoopState.h"

#include <OgreTimer.h>
#include <OgreWindowEventUtilities.h>

#include "MicroAppRunner.h"
#include "UpdObjsManager.h"
#include "LoaderManager.h"
#include "LoaderData.h"
#include "MenuManager.h"
#include "LevelManager.h"
#include "PlayerUnit.h"
#include "GameUnit.h"
#include "GlobalObjects.h"
#include "MainStateMachineDefs.h"
#include "CameraController.h"
#include "Util.h"
#include "MouseCursor.h"





const char *GameLoopState::CAMERA_INTRO_FILENAME = "CameraIntro.xml";


////////////////////////////////////////////////////////////////////////////////
Ogre::AnimationState *
GameLoopState::getLevelAnimation(void) const
{
    return Common::Util::getAnimationFromFile(GLOBAL_SCN_MNGR,
                                              mCameraController->getCameraSceneNode(),
                                              CAMERA_INTRO_FILENAME);
}

////////////////////////////////////////////////////////////////////////////////
void
GameLoopState::setupScene(void)
{
    // check if we have any animation to reproduce?
    Ogre::AnimationState *introAnim = getLevelAnimation();
    if (introAnim != 0){
        // reproduce the animation
        mCameraController->reproduceAnimation(introAnim);
        mState = State::IntroVideo;

        // hide the mouse cursor
        GLOBAL_CURSOR->setVisible(false);
    }

    // TODO: load all the mission specific logic here (if there aren't a loader
    // that loads that already).
}

////////////////////////////////////////////////////////////////////////////////
void
GameLoopState::mainLoop(void)
{
	updateInput();
	updateGUISystem();
	updateIA();
	updateExtras();
}

/**
 * Main loop auxiliar functions
 */
void
GameLoopState::updateInput(void)
{
	// TODO: aca tenemos que verificar si seguimos ejecutando o no el mainloop
	// y por ende modificar la variable mRunning
}
void
GameLoopState::updateGUISystem(void)
{
	ASSERT(mMenuManager);
	mMenuManager->update();
}
void
GameLoopState::updateIA(void)
{
	for(size_t i = 0, size = mGameUnits.size(); i < size; ++i) {
	    mGameUnits[i]->update();
	}
}
void
GameLoopState::updateExtras(void)
{
	ASSERT(mUpdatableObjsManager);
	mUpdatableObjsManager->updateAllObjects();
}


/**
 * This function is called once the frame finish and here we have to execute
 * all the "MicroApps" (or whatever) that we don't want to measure the time
 * (affect the time frame).
 */
void
GameLoopState::serveExtraApps(void)
{
	if(!MicroAppRunner::hasMicroAppToRun()) return;

	// else we call the system
}


/**
 * Unload all the resources of the level
 */
void GameLoopState::unloadResources(void)
{
	debugERROR("TODO\n");
}


GameLoopState::GameLoopState() :
mLoaderManager(0),
mLevelManager(0),
mMenuManager(0),
mUpdatableObjsManager(0),
mCameraController(0)
{
	// TODO Auto-generated constructor stub

}

GameLoopState::~GameLoopState()
{
	// TODO Auto-generated destructor stub
}


/**
 * Set/Get the loading system used for this level
 */
void
GameLoopState::setData(GameLoopData *lm)
{
    ASSERT(lm);

    mLevelManager = lm->levelManager;
    ASSERT(mLevelManager);

    mLoaderManager = lm->loaderManager;
    ASSERT(mLoaderManager);

    mMenuManager = lm->menuManager;
    ASSERT(mMenuManager);

    mUpdatableObjsManager = lm->updatableObjsManager;
    ASSERT(mUpdatableObjsManager);

    // get the units
    LoaderData &loadedData = mLoaderManager->getLoaderData();
    ASSERT(loadedData.playerUnits);
    ASSERT(loadedData.gameUnits);
    mGameUnits.reserve(loadedData.playerUnits->size());
    for(size_t i = 0, size = loadedData.playerUnits->size(); i < size; ++i) {
        mGameUnits.push_back((*loadedData.playerUnits)[i]);
    }
    mGameUnits.reserve(mGameUnits.capacity() + loadedData.gameUnits->size());
    for(size_t i = 0, size = loadedData.gameUnits->size(); i < size; ++i) {
        mGameUnits.push_back((*loadedData.gameUnits)[i]);
    }

    mCameraController = lm->cameraController;
    ASSERT(mCameraController);

    // TODO: continue setting the other things here.

}




////////////////////////////////////////////////////////////////////////////
/////					IMainState Functions							////
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void
GameLoopState::getResources(ResourcesInfoVec &resourcesList,
                            const MainMachineInfo &info) const
{
    resourcesList.clear();

    // we must have the LEVEL_PATH here
    const MainMachineParams &params = info.params;
    ASSERT(params.find("LEVEL_PATH") != params.end());

    std::string levelPath = params.find("LEVEL_PATH")->second;
    if (levelPath[levelPath.size()-1] != '/') {
        levelPath += '/';
    }
    levelPath += "GameLoopState/resources.cfg";
    ResourcesInfo rinfo;
    rinfo.filePath = levelPath;
    rinfo.groupNames.push_back("GameLoopState");
    resourcesList.push_back(rinfo);

    debugERROR("Complete here the correct path\n");
}

////////////////////////////////////////////////////////////////////////////////
void
GameLoopState::enter(const MainMachineInfo &info)
{
	mInfo = info;
	mState = State::None;

	// setup the scene
	setupScene();
}

////////////////////////////////////////////////////////////////////////////////
MainMachineEvent
GameLoopState::update(MainMachineInfo &info)
{
	Ogre::Timer timer;
	float timeStamp = 0;

	// here is the main loop
	// TODO: fix the FrameElapsedTime and check how to get the ogres one.
	mRunning = true;

	while(mRunning) {
		timeStamp = timer.getMilliseconds();

		// render the frame
		if(!GLOBAL_ROOT->renderOneFrame())
			break;

		// execute the main loop
		mainLoop();

		// This must be called when we use the renderOneFrame approach
		Ogre::WindowEventUtilities::messagePump();

		// update the frame time
		Common::GlobalObjects::lastTimeFrame = (timer.getMilliseconds() -
				timeStamp) * 0.001f;

		// serve the extra apps... if we need to
		serveExtraApps();
	}

}

/**
 * Function called when the state is not "the actual" anymore
 */
void
GameLoopState::exit(void)
{

}



