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
//#include "CameraController.h"



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
	// Add corners to a box
	void showCorners(const Ogre::AxisAlignedBox &b);

	// handle input
	void handleInput(void);

private:
	Ogre::SceneNode *mEntNode;
};

#endif /* TEST_H_ */
