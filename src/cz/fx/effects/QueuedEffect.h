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
    QueuedEffect() {};
    virtual
    ~QueuedEffect() {};

    // @brief Set / Get the associated effect queue for this class
    //
    inline void
    setQueue(QueueType* effectQueue) { sEffectQueue = effectQueue; }
    inline QueueType*
    queue(void) const { return sEffectQueue; }

    // @brief Let this effect available again
    //
    inline void
    letThisFree(void)
    {
        ASSERT(sEffectQueue);
        sEffectQueue->letAvailable(static_cast<EffectType*>(this));
    }

protected:
    static QueueType* sEffectQueue;
};

template<typename QueueType, typename EffectType>
QueueType* QueuedEffect<QueueType, EffectType>::sEffectQueue = 0;

} /* namespace effect */
#endif /* QUEUEDEFFECT_H_ */
