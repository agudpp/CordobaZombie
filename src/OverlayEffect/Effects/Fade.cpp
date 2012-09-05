/*
 * Fade.cpp
 *
 *  Created on: 23/08/2012
 *      Author: agustin
 *
 */

#include "Fade.h"

#include <OgreOverlayManager.h>
#include <OgreStringConverter.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgrePass.h>

#include "GUIHelper.h"
#include "GlobalObjects.h"

namespace OvEff {

Ogre::Overlay	*Fade::mOverlay = 0;
unsigned int	Fade::mICounter = 0;

////////////////////////////////////////////////////////////////////////////////
Fade::Fade() :
mTime(1.0f),
mType(FADE_IN),
mTexture(0),
mAccumTime(0)
{
	if(!mOverlay){
		// create the shared overlay
		mOverlay = Ogre::OverlayManager::getSingleton().create("sFadeOv");
		mOverlay->show();
	}

	// create the element
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElement(
			"Panel", "FdE" + Ogre::StringConverter::toString(mICounter));

	// Set the material
	mElement->setMaterialName("FaderMaterial");
	Ogre::MaterialPtr mat = mElement->getMaterial();
	ASSERT(!mat.isNull());
	mTexture = mat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
						Ogre::LBS_TEXTURE, 0);
	ASSERT(mTexture);

	mElement->show();

	++mICounter;
}

////////////////////////////////////////////////////////////////////////////////
Fade::~Fade()
{
	--mICounter;
	Ogre::OverlayManager::getSingleton().destroyOverlayElement(mElement);
	if(mICounter <= 0){
		GUIHelper::fullDestroyOverlay(mOverlay);
		mOverlay = 0;
	}
}


////////////////////////////////////////////////////////////////////////////////
void Fade::enter(void)
{
	switch(mType){
	case FADE_OUT: 	mAccumTime = 0;		break;
	case FADE_IN:	mAccumTime = mTime;	break;
	default: ASSERT(false);
	}
	mOverlay->add2D(static_cast<Ogre::OverlayContainer *>(mElement));
}

////////////////////////////////////////////////////////////////////////////////
void Fade::update(void)
{
	// check which functionality are executing
	switch(mType){
	case FADE_OUT:
	{
		mAccumTime += GLOBAL_TIME_FRAME;
		const Ogre::Real faderStep = mAccumTime / mTime;
		mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
						Ogre::LBS_TEXTURE, faderStep);
		if(mAccumTime >= mTime){
			// we have to stop the effect
			stop();
		}
	}
		break;

	case FADE_IN:
	{
		mAccumTime -= GLOBAL_TIME_FRAME;
		const Ogre::Real faderStep = mAccumTime / mTime;
		mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
						Ogre::LBS_TEXTURE, faderStep);
		if(mAccumTime <= 0){
			// we have to stop the effect
			stop();
		}
	}
		break;
	default:
		ASSERT(false);
	}
}

////////////////////////////////////////////////////////////////////////////////
void Fade::exit(void)
{
	mOverlay->remove2D(static_cast<Ogre::OverlayContainer *>(mElement));
}

}
