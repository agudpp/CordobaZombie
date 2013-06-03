/*
 * Blood.h
 *
 *  Created on: Apr 1, 2012
 *      Author: Emi
 */

#ifndef TEST_H_
#define TEST_H_

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>

#include "AppTester.h"
#include "DebugUtil.h"
#include "MouseCursor.h"
#include "EffectsManager.h"

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

	//Prueba de efectos




private:
	f_e::MouseCursor				mMouseCursor;
	effects::EffectsManager effectMgr;

};

#endif /* TEST_H_ */
