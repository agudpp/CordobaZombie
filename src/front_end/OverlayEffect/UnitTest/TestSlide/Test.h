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
#include "OverlayEffectManager.h"
#include "Slide.h"


const float PI = 3.1415;

class myVfun: public OvEff::Slide::VelFunction
{
public:
	float operator()(float x) const
	{
		x *= 3.1415f;
		return (std::sin(x-(PI/2.0f))+1.0f)/2.0f;
	}
};

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

	// create overlay
	void createOverlay(void);

	// handle input
	void handleInput(void);

private:
	MouseCursor				mMouseCursor;
	OvEff::OverlayEffectManager	mOvEffMngr;
	Ogre::PanelOverlayElement	*mOverlayPanel;
	OvEff::Slide				*mSlideEffect;
	myVfun						mvfun;
};

#endif /* TEST_H_ */
