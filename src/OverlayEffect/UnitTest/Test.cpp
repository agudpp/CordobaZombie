/*
 * Test.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include "Test.h"

#include <OgreSceneNode.h>
#include <iostream>
#include <assert.h>
#include <OgreSceneQuery.h>
#include <OgreRay.h>
#include <OgreManualObject.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreString.h>

// create overlay uv test
void Test::createOverlay(void)
{
	Ogre::Overlay *mOverlay = 0;
	// load the fade
	Ogre::MaterialPtr		mFaderMaterial;
	Ogre::TextureUnitState*	mTexture(0);
	Ogre::OverlayManager&	overlayManager(Ogre::OverlayManager::getSingleton());

	mOverlayPanel = static_cast<Ogre::PanelOverlayElement*>(
		overlayManager.createOverlayElement("Panel", "Fader"));
	mOverlayPanel->setMetricsMode(Ogre::GMM_RELATIVE);
	mOverlayPanel->setPosition(0.05f, 0.1f);
	mOverlayPanel->setDimensions(0.15f, 0.25f);
	mOverlayPanel->setMaterialName("BackpackMaterial"); // Optional background material

	// Make sure the panel exists
	mOverlayPanel->setUV(0.0f, 0.0f, 1.0f, 1.0f);

	// Make the panel visible
	mOverlayPanel->show();

	// Create an overlay, and attach the panel to it
	mOverlay = overlayManager.create("TestOverlay1");
	mOverlay->add2D(mOverlayPanel);
	mOverlay->show();
}



Test::Test()
{
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight());
	OvEff::OverlayEffect::setManager(&mOvEffMngr);

	printf("\n\n\33[01;34mOverlay fading controls:\n\33[22;32m"
				" ¤\33[01;34m 1\33[22;32m :  fade in.\n"
				" ¤\33[01;34m 2\33[22;32m :  fade out.\n"
				"\33[0m\n");
}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}


void
Test::handleInput()
{
	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	// KEYBOARD
	static bool k1p = false;
	static bool k2p = false;

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_1)){
		if(!k1p){
			k1p = true;
			// switch and start the effect
			mAlphaEffect->setType(OvEff::Alpha::FADE_IN);
			mAlphaEffect->start();
		}
	} else {
		k1p = false;
	}

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_2)){
		if(!k2p){
			k2p = true;
			// switch and start the effect
			mAlphaEffect->setType(OvEff::Alpha::FADE_OUT);
			mAlphaEffect->start();
		}
	} else {
		k2p = false;
	}
}


/* Load additional info */
void Test::loadAditionalData(void)
{
	createOverlay();
	mAlphaEffect = new OvEff::Alpha;
	mAlphaEffect->setTime(2.0f);
	mAlphaEffect->configure(mOverlayPanel);
}


/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	handleInput();
	mOvEffMngr.update();
}
