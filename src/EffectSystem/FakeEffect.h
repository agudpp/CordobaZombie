/*
 * FakeEffect.h
 *
 *  Created on: 07/08/2012
 *      Author: agustin
 */

#ifndef FAKEEFFECT_H_
#define FAKEEFFECT_H_


#include "Effect.h"

/**
 * Note that we inherit from QueueContainerObject so we automatically have the
 * "setContainer" static function.
 */

class FakeEffect
{
public:
	FakeEffect(EffectQueue *q = 0);
	~FakeEffect();

	/**
	 * Set the queue associated to this fake effect
	 */
	void setQueue(EffectQueue *q);

	/**
	 * Start the effect at some position
	 * @param	pos		The position where we want to reproduce the effect
	 * @param	dir		The direction (if needed) of the effect
	 */
	void start(const Ogre::Vector3 &pos,
			const Ogre::Vector3 &dir = Ogre::Vector3::ZERO);

private:
	EffectQueue		*mQueue;
};

#endif /* FAKEEFFECT_H_ */
