/*
 * BodyPartEffect.h
 *
 *  Created on: Nov 4, 2013
 *      Author: agustin
 */

#ifndef BODYPARTEFFECT_H_
#define BODYPARTEFFECT_H_

#include <OgreVector3.h>

#include <effect_handler/Effect.h>
#include <debug/DebugUtil.h>
#include <zombie_unit/BodyPartElement.h>
#include <zombie_unit/BodyPartQueue.h>
#include <fx/effects/EffectQueueDefs.h>
#include <fx/effects/QueuedEffect.h>


// Forward
//
namespace physics {
class DynamicWorld;
}

namespace cz {

// Forward
//
class BodyPartEffect;

class BodyPartEffect : public QueuedEffect<BodyPartsEffectQueue, BodyPartEffect>
{
    // we will also define the minimum time we want to be in scene just in case
    // that the body part starts quiet.
    static const float MIN_TIME_IN_SCENE_SECS;
public:
    BodyPartEffect();
    virtual
    ~BodyPartEffect();

    // @brief Set the DynamicWorld to be used and shared by all the BodyPart
    //        effects
    // @param dw        The dynamic world instance
    //
    static inline void
    setDynamicWorld(physics::DynamicWorld* dw);

    // @brief Set the element and queue to be used
    // @param element       The body part element
    // @param elementQueue  The queue associated to the element
    //
    inline void
    setElement(BodyPartElement* element);
    inline void
    setElementQueue(BodyPartQueue* elementQueue);
    inline BodyPartElement*
    element(void);
    inline const BodyPartElement*
    element(void) const;
    inline BodyPartQueue*
    elementQueue(void);
    inline const BodyPartQueue*
    elementQueue(void) const;

    // @brief Configure the force that we have to put to the element using the
    //        impact point and the bullet or hit direction.
    //        To call this method you should already set the Element
    // @param impactPoint       The position of the impact
    // @param direction         The bullet / hit direction
    // @param impactForce       The force of the impact
    //
    void
    configureForce(const Ogre::Vector3& impactPosition,
                   const Ogre::Vector3& direction,
                   float impactForce);


    ////////////////////////////////////////////////////////////////////////////
    // Inherited methods
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
    BodyPartElement* mElement;
    BodyPartQueue* mElementQueue;
    float mAccumTime;

    static physics::DynamicWorld* sDynamicWorld;
};






////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
BodyPartEffect::setDynamicWorld(physics::DynamicWorld* dw)
{
    ASSERT(dw);
    sDynamicWorld = dw;
}

inline void
BodyPartEffect::setElement(BodyPartElement* element)
{
    ASSERT(element);
    mElement = element;
}
inline void
BodyPartEffect::setElementQueue(BodyPartQueue* elementQueue)
{
    ASSERT(elementQueue);
    mElementQueue = elementQueue;
}
inline BodyPartElement*
BodyPartEffect::element(void)
{
    return mElement;
}
inline const BodyPartElement*
BodyPartEffect::element(void) const
{
    return mElement;
}
inline BodyPartQueue*
BodyPartEffect::elementQueue(void)
{
    return mElementQueue;
}
inline const BodyPartQueue*
BodyPartEffect::elementQueue(void) const
{
    return mElementQueue;
}


} /* namespace cz */
#endif /* BODYPARTEFFECT_H_ */
