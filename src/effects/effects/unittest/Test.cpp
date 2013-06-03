/*
 * Blood.h
 *
 *  Created on: Apr 1, 2012
 *      Author: Emi
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





// handle input
void Test::handleInput(void)
{






}

Test::Test()
{
	setUseDefaultInput(false);
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight());


	//We put the cursor in the middle of the screen
	OIS::MouseState &mMouseState = const_cast<OIS::MouseState &>(mMouse->getMouseState());
	mMouseState.X.abs = mCamera->getViewport()->getActualWidth() / 2;
	mMouseState.Y.abs = mCamera->getViewport()->getActualHeight() / 2;

	mMouseCursor.updatePosition(mMouseState.X.abs,mMouseState.Y.abs);



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
	effectMgr.update();
}
