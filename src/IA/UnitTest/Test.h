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

#include "AppTester.h"
#include "DebugUtil.h"

#include "CameraController.h"
#include "MouseCursor.h"

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
	CameraController		mCamController;
	MouseCursor				mMouseCursor;

};

#endif /* TEST_H_ */
