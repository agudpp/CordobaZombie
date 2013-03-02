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

#include "GlobalObjects.h"
#include "Test.h"
#include <DotSceneLoader.h>
#include <Util.h>

#define NUM_COL 6
#define NUM_ROW 5


// handle input
void Test::handleInput(void)
{
    static bool kpr = false;
    static bool kpr1 = false;
    static size_t counter = 0;
	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
		// we have to exit
		mStopRunning = true;
	}

	if (mKeyboard->isKeyDown(OIS::KC_1)){
	    if (!kpr){
	        kpr = true;
	        counter++;
	        if (counter >= NUM_ROW * NUM_COL) counter = 0;
	        mActualCol = counter % NUM_COL;
	        mActualRow = (counter / NUM_COL) % NUM_ROW;
	        debugYELLOW("col: %ul, row: %ul\n", mActualCol, mActualRow);

//	        mPassport.selectPicture(mActualCol, mActualRow);
	        mPassport.selectPicture(counter);
	    }
	} else {
	    kpr = false;
	}

	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	GLOBAL_CURSOR->updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

}

Test::Test() :
        AppTester(false),
        mInputManager(InputManager::getInstance())
{
//	setUseDefaultInput(false);
    mPassport.load();
    mPassport.changeAtlasPicture("faces.jpg", NUM_COL, NUM_ROW);
    mPassport.selectPicture(0,0);
    mActualRow = mActualCol = 0;
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
	handleInput();

	mUpdaterManager.updateAllObjects();
}
