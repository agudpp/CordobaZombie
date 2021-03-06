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
#include "Fade.h"


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
	OvEff::Fade					*mFadeEffect;
};

#endif /* TEST_H_ */
