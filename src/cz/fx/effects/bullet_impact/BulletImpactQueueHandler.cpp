/*
 * BulletImpactQueueHandler.cpp
 *
 *  Created on: Nov 20, 2013
 *      Author: agustin
 */

#include "BulletImpactQueueHandler.h"

namespace cz {

const char* BulletImpactQueueHandler::QUEUE_PARTICLE_NAMES[Type::COUNT] = {
    "FX/BulletImpact/Default",
    "FX/BulletImpact/Metal",
    "FX/BulletImpact/Wood",
    "FX/BulletImpact/Rock",
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
BulletImpactQueueHandler::BulletImpactQueueHandler()
{

}

////////////////////////////////////////////////////////////////////////////////
BulletImpactQueueHandler::~BulletImpactQueueHandler()
{
    destroyQueues();
}

////////////////////////////////////////////////////////////////////////////////
bool
BulletImpactQueueHandler::buildQueues(void)
{
    bool allOk = true;
    for (unsigned int i = 0; i < Type::COUNT && allOk; ++i) {
        BulletImpactEffectQueue& q = mQueues[i];
        BulletImpactEffect* effects = q.getEffects();
        const unsigned int numElements = q.numAllocatedEffects();
        const Ogre::String particleName = QUEUE_PARTICLE_NAMES[i];
        for (unsigned int j = 0; j < numElements && allOk; ++j) {
            // TODO: here we have to put the sound stuff
            allOk = allOk && effects[j].construct(particleName);
            if (!allOk) {
                debugERROR("Error building the particle system %s\n",
                           particleName.c_str());
            }
        }
    }

    return allOk;
}

////////////////////////////////////////////////////////////////////////////////
void
BulletImpactQueueHandler::destroyQueues(void)
{
    for (unsigned int i = 0; i < Type::COUNT; ++i) {
        BulletImpactEffectQueue& q = mQueues[i];
        BulletImpactEffect* effects = q.getEffects();
        const unsigned int numElements = q.numAllocatedEffects();
        for (unsigned int j = 0; j < numElements; ++j) {
            effects[j].destroy();
        }
    }
}


} /* namespace cz */
