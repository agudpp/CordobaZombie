/*
 * BulletImpactQueueHandler.cpp
 *
 *  Created on: Nov 20, 2013
 *      Author: agustin
 */

#include <map>
#include <array>

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
    // FIXME: HARDCODED SOUND NAMES, SHOULD COME FROM XML /////////////////////
	const uint MAX_LIST_SIZE(3u);
    std::array <const Ogre::String, MAX_LIST_SIZE> soundsDefault =
    	{"fxA9.ogg", "fxA10.ogg", "fxA11.ogg"};
    std::array <const Ogre::String, MAX_LIST_SIZE> soundsMetal =
    	{"fxA12.ogg","",""};
    std::array <const Ogre::String, MAX_LIST_SIZE> soundsWood =
    	{"fxA13.ogg", "fxA14.ogg",""};
    std::array <const Ogre::String, MAX_LIST_SIZE> soundsRock =
    	{"fxA15.ogg","",""};
    std::map <int, std::array<const Ogre::String, MAX_LIST_SIZE>>
    	soundsLists = {{Type::BIQT_DEFAULT, soundsDefault},
					   {Type::BIQT_METAL,   soundsMetal},
					   {Type::BIQT_WOOD,    soundsWood},
					   {Type::BIQT_ROCK,    soundsRock}};
    ///////////////////////////////////////////////////////////////////////////
    bool allOk = true;

    for (unsigned int i = 0; i < Type::COUNT && allOk; ++i) {
        BulletImpactEffectQueue& q = mQueues[i];  // queue for this type
        BulletImpactEffect* effects = q.getEffects();
        const unsigned int numElements = q.numAllocatedEffects();
        const Ogre::String particleName = QUEUE_PARTICLE_NAMES[i];
        // Construct sounds for the kind of particles in this queue
        mSounds.addSounds(i, soundsLists[i].data(), MAX_LIST_SIZE);
        // Construct effects for all particles in this queue
        for (unsigned int j = 0; j < numElements && allOk; ++j) {
        	const Ogre::String* soundNamep(mSounds.getRandomSound(i));
        	ASSERT(soundNamep);
            allOk = allOk && effects[j].construct(particleName, soundNamep);
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
