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
#include "LoadingBar.h"
#include "LoadingState.h"
#include "MainStateMachineDefs.h"
#include "LoaderManager.h"


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

	// Load loading state
	void loadLoadingState(void);

	// handle input
	void handleInput(void);

private:
	MouseCursor				mMouseCursor;
	LoadingBar				mBar;
	LoadingState			mLoadingState;


};

#endif /* TEST_H_ */
