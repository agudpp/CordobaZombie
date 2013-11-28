/*
 * SlideOverlayEffect.h
 *
 *  Created on: Nov 28, 2013
 *      Author: agustin
 */

#ifndef SLIDEOVERLAYEFFECT_H_
#define SLIDEOVERLAYEFFECT_H_

#include <functional>

#include <math/Vec2.h>
#include <debug/DebugUtil.h>

#include "OverlayEffect.h"

namespace ui {

class SlideOverlayEffect : public OverlayEffect
{
public:

    // @brief We will use a class to represent the change of velocity that we want
    // to use to translate the overlays in the windows.
    // If we use for example a constant function then we will translate the
    // overlayElement at the same velocity all the time
    // The function will take a value between [0,1] representing the percentage
    // of accumulated time from the start of the translation.
    // It will return a value also between [0,1] representing the percentage of
    // translation from the start point corresponding to the curve defined by
    // this velocity function.
    //
    typedef std::function<float (float)> VelFunction;

    // Some Useful functions here
    static const VelFunction LINEAL_FUNCTION;

public:
    SlideOverlayEffect();
    virtual
    ~SlideOverlayEffect();

    // @brief Set the translation positions
    // @param orig  The begin position
    // @param dest  The destination position
    //
    inline void
    setTranslationPositions(const core::Vector2 &orig,
                            const core::Vector2 &dest);

    // @brief Set the velocity (acceleration) function to be used
    // @param f     The slide function
    //
    inline void
    setFunction(const VelFunction *f);

    // @brief Set the total time that the slide should last
    // @param t must be grater than 0.0f.
    //
    inline void
    setDuration(float t);
    inline float
    duration(void) const;

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

    // @brief Complement the current effect if possible.
    // @return true if we can complement this effect | false otherwise
    //
    virtual bool
    complement(void);

private:
    core::Vector2 mOrig;
    core::Vector2 mDest;
    core::Vector2 mTransVec;
    core::Vector2 mActualPos;
    const VelFunction* mFun;
    // 1.0f / total time that should last the slide movement:
    float mTimeLapse;
    // Accumulated time lapsed in the slide movement:
    float mAccumTime;
};














////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//
inline void
SlideOverlayEffect::setTranslationPositions(const core::Vector2 &orig,
                                            const core::Vector2 &dest)
{
    mOrig = orig;
    mDest = dest;
    mTransVec = dest-orig;
}


inline void
SlideOverlayEffect::setFunction(const VelFunction *f)
{
    mFun = f;
}


inline void
SlideOverlayEffect::setDuration(float t)
{
    ASSERT(t > 0.0f);
    mTimeLapse = 1.0f/t;
}

inline float
SlideOverlayEffect::duration(void) const
{
    return 1.0f / mTimeLapse;
}

} /* namespace ui */
#endif /* SLIDEOVERLAYEFFECT_H_ */
