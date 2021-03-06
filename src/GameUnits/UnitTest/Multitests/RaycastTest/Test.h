/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
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

#include "ZombieUnit.h"
#include "PlayerUnit.h"
#include "PlayerUnit/PlayerGroup/PlayerGroup.h"
#include "UpdObjsManager.h"
#include "Bomb.h"
#include "Shoot.h"

#include <vector>

#include "AppTester.h"
#include "DebugUtil.h"
#include "PrimitiveDrawer.h"
#include "MouseCursor.h"
#include "Backpack.h"
#include "MenuManager.h"
#include "CivilUnit.h"


struct ZombiePair{
	ZombieUnit *zu;
	sm::Vector2 desTpos;
};

//#include "TestUnit.h"

class Test : public AppTester
{
public:
	Test();

	/* Load additional info */
	void loadAditionalData(void);

	/* function called every frame. Use GlobalObjects::lastTimeFrame */
	void update();


	~Test();

private:
	/// performe a ray from the position of the mouse
	bool performMouseRay();

	void createLevelManager(void);

	// create game objects
	void createPlayer(void);
	void createZombies(void);
	void createCivils(void);
	void createBombs(void);
	void createCollectable(void);


	// create billboardset
	void createBillboardSet(void);

	// handle input
	void handleInput(void);




private:
	LevelManager	mLevelManager;
	bool 		mKeyPressed;
	int			mPlacesCount;
	MouseCursor				mMouseCursor;
//	CameraController		mCamController;
//	MouseCursor				mMouseCursor;
	std::vector<ZombiePair> mGameObjs;
	std::vector<ZombieUnit*> mZombies;
	std::vector<PlayerUnit*> mPlayers;
	std::vector<CivilUnit*>	mCivils;
	PlayerGroup				*mPlayerGroup;
	Ogre::BillboardSet		*mBillboardSet;
	Bomb					*mBomb;
	CollectableObject		*mCollectable;

	UpdObjsManager			mUpdMngr;
	MenuManager				mMenuManager;


//	TestUnit				mTestUnit;

};

#endif /* TEST_H_ */
