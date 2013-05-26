/*
 * Test.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreSceneNode.h>
#include <OgreMaterialManager.h>
#include <iostream>
#include <assert.h>


#include "Test.h"
#include "GlobalObjects.h"


/* Test the Image Shower MicroApp */
void Test::testImgShowerApp(void)
{
	ImgShowerApp app;
	app.setOverlayName("Examples/OgreDance");
	MicroAppRunner::runMicroApp(&app);
}


Test::Test()
{
	// TODO Auto-generated constructor stub

}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}


/* Load additional info */
void Test::loadAditionalData(void)
{
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_I)){
		testImgShowerApp();
	}

}
