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



	// handle input
	void handleInput(void);

private:
	bool 		mKeyPressed;
	int			mPlacesCount;
	MenuManager	mMenuManager;
	f_e::MouseCursor	mMouseCursor;
	ExtraActionPanel	mPanel;
};

#endif /* TEST_H_ */
