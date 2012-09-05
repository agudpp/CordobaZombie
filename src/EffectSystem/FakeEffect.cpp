/*
 * FakeEffect.cpp
 *
 *  Created on: 07/08/2012
 *      Author: agustin
 */

#include "FakeEffect.h"

#include <OgreVector3.h>

#include "DebugUtil.h"


FakeEffect::FakeEffect(EffectQueue *q) :
mQueue(q)
{

}
FakeEffect::~FakeEffect()
{

}

void FakeEffect::setQueue(EffectQueue *q)
{
	ASSERT(q);
	mQueue = q;
}

/**
 * Start the effect at some position
 * @param	pos		The position where we want to reproduce the effect
 */
void FakeEffect::start(const Ogre::Vector3 &pos, const Ogre::Vector3 &dir)
{
	ASSERT(mQueue);
	Effect *effect = mQueue->getAvailableObject();
	if(!effect) {
		debugWARNING("Cannot get an effect to be reproduced\n");
		return;
	}
	// else reproduce the effect
	effect->start(pos, dir);
}
