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

#include "UpdObjsManager.h"


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
	bool performMouseRay(Ogre::Vector3 &v);

	void createLevelManager(void);

	// handle input
	void handleInput(void);




private:
	LevelManager	mLevelManager;
	bool 		mKeyPressed;
	int			mPlacesCount;
	MouseCursor				mMouseCursor;
//	CameraController		mCamController;
//	MouseCursor				mMouseCursor;
	UpdObjsManager			mUpdMngr;


//	TestUnit				mTestUnit;

};

#endif /* TEST_H_ */
