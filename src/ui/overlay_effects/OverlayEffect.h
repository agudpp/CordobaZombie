/*
 * OverlayEffect.h
 *
 *  Created on: Nov 28, 2013
 *      Author: agustin
 */

#ifndef OVERLAYEFFECT_H_
#define OVERLAYEFFECT_H_

#include <OgreOverlayElement.h>

#include <effect_handler/Effect.h>

namespace ui {

class OverlayEffect : public effect::Effect
{
public:

public:
    OverlayEffect(Ogre::OverlayElement* element = 0) :
        mElement(element)
    {}
    virtual
    ~OverlayEffect()
    {}

    // @brief Set the overlay element to be animated / or to apply the effect.
    // @param element       The OgreOverlayElement
    //
    inline void
    setOverlayElement(Ogre::OverlayElement* element);
    inline Ogre::OverlayElement*
    overlayElement(void);
    inline const Ogre::OverlayElement*
    overlayElement(void) const;

    ////////////////////////////////////////////////////////////////////////////
    // Inherited methods
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

    // @brief Complement the current effect if possible.
    // @return true if we can complement this effect | false otherwise
    //
    virtual bool
    complement(void) = 0;

protected:
    Ogre::OverlayElement* mElement;
};








////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
OverlayEffect::setOverlayElement(Ogre::OverlayElement* element)
{
    mElement = element;
}
inline Ogre::OverlayElement*
OverlayEffect::overlayElement(void)
{
    return mElement;
}
inline const Ogre::OverlayElement*
OverlayEffect::overlayElement(void) const
{
    return mElement;
}

} /* namespace ui */
#endif /* OVERLAYEFFECT_H_ */
