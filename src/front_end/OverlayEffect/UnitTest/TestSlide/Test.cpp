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
	// load the fade
	mOverlayPanel = 0;
	Ogre::MaterialPtr		mFaderMaterial;
	Ogre::TextureUnitState 	*mTexture = 0;

	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

	mOverlayPanel = static_cast<Ogre::PanelOverlayElement*>(
		overlayManager.createOverlayElement("Panel", "Fader"));
	mOverlayPanel->setMetricsMode(Ogre::GMM_RELATIVE);
	mOverlayPanel->setPosition(0, 0);
	mOverlayPanel->setDimensions(0.5f, 0.5f);
	mOverlayPanel->setMaterialName("BackpackMaterial"); // Optional background material

	// Ensures that the material exists
	mOverlayPanel->setUV(0,0,0.5,1);

	// show the fade
	mOverlayPanel->show();

	// Create an overlay, and add the panel
	Ogre::Overlay			*mOverlay = 0;
	mOverlay = overlayManager.create("TestOverlay1");
	mOverlay->add2D(mOverlayPanel);
	mOverlay->show();
}



Test::Test()
{
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight());
	f_e::OverlayEffect::setManager(&mOvEffMngr);
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
	createOverlay();
	mSlideEffect = new f_e::Slide;
	mSlideEffect->setDuration(1.5f);
	mSlideEffect->setTranslationPositions(Ogre::Vector2(0.90f,0.0f),
			Ogre::Vector2(-0.1f,0.0f));
	mSlideEffect->setFunction(&mvfun);
	mSlideEffect->setElement(mOverlayPanel);
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	static bool k1p = false;

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_G)){
		if(!k1p){
			k1p = true;
			// switch and start the effect
			mSlideEffect->stop();
			mSlideEffect->start();
		}
	} else {
		k1p = false;
	}

	handleInput();
	mOvEffMngr.update();
}
