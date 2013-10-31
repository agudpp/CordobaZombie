/*
 * EffectWrapper.h
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#ifndef EFFECTWRAPPER_H_
#define EFFECTWRAPPER_H_

#include <debug/DebugUtil.h>

#include "Effect.h"
#include "EffectHandler.h"

namespace effect {

class EffectWrapper
{
public:
    // @brief Construct with the instances
    //
    inline EffectWrapper(Effect* effect = 0, EffectHandler* handler = 0);
    inline ~EffectWrapper();

    // @brief Return the associated instances
    //
    inline Effect*
    effect(void);
    inline const Effect*
    effect(void) const;
    inline EffectHandler*
    effectHandler(void);
    inline const EffectHandler*
    effectHandler(void) const;

    // @brief Set the instances to be used here
    //
    inline void
    setEffect(Effect* effect);
    inline void
    setHandler(EffectHandler* handler);

    // @brief Start the current effect in the associated handler
    //
    inline void
    startEffect(void);

    // @brief Stop the current effect in the associated handler
    //
    inline void
    stopEffect(void);

    // @brief Check if the current effect is being executed in the associated
    //        handler
    //
    inline bool
    isRunning(void) const;

private:
    Effect* mEffect;
    EffectHandler* mHandler;
};




////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline EffectWrapper::EffectWrapper(Effect* effect, EffectHandler* handler) :
    mEffect(effect)
,   mHandler(handler)
{
}
inline EffectWrapper::~EffectWrapper()
{}

inline Effect*
EffectWrapper::effect(void)
{
    return mEffect;
}
inline const Effect*
EffectWrapper::effect(void) const
{
    return mEffect;
}
inline EffectHandler*
EffectWrapper::effectHandler(void)
{
    return mHandler;
}
inline const EffectHandler*
EffectWrapper::effectHandler(void) const
{
    return mHandler;
}


inline void
EffectWrapper::setEffect(Effect* effect)
{
    mEffect = effect;
}
inline void
EffectWrapper::setHandler(EffectHandler* handler)
{
    mHandler = handler;
}

inline void
EffectWrapper::startEffect(void)
{
    ASSERT(mHandler);
    ASSERT(mEffect);
    mHandler->add(mEffect);
}

inline void
EffectWrapper::stopEffect(void)
{
    ASSERT(mHandler);
    ASSERT(mEffect);
    mHandler->remove(mEffect);
}

inline bool
EffectWrapper::isRunning(void) const
{
    ASSERT(mHandler);
    ASSERT(mEffect);
    return mHandler->contains(mEffect);
}

} /* namespace effect */
#endif /* EFFECTWRAPPER_H_ */
