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

#include <vector>

#include "AppTester.h"
#include "DebugUtil.h"
#include "PrimitiveDrawer.h"
#include "MouseCursor.h"
#include "LevelManager.h"

#include "ZombieUnit.h"


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

	void createLevelManager(void);
	void loadLevelMeshes(void);

	// create game objects
	void createRealZombies(void);
	void createZombieTarget(void);

	// handle input
	void handleInput(void);

private:
	LevelManager	mLevelManager;
	bool 		mKeyPressed;
	int			mPlacesCount;
//	CameraController		mCamController;
//	MouseCursor				mMouseCursor;
	std::vector<ZombiePair> mGameObjs;
	std::vector<ZombieUnit*> mZombies;
	GameUnit *mPlayer;

//	TestUnit				mTestUnit;

};

#endif /* TEST_H_ */
