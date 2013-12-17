/*
 * AlphaOverlayEffect.h
 *
 *  Created on: Nov 28, 2013
 *      Author: agustin
 */

#ifndef ALPHAOVERLAYEFFECT_H_
#define ALPHAOVERLAYEFFECT_H_

#include <OgreTextureUnitState.h>


#include "OverlayEffect.h"

namespace ui {

class AlphaOverlayEffect : public OverlayEffect
{
public:

    // Define the transition type
    enum Type {
        FADE_IN,    // Appear out of thin air
        FADE_OUT    // Dissappear
    };

public:
    AlphaOverlayEffect();
    virtual
    ~AlphaOverlayEffect();

    // @brief Set/get the transition time for this effect
    // @param time      The time we want the transition takes place
    //
    inline void
    setTime(float time);
    inline float
    time(void) const;

    // @brief Set the transition type (type)
    // @param ttype     Transition type {FADE_IN, FADE_OUT}
    //
    inline void
    setTransitionType(Type ttype);
    inline Type
    transitionType(void) const;

    // @brief Set the overlay hidded if we do a FADE_OUT when we finish.
    //        This way we can avoid some rare visual effects.
    // @param letItHidded       If we want to hide the element at the end of the
    //                          effect, by default this is false.
    //
    inline void
    setLetItHidded(bool letItHidded);
    inline bool
    isLetItHidded(void) const;

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
    float mTime;
    float mAccumTime;
    Type mType;
    Ogre::TextureUnitState* mTexture;
    bool mLetItHidded;
};







////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//
inline void
AlphaOverlayEffect::setTime(float time)
{
    mTime = time;
}
inline float
AlphaOverlayEffect::time(void) const
{
    return mTime;
}

inline void
AlphaOverlayEffect::setTransitionType(Type ttype)
{
    mType = ttype;
}
inline AlphaOverlayEffect::Type
AlphaOverlayEffect::transitionType(void) const
{
    return mType;
}

inline void
AlphaOverlayEffect::setLetItHidded(bool letItHidded)
{
    mLetItHidded = letItHidded;
}
inline bool
AlphaOverlayEffect::isLetItHidded(void) const
{
    return mLetItHidded;
}


} /* namespace ui */
#endif /* ALPHAOVERLAYEFFECT_H_ */
