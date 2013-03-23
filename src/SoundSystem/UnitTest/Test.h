/*
 * Test.h
 *
 *  Created on: May 8, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef TEST_H_
#define TEST_H_


#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>
#include <OgreBillboardSet.h>
#include <OgreBillboard.h>

#include <vector>


#include "AppTester.h"
#include "DebugUtil.h"
#include "PrimitiveDrawer.h"
#include "MouseCursor.h"
#include "LevelManager.h"
#include "MenuManager.h"

#include "SoundHandler.h"
#include "ZombieUnit.h"
#include "PlayerUnit.h"
#include "PlayerUnit/PlayerGroup/PlayerGroup.h"
#include "UpdObjsManager.h"
#include "Bomb.h"
#include "Shoot.h"


struct ZombiePair{
	ZombieUnit *zu;
	sm::Vector2 desTpos;
};



class Test : public AppTester
{
	static const float TIME_UPDATE_PERIOD = 1.0;

public:
	Test();
	virtual ~Test();

	/* Load additional info */
	void
	loadAditionalData(void);

	/* function called every frame. Use GlobalObjects::lastTimeFrame */
	void
	update();

private:
	void createLevelManager(void);

	// create game units & objects
	void createPlayer(void);
	void createZombies(void);
	void createBombs(void);
	void createCollectable(void);
	void createBillboardSet(void);

	// cast a ray from the position of the mouse
	bool performMouseRay(Ogre::Vector3 &v);
	// handle input
	void handleInput();

	// pause all unit sounds
	void pauseUnitsSounds();
	// play again all paused unit sounds
	void playUnitsSounds();
	// pause all environmental sounds
	void pauseEnvSounds();
	// play again all paused environmental sounds
	void playEnvSounds();

	// Test sets
	void testCollissionRaycast(void);
	void testCollectObject(void);
	void testStart(void);
	void testEngageEveryone(void);

private:
	LevelManager 				mLevelManager;
	bool 						mKeyPressed;
	int							mPlacesCount;
	MouseCursor					mMouseCursor;
	std::vector<ZombiePair>		mGameObjs;
	std::vector<ZombieUnit*>	mZombies;
	std::vector<PlayerUnit*>	mPlayers;
	PlayerGroup				   *mPlayerGroup;
	Ogre::BillboardSet		   *mBillboardSet;
	Bomb					   *mBomb;
	CollectableObject		   *mCollectable;
	UpdObjsManager				mUpdMngr;
	MenuManager					mMenuManager;
	SoundHandler&				mSoundHandler;
};


#endif /* TEST_H_ */
