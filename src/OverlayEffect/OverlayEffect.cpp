/*
 * OverlayEffect.cpp
 *
 *  Created on: 23/08/2012
 *      Author: agustin
 *
 */

#include "OverlayEffect.h"
#include "OverlayEffectManager.h"

namespace OvEff {

OverlayEffectManager		*OverlayEffect::mMngr = 0;

/**
 * Set the OverlayElement to be used
 * @param	e	The overlay element
 */
void OverlayEffect::setElement(Ogre::OverlayElement *e)
{
	mElement = e;
}

/**
 * Set the Overlay effect manager to be used
 * @param	em	the OverlayEffectManager
 */
void OverlayEffect::setManager(OverlayEffectManager *em)
{
	ASSERT(em);
	mMngr = em;
}

OverlayEffect::OverlayEffect() :
mElement(0),
mActive(false)
{
	ASSERT(mMngr);

}

OverlayEffect::~OverlayEffect()
{
	// TODO Auto-generated destructor stub
}


/**
 * Stops the effect (if is actually reproducing it
 * @note	Removes the effect automatically from the manager
 */
void OverlayEffect::stop(void)
{
	if(!mMngr->hasEffect(this)) return;
	mMngr->remove(this);
	mActive = false;
}

/**
 * Start the effect.
 * @note	Put the effect in the Manager
 */
void OverlayEffect::start(void)
{
	if(mMngr->hasEffect(this)){
		stop();
	}
	mMngr->add(this);
	mActive = true;
}


}
