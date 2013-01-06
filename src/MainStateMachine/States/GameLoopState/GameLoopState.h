/*
 * GameLoopState.h
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#ifndef GAMELOOPSTATE_H_
#define GAMELOOPSTATE_H_

#include <vector>

#include <OgreAnimationState.h>

#include "IMainState.h"


// Forward declaration
class GameUnit;
class ZombieUnit;
class PlayerUnit;
class CivilUnit;
class UpdObjsManager;
class LoaderManager;
class LevelManager;
class MenuManager;
class CameraController;

class GameLoopState : public IMainState
{
    static const char * CAMERA_INTRO_FILENAME = "CameraIntro.xml";

	typedef std::vector<GameUnit*>			GameUnitVec;
public:
	struct GameLoopData {
		LoaderManager			*loaderManager;
		UpdObjsManager			*updatableObjsManager;
		MenuManager				*menuManager;
		LevelManager			*levelManager;
		CameraController        *cameraController;
	};
public:
	GameLoopState();
	virtual ~GameLoopState();

	/**
	 * Set the loading system used for this level
	 */
	void setData(GameLoopData *lm);



	////////////////////////////////////////////////////////////////////////////
	/////					IMainState Functions							////
	////////////////////////////////////////////////////////////////////////////

	/**
     * Function used to get the resources files used by the state.
     * The list returned is the list of the resources used by and only by this
     * state.
     */
    virtual void getResources(ResourcesInfoVec &resourcesList,
                              const MainMachineInfo &info) const;

	/**
	 * Entering the state with additional info
	 */
	virtual void enter(const MainMachineInfo &info);

	/**
	 * Update the state... This function will be called once.
	 * @param	info	The structure used to pass throw the states. If we want
	 * 					to fill it with some information then we can do it.
	 * @return	event	The event that was happend.
	 */
	virtual MainMachineEvent update(MainMachineInfo &info);

	/**
	 * Function called when the state is not "the actual" anymore
	 */
	virtual void exit(void);

	////////////////////////////////////////////////////////////////////////////


protected:

	/**
	 * Check if there are animation to reproduce for this level.
     * @return Anim in case there is an animation to reproduce.
     *         0 otherwise
	 */
	Ogre::AnimationState *getLevelAnimation(void) const;

	/**
	 * Setup the scene (configure the mouse cursor, initial animation? etc)
	 */
	virtual void setupScene(void);

	/**
	 * This function is called to update all the main logic of the game and
	 * measure the time to get the "LastTimeFrame".
	 */
	virtual void mainLoop(void);

	/**
	 * Main loop auxiliar functions
	 */
	virtual void updateInput(void);
	virtual void updateGUISystem(void);
	virtual void updateIA(void);
	virtual void updateExtras(void);


	/**
	 * This function is called once the frame finish and here we have to execute
	 * all the "MicroApps" (or whatever) that we don't want to measure the time
	 * (affect the time frame).
	 */
	virtual void serveExtraApps(void);

	/**
	 * Unload all the resources of the level
	 */
	virtual void unloadResources(void);

protected:
	LoaderManager			*mLoaderManager;
	UpdObjsManager			*mUpdatableObjsManager;
	MenuManager				*mMenuManager;
	LevelManager			*mLevelManager;
	MainMachineInfo			mInfo;
	GameUnitVec				mGameUnits;
	CameraController        *mCameraController;
	bool					mRunning;
};

#endif /* GAMELOOPSTATE_H_ */
