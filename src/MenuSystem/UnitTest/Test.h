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
#include "MenuManager.h"
#include "GUIBuilder.h"
#include "GUIManager.h"


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

	// create overlay uv test
	void createOverlay(void);

	// handle input
	void handleInput(void);

private:
	MouseCursor				mMouseCursor;
	MenuManager				mMenuManager;
	GUIManager				mGuiManager;
	GUIBuilder				mGUIBuilder;

	Ogre::PanelOverlayElement *mFader;


};

#endif /* TEST_H_ */
