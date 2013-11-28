/*
 * BloodParticles.h
 *
 *  Created on: Oct 2, 2013
 *      Author: agustin
 */

#ifndef BLOODPARTICLES_H_
#define BLOODPARTICLES_H_


#include <OgreSceneNode.h>
#include <OgreParticleSystem.h>

#include <fx/effects/EffectQueueDefs.h>
#include <fx/effects/QueuedEffect.h>

// We will define the blood particle system names to be used
//
#define BLOOD_PART_NAME "FX/BloodParticles"

namespace cz {

// Forward
//
class BloodParticles;

class BloodParticles : public QueuedEffect<BloodParticlesQueue, BloodParticles>
{
    static const float ACTIVE_TIME_SECS; // in seconds
public:
    BloodParticles();
    virtual
    ~BloodParticles();

    // @brief Construct the Particle system and everything and configure it.
    //        We will use the GlobalData information to do this.
    //
    void
    construct(void);

    // @brief Configure the effect to be ready to be reproduced in a certain
    //        position and direction and intensity
    // @param position      The position
    // @param direction     The direction
    // @param intensity     The intensity
    //
    void
    configure(const Ogre::Vector3& position,
              const Ogre::Vector3& direction,
              float intensity);

    // @brief Function called before the effect will be reproduced
    //
    virtual void
    beforeStart(void);

    // @brief Function called each frame to update the effect.
    // @param timeFrame     The time of the frame
    // @returns true if the effect is still active and should be reproduced
    //          next frame, or false if the effect finished.
    //
    virtual bool
    update(float timeFrame);

    // @brief Function called after the effect stops reproducing it.
    //
    virtual void
    afterFinish(void);

protected:
    Ogre::SceneNode* mNode;
    Ogre::ParticleSystem* mParticleSystem;
    float mCurrentTime;
};

} /* namespace effect */
#endif /* BLOODPARTICLES_H_ */
