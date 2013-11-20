/*
 * EffectQueueDefs.h
 *
 *  Created on: Oct 2, 2013
 *      Author: agustin
 */

#ifndef EFFECTQUEUEDEFS_H_
#define EFFECTQUEUEDEFS_H_

#include "EffectQueue.h"

namespace cz {

// forwards
//
class BloodParticles;
class BodyPartEffect;
class BulletImpactEffect;

// defines the different queues here
//
typedef EffectQueue<BloodParticles, 3> BloodParticlesQueue;
typedef EffectQueue<BodyPartEffect, 25> BodyPartsEffectQueue;
typedef EffectQueue<BulletImpactEffect, 3> BulletImpactEffectQueue;


}

#endif /* EFFECTQUEUEDEFS_H_ */
