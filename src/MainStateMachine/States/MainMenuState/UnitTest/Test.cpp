/*
 * Test.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreSceneNode.h>
#include <iostream>
#include <assert.h>
#include <OgreSceneQuery.h>
#include <OgreRay.h>
#include <OgreManualObject.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreString.h>

#include <auto_ptr.h>

#include "GlobalObjects.h"
#include "Test.h"
#include "Util.h"
#include "IMenu.h"
#include "OverlayEffect.h"

Test::Test() :
mTimer(0)
{
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight());

}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}



// handle input
void Test::handleInput(void)
{

}


/* Load additional info */
void Test::loadAditionalData(void)
{
	mMenuManager.build(GLOBAL_WINDOW->getWidth(),GLOBAL_WINDOW->getHeight(),
				5,5);
	IMenu::setMenuManager(&mMenuManager);
	OvEff::OverlayEffect::setManager(&mEffectManager);

	mCreditsState.setFilename("Configurations.xml");
	mCreditsState.load();

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	handleInput();

//	mTimer += GLOBAL_TIME_FRAME;
//	if(mTimer < 1.0f){
//		mCreditsState.update(mm_states::IState::ENTERING);
//	} else if(mTimer < 5.0f){
//		mCreditsState.update(mm_states::IState::LOOPING);
//	} else if(mTimer < 7.0f) {
//		mCreditsState.update(mm_states::IState::EXITING);
//	} else {
//	}

	mMenuManager.update();
	mEffectManager.update();

}
