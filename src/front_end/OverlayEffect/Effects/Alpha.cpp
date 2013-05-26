/*
 * Alpha.cpp
 *
 *  Created on: 28/08/2012
 *      Author: agustin
 *
 */


#include "GlobalObjects.h"
#include "DebugUtil.h"
#include "Alpha.h"

namespace OvEff {


////////////////////////////////////////////////////////////////////////////////
void
Alpha::configure(Ogre::OverlayElement *e)
{
	ASSERT(e);
	mElement = e;
	// Get the texture from the OverlayElement
	Ogre::MaterialPtr mat = e->getMaterial();
	ASSERT(!mat.isNull());
	mTexture = mat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	ASSERT(mTexture);
}


////////////////////////////////////////////////////////////////////////////////
void
Alpha::configure(Ogre::OverlayElement *e, Type t, float time)
{
	mType = t;
	mTime = time;
	configure(e);
}


////////////////////////////////////////////////////////////////////////////////
void Alpha::setElement(Ogre::OverlayElement *e) { configure(e); }


////////////////////////////////////////////////////////////////////////////////
void
Alpha::enter(void)
{
	switch(mType)
	{
	case FADE_OUT:
		mAccumTime = mTime;
		break;

	case FADE_IN:
		mAccumTime = 0;
		break;

	default:
		debugERROR("Invalid Alpha transition type.%s", "\n");
		ASSERT(false);
		break;
	}
}


////////////////////////////////////////////////////////////////////////////////
void
Alpha::update()
{
	switch(mType)
	{
	case FADE_OUT:
		// Slowly dissappear: reduce alpha value
		mAccumTime -= GLOBAL_TIME_FRAME;
		mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
						Ogre::LBS_TEXTURE, mAccumTime / mTime);
		if(mAccumTime <= 0){
			// Effect finished
			stop();
		}
		break;

	case FADE_IN:
		// Slowly materialize: increase alpha value
		mAccumTime += GLOBAL_TIME_FRAME;
		mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
						Ogre::LBS_TEXTURE, mAccumTime / mTime);
		if(mAccumTime >= mTime){
			// Effect finished
			stop();
		}
		break;

	default:
		debugERROR("Invalid Alpha transition type.%s", "\n");
		ASSERT(false);
		break;
	}
}


////////////////////////////////////////////////////////////////////////////////
void Alpha::exit(void) { /* Nothing to do. */ }



}

