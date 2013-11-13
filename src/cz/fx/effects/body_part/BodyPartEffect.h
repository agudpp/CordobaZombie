/*
 * BodyPartEffect.h
 *
 *  Created on: Nov 4, 2013
 *      Author: agustin
 */

#ifndef BODYPARTEFFECT_H_
#define BODYPARTEFFECT_H_

#include <effect_handler/Effect.h>
#include <debug/DebugUtil.h>
#include <zombie_unit/BodyPartElement.h>
#include <zombie_unit/BodyPartQueue.h>


// Forward
//
namespace physics {
class DynamicWorld;

}

namespace cz {

class BodyPartEffect : public effect::Effect
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
    // @param queue         The queue associated to the element
    //
    inline void
    setElement(BodyPartElement* element);
    inline void
    setQueue(BodyPartQueue* queue);
    inline BodyPartElement*
    element(void);
    inline const BodyPartElement*
    element(void) const;
    inline BodyPartQueue*
    queue(void);
    inline const BodyPartQueue*
    queue(void) const;


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
    BodyPartQueue* mQueue;
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
BodyPartEffect::setQueue(BodyPartQueue* queue)
{
    ASSERT(queue);
    mQueue = queue;
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
BodyPartEffect::queue(void)
{
    return mQueue;
}
inline const BodyPartQueue*
BodyPartEffect::queue(void) const
{
    return mQueue;
}


} /* namespace cz */
#endif /* BODYPARTEFFECT_H_ */
