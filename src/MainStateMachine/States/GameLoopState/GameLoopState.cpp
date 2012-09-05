/*
 * GameLoopState.cpp
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#include <OgreTimer.h>
#include <OgreWindowEventUtilities.h>

#include "MicroAppRunner.h"
#include "UpdObjsManager.h"
#include "LoaderManager.h"
#include "MenuManager.h"
#include "LevelManager.h"
#include "GameUnit.h"
#include "GameLoopState.h"
#include "GlobalObjects.h"


/**
 * This function is called to update all the main logic of the game and
 * measure the time to get the "LastTimeFrame".
 */
void GameLoopState::mainLoop(void)
{
	updateInput();
	updateGUISystem();
	updateIA();
	updateExtras();
}

/**
 * Main loop auxiliar functions
 */
void GameLoopState::updateInput(void)
{
	// TODO: aca tenemos que verificar si seguimos ejecutando o no el mainloop
	// y por ende modificar la variable mRunning
}
void GameLoopState::updateGUISystem(void)
{
	ASSERT(mMenuManager);
	mMenuManager->update();
}
void GameLoopState::updateIA(void)
{
	for(int i = mGameUnits.size(); i >= 0; --i) mGameUnits[i]->update();
}
void GameLoopState::updateExtras(void)
{
	ASSERT(mUpdatableObjsManager);
	mUpdatableObjsManager->updateAllObjects();
}


/**
 * This function is called once the frame finish and here we have to execute
 * all the "MicroApps" (or whatever) that we don't want to measure the time
 * (affect the time frame).
 */
void GameLoopState::serveExtraApps(void)
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
mUpdatableObjsManager(0)
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
void GameLoopState::setData(GameLoopData *lm)
{

}
GameLoopState::GameLoopData *GameLoopState::getData(void)
{

}




////////////////////////////////////////////////////////////////////////////
/////					IMainState Functions							////
////////////////////////////////////////////////////////////////////////////

/**
 * Entering the state with additional info
 */
void GameLoopState::enter(const MainMachineInfo &info)
{
	mInfo = info;
}

/**
 * Update the state... This function will be called once.
 * @param	info	The structure used to pass throw the states. If we want
 * 					to fill it with some information then we can do it.
 * @return	event	The event that was happend.
 */
MainMachineEvent GameLoopState::update(MainMachineInfo &info)
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
void GameLoopState::exit(void)
{

}



