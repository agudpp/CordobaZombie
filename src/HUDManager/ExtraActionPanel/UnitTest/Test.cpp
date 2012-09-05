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

#include "GlobalObjects.h"
#include "Test.h"


struct TestCallback : public ExtraAction::Callback {
	virtual void operator()(int buttonPressed)
	{
		debugColor(DEBUG_BROWN, "Apretado boton :%d\n", buttonPressed);
	}
};

static double gettimestamp(void)
{
	struct timeval now;

	if(gettimeofday(&now, 0) < 0)
		return 0.0;

	double nowDouble = static_cast<double>(now.tv_sec + (
						static_cast<double>(now.tv_usec)/1000000.0));

	return nowDouble;
}








Test::Test()
{
	mMouseCursor.setImage("cursor.png");
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight());

	// build menu manager
	mMenuManager.build(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight(),
				5,5);
	IMenu::setMenuManager(&mMenuManager);

}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}

// handle input
void Test::handleInput(void)
{
	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);
}


/* Load additional info */
void Test::loadAditionalData(void)
{
	mPanel.load();
	ExtraAction::Callback *cb = new TestCallback;
	ASSERT(mPanel.getNumLockers() == 2);
	ASSERT(mPanel.getNumActualActions() == 0);
	mPanel.addAction(cb, "HUD/ExtraAction/bombs");
	ASSERT(mPanel.getNumActualActions() == 1);
	mPanel.addAction(cb, "HUD/ExtraAction/bombs");
	ASSERT(mPanel.getNumActualActions() == 2);
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	handleInput();
//	mTestUnit.update();
	static bool keyPres = false, keyPres2 = false;

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
		if(!keyPres){
			keyPres = true;
			mPanel.removeAction(0);
		}
	} else {
		keyPres = false;
	}


	mMenuManager.update();
}
