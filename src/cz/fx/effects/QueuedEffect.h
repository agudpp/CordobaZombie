/*
 * QueuedEffect.h
 *
 *  Created on: Oct 2, 2013
 *      Author: agustin
 */

#ifndef QUEUEDEFFECT_H_
#define QUEUEDEFFECT_H_

#include <debug/DebugUtil.h>
#include <effect_handler/Effect.h>

namespace cz {

template<typename QueueType, typename EffectType>
class QueuedEffect : public effect::Effect
{
public:
    QueuedEffect() : mEffectQueue(0) {};
    virtual
    ~QueuedEffect() {};

    // @brief Set / Get the associated effect queue for this class
    //
    inline void
    setQueue(QueueType* effectQueue) { mEffectQueue = effectQueue; }
    inline QueueType*
    queue(void) const { return mEffectQueue; }

    // @brief Let this effect available again
    //
    inline void
    letThisFree(void)
    {
        ASSERT(mEffectQueue);
        mEffectQueue->letAvailable(static_cast<EffectType*>(this));
    }


    ////////////////////////////////////////////////////////////////////////////
    // Effect interface
    //
    // @brief Function called before the effect will be reproduced
    //
    virtual void
    beforeStart(void) = 0;

    // @brief Function called each frame to update the effect.
    // @param timeFrame     The time of the frame
    // @returns true if the effect is still active and should be reproduced
    //          next frame, or false if the effect finished.
    //
    virtual bool
    update(float timeFrame) = 0;

    // @brief Function called after the effect stops reproducing it.
    //
    virtual void
    afterFinish(void) = 0;

protected:
    QueueType* mEffectQueue;
};

} /* namespace effect */
#endif /* QUEUEDEFFECT_H_ */
