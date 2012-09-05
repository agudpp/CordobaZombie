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
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>

#include <auto_ptr.h>

#include "GlobalObjects.h"
#include "Test.h"
#include "Util.h"




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
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	Ogre::Overlay *ov = om.getByName("LoadingBarOverlay");
	ASSERT(ov);
	ov->show();
	Ogre::OverlayContainer *cont = static_cast<Ogre::OverlayContainer *>(
			om.getOverlayElement("LoadingBarOverlay/Background"));
	ASSERT(cont);

	mBar.setOverlayContainer(cont);
	mBar.setMaximumValue(100.0f);
	mBar.useSliceFeature(true);
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
	loadInfostate();
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	static float progress = 0.0f;

	progress += 0.1;
	if(progress >= 100.0f){
		progress = 0;
	}
	mBar.setActualValue(progress);

	handleInput();

}
