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

#include "BillboardBatery.h"
#include "BillboardManager.h"
#include "AppTester.h"
#include "DebugUtil.h"

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

	bool performMouseRay(Ogre::Vector3 &v);
	// handle input
	void handleInput(void);

private:
	MouseCursor				mMouseCursor;
	BillboardBatery			mBbBatery;
	BillboardManager		mBbManager;

};

#endif /* TEST_H_ */
