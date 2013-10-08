/*
 * EffectHandler.h
 *
 *  Created on: Oct 2, 2013
 *      Author: agustin
 */

#ifndef EFFECTHANDLER_H_
#define EFFECTHANDLER_H_

#include <debug/DebugUtil.h>
#include <types/StackVector.h>

#include "Effect.h"

namespace effect {

class EffectHandler
{
    // define the number of effects we can handle at the same time
    //
    static const unsigned int MAX_EFFECTS = 16;
public:
    EffectHandler();
    ~EffectHandler();

    // @brief Add an effect to be reproduced in each frame.
    // @param effect        The effect we want to reproduce.
    //
    inline void
    add(Effect* effect);

    // @brief Remove an existent effect to stops its reproduction.
    // @param effect        The effect we want to stop.
    //
    inline void
    remove(Effect* effect);

    // @brief Check if an element already exists in the handler or not.
    // @param effect        The effect we want to check if it is already being
    //                      reproduced.
    //
    inline bool
    contains(const Effect* effect) const;

    // @brief The update method, this should be called each frame to update all
    //        the active and current effects.
    // @param timeFrame     The time of the frame
    //
    inline void
    update(float timeFrame);

private:
    core::StackVector<Effect*, MAX_EFFECTS> mEffects;
};


////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//


inline void
EffectHandler::add(Effect* effect)
{
    ASSERT(effect);
    if (contains(effect)) {
        return;
    }
    effect->beforeStart();
    mEffects.push_back(effect);
}

inline void
EffectHandler::remove(Effect* effect)
{
    ASSERT(effect);
    if (!contains(effect)) {
        return;
    }
    effect->afterFinish();
    mEffects.back()->id = effect->id;
    mEffects[effect->id] = mEffects.back();
    mEffects.pop_back();
}

inline bool
EffectHandler::contains(const Effect* effect) const
{
    ASSERT(effect);
    return (effect->id < mEffects.size()) && (mEffects[effect->id] == effect);
}

inline void
EffectHandler::update(float timeFrame)
{
    for (Effect** beg = mEffects.begin(), **end = mEffects.end(); beg != end; ++beg) {
        if (!(*beg)->update(timeFrame)) {
            // remove this one
            remove(*beg);
            --beg;
            --end;
        }
    }
}

} /* namespace effect */
#endif /* EFFECTHANDLER_H_ */
