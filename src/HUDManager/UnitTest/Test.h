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
#include "MenuManager.h"
#include "MouseCursor.h"
#include "ExtraActionPanel.h"
#include "ExtraAction.h"
#include "PassportPicture.h"
#include "CellphoneMap.h"
#include "LevelManager.h"

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

	// configure the map
	void createLevelManager(void);
	void createPlayer(void);
	void configureMap(void);

	// handle input
	void handleInput(void);

private:
	bool 		mKeyPressed;
	int			mPlacesCount;
	MenuManager	mMenuManager;
	MouseCursor	mMouseCursor;
	ExtraActionPanel	mPanel;
	PassportPicture		mPassportPic;

	CellphoneMap		mMap;
	LevelManager		mLevelManager;
	std::vector<PlayerUnit *>	mPlayers;
};

#endif /* TEST_H_ */
