/*
 * MicroAppRunner.cpp
 *
 *  Created on: 10/11/2011
 *      Author: agustin
 */

#include <OgreTimer.h>
#include <OgreWindowEventUtilities.h>
#include <OgreOverlay.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>

#include "MicroAppRunner.h"
#include "MicroApp.h"

MicroApp *MicroAppRunner::microApp = 0;


void MicroAppRunner::showFade(bool begin)
{

	// load the fade
	Ogre::OverlayContainer *mFader = 0;
	Ogre::MaterialPtr		mFaderMaterial;
	Ogre::TextureUnitState 	*mTexture = 0;

	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

	mFader = static_cast<Ogre::OverlayContainer*>(
		overlayManager.createOverlayElement("Panel", "Fader"));
	mFader->setMetricsMode(Ogre::GMM_RELATIVE);
	mFader->setPosition(0, 0);
	mFader->setDimensions(1, 1);
	mFader->setMaterialName("FaderMaterial"); // Optional background material

	// Ensures that the material exists
	mFaderMaterial = Ogre::MaterialManager::getSingleton().getByName(
			"FaderMaterial",
			Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
	ASSERT(!mFaderMaterial.isNull());
	mTexture = mFaderMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	ASSERT(mTexture);
	mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
					Ogre::LBS_TEXTURE, 0);

	// show the fade
	mFader->show();

	// Create an overlay, and add the panel
	Ogre::Overlay			*mOverlay = 0;
	mOverlay = overlayManager.create("FaderOverlay");
	mOverlay->add2D(mFader);
	mOverlay->show();

	// start the main loop
	Ogre::Timer timer;
	float timeStamp = 0;
	Ogre::Real frameTime = 0;
	Ogre::Real fadeTime = 0.0f;
	if(!begin){
		fadeTime = MICROAR_FADER_TIME;
	}
	Ogre::Real mFaderStep = 0;


	// here is the main loop
	while(fadeTime <= MICROAR_FADER_TIME && fadeTime >= 0.0f) {
		timeStamp = timer.getMilliseconds();
		if(begin){
			fadeTime += frameTime;
			mFaderStep = fadeTime / MICROAR_FADER_TIME;
		} else {
			fadeTime -= frameTime;
			mFaderStep = fadeTime / MICROAR_FADER_TIME;
		}


		mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
				Ogre::LBS_TEXTURE, mFaderStep);

		// render the frame
		if(!GLOBAL_ROOT->renderOneFrame())
			break;

		// This must be called when we use the renderOneFrame approach
		Ogre::WindowEventUtilities::messagePump();

		frameTime = (timer.getMilliseconds() - timeStamp) * 0.001;
	}

	// remove fader
	if(mOverlay){
		Ogre::OverlayManager::getSingleton().destroy(mOverlay);
	}

	if(mFader){
		Ogre::MaterialManager::getSingleton().unload("FaderMaterial");
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(mFader);
		mFader = 0;
	}

}




void MicroAppRunner::runMicroApp(void)
{
	if(!microApp){
		debugERROR("Error: app is null\n");
		ASSERT(app);
		return;
	}


	// show the fader
	showFade(true);

	// start the main loop
	Ogre::Timer timer;
	float timeStamp = 0;
	Ogre::Real frameTime = 0;

	// load the app
	microApp->load();

	// here is the main loop
	while(true) {
		timeStamp = timer.getMilliseconds();


		// handle local input		capture input
		GLOBAL_KEYBOARD->capture();
		GLOBAL_MOUSE->capture();

		// update the app
		if(microApp->update(frameTime) != MicroApp::CONTINUE){
			break;
		}

		// render the frame
		if(!GLOBAL_ROOT->renderOneFrame())
			break;

		// This must be called when we use the renderOneFrame approach
		Ogre::WindowEventUtilities::messagePump();

		frameTime = (timer.getMilliseconds() - timeStamp) * 0.001;
	}

	// unload the app
	microApp->unload();

	// remove the microapp
	microApp = 0;

	showFade(false);

	return;
}
