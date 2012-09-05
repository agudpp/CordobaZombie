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
#include "Loader.h"




// create overlay uv test
void Test::createOverlay(void)
{

}


Test::Test()
{
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight());

}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}



void Test::loadInfostate(void)
{

	MainMachineInfo info;
	mFirstInfoState.enter(info);
	mFirstInfoState.update(info);
	mFirstInfoState.exit();
}

// handle input
void Test::handleInput(void)
{
	static bool pressed = false;
	static float counter = 0.0f;

//	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
//		if(!pressed){
//			pressed = true;
//			mBar.setState(counter);
//			counter += 0.1;
////			mBar.clear();
//			if(counter > 100.0f){
//				counter = 0.0f;
//			}
//		}
//	} else {
//		pressed = false;
//	}

}


/* Load additional info */
void Test::loadAditionalData(void)
{
	// positions of bar:
	// x:0,114068441 and y:0,371727749
//	mBar.setOverlayName("LoadingBarOverlay");
	loadInfostate();
	ASSERT(false);

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	handleInput();

}
