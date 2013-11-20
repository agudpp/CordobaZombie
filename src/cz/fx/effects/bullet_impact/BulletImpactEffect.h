/*
 * BulletImpactEffect.h
 *
 *  Created on: Nov 20, 2013
 *      Author: agustin
 */

#ifndef BULLETIMPACTEFFECT_H_
#define BULLETIMPACTEFFECT_H_

#include <OgreVector3.h>
#include <OgreSceneNode.h>
#include <OgreParticleSystem.h>

#include <debug/DebugUtil.h>
#include <fx/effects/EffectQueueDefs.h>
#include <fx/effects/QueuedEffect.h>

namespace cz {

// forward
//
class BulletImpactEffect;

class BulletImpactEffect : public QueuedEffect<BulletImpactEffectQueue, BulletImpactEffect>
{
    static const float ACTIVE_TIME_SECS; // in seconds
public:
    BulletImpactEffect();
    virtual
    ~BulletImpactEffect();


    // @brief Configure the effect with the name of the particle system to be
    //        used and the sound to be used
    // @param particleName      The particle system name to be loaded
    // @param sound             The sound to be used
    // @return true on success | false otherwise
    // TODO: complete with the sound stuff here, issue #256
    //
    bool
    construct(const Ogre::String& particleName);

    // @brief Destroy all the data used by this instance
    //
    void
    destroy(void);

    // @brief Configure the effect before reproducing it. This method
    //        will put the effect in the correct position / direction / and
    //        any other thing we need.
    // @param position      The position where the bullect impact (world pos)
    // @param surfaceNormal The normal of the surface where the bullet impact
    // @param bulletDir     The direction of the bullet
    // @param power         The power of the bullet (mass and velocity for example)
    //
    void
    setParams(const Ogre::Vector3& position,
              const Ogre::Vector3& surfaceNormal,
              const Ogre::Vector3& bulletDir,
              float power);

    ////////////////////////////////////////////////////////////////////////////
    // Effect interface
    //
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


private:
    Ogre::SceneNode* mNode;
    Ogre::ParticleSystem* mParticleSystem;
    float mCurrentTime;
};

} /* namespace cz */
#endif /* BULLETIMPACTEFFECT_H_ */
