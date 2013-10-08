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

// defines the different queues here
//
typedef EffectQueue<BloodParticles, 3> BloodParticlesQueue;


}

#endif /* EFFECTQUEUEDEFS_H_ */
