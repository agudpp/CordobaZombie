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
#include <OgreOverlayElement.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayManager.h>

#include "AppTester.h"
#include "DebugUtil.h"
#include "MouseCursor.h"
#include "CreditsState.h"
#include "MenuManager.h"
#include "OverlayEffectManager.h"
#include "MainMenuState.h"


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

	// simulate MainMenuState run
	void simulateMainMenuStateRun(void);

private:
	MouseCursor				mMouseCursor;
	float					mTimer;
	MenuManager				mMenuManager;
	OvEff::OverlayEffectManager mEffectManager;


};

#endif /* TEST_H_ */
