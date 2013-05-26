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
#include <OgreSubEntity.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreTextureUnitState.h>

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
	Ogre::Overlay *ov = om.getByName("TestOverlayAtlas");
	ASSERT(ov);
//	ov->show();
	Ogre::PanelOverlayElement *contSingle = static_cast<Ogre::PanelOverlayElement *>(
			om.getOverlayElement("TestOverlayAtlas/single"));
	ASSERT(contSingle);

	Ogre::PanelOverlayElement *contMulti = static_cast<Ogre::PanelOverlayElement *>(
			om.getOverlayElement("TestOverlayAtlas/multi"));
	ASSERT(contMulti);

	mSingleA.setPanel(contSingle);
	mSingleA.setNumberImages(3);

	mMultiA.setPanel(contMulti);
	mMultiA.setNumberImages(2,3);

}

// handle input
void Test::handleInput(void)
{
	static bool pressed = false;
	static bool pressed2 = false;
	static float counter = 0.0f;

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
		if(!pressed){
			pressed = true;
			static int counter = 0;
			counter = (counter+1)%3;
			mSingleA.changeAtlas(counter);
		}
	} else {
		pressed = false;
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
		if(!pressed2){
			pressed2 = true;
			static int counter = 0;
			int row,column;
			column = counter % 3;
			row = (counter / 2) % 2;
			counter = (counter+1)%6;
			debugBLUE("Changing row: %d, column:%d\n", row, column);
			mMultiA.changeAtlas(row,column);
		}
	} else {
		pressed2 = false;
	}

	static bool press3 = false;
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_1)){
		if(!press3){
			press3 = true;
			Ogre::TextureUnitState *tus = mEntity->getSubEntity(0)->getMaterial()->getTechnique(0)->
								getPass(0)->getTextureUnitState(0);

			tus->setColourOperationEx(
							Ogre::LBX_SOURCE1,
							Ogre::LBS_MANUAL,
							Ogre::LBS_CURRENT,
							Ogre::ColourValue(1.0, 0.0, 1.0,1.0));

		}
	} else {
		press3 = false;
	}

}


/* Load additional info */
void Test::loadAditionalData(void)
{
	loadInfostate();
	mEntity = GLOBAL_SCN_MNGR->createEntity("zombie01.mesh");
	GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode()->attachObject(mEntity);
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	handleInput();

}
