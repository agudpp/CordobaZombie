/*
 * EffectQueue.h
 *
 *  Created on: Oct 2, 2013
 *      Author: agustin
 */

#ifndef EFFECTQUEUE_H_
#define EFFECTQUEUE_H_

#include <debug/DebugUtil.h>
#include <types/StackQueue.h>

namespace cz {

// @brief Represent the queue used for the Effects. Note that the Effect
//        should contain a method named setQueue() to set the parent queue at least.
//


template <typename Effect, unsigned int COUNT>
class EffectQueue
{
public:
    // The constructor will initializate the effects setting this queue instance
    // as the parent queue
    //
    inline EffectQueue();
    inline ~EffectQueue();

    // @brief Let a effect to be used again.
    // @param effect        The effect not used anymore.
    // @note THE EFFECT SHOULD BELONG TO THIS CONTAINER
    //
    inline void
    letAvailable(Effect* effect);

    // @brief Get an available effect to be used.
    // @return the available effect, or 0 if no more effects.
    // @note THIS CLASS IS THE OWNER OF THE MEMORY.
    //
    inline Effect*
    getAvailable(void);

    // @brief Check if we have more effects here
    //
    inline bool
    hasEffects(void) const;

    // @brief Count the number of allocated effects we have.
    //
    inline unsigned int
    numAllocatedEffects(void) const;

    // @brief Return the array of effects of size numAllocatedeffects();
    //
    inline Effect*
    getEffects(void);
    inline const Effect*
    getEffects(void) const;

private:
    Effect mEffects[COUNT];
    core::StackQueue<Effect*, COUNT> mAvailableEffects;
};


////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

template <typename Effect, unsigned int COUNT>
inline EffectQueue<Effect, COUNT>::EffectQueue()
{
    for (unsigned int i = 0; i < COUNT; ++i) {
        mAvailableEffects.push_back(&(mEffects[i]));
        mEffects[i].setQueue(this);
    }

}
template <typename Effect, unsigned int COUNT>
inline EffectQueue<Effect, COUNT>::~EffectQueue()
{

}

template <typename Effect, unsigned int COUNT>
inline void
EffectQueue<Effect, COUNT>::letAvailable(Effect* effect)
{
    ASSERT(!mAvailableEffects.full());
    ASSERT(effect);
    debugERROR("TODO check here that effect is one of the meffect positions and"
        " is not in mAvailableeffects already\n");
    mAvailableEffects.push_back(effect);
}

template <typename Effect, unsigned int COUNT>
inline Effect*
EffectQueue<Effect, COUNT>::getAvailable(void)
{
    if (mAvailableEffects.empty()) {
        return 0;
    }
    Effect* b = mAvailableEffects.back();
    mAvailableEffects.pop_back();
    return b;
}

template <typename Effect, unsigned int COUNT>
inline bool
EffectQueue<Effect, COUNT>::hasEffects(void) const
{
    return !mAvailableEffects.empty();
}

template <typename Effect, unsigned int COUNT>
inline unsigned int
EffectQueue<Effect, COUNT>::numAllocatedEffects(void) const
{
    return COUNT;
}

template <typename Effect, unsigned int COUNT>
inline Effect*
EffectQueue<Effect, COUNT>::getEffects(void)
{
    return mEffects;
}
template <typename Effect, unsigned int COUNT>
inline const Effect*
EffectQueue<Effect, COUNT>::getEffects(void) const
{
    return mEffects;
}

} /* namespace effect */
#endif /* EFFECTQUEUE_H_ */
