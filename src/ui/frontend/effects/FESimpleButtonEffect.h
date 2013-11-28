/*
 * FESimpleButtonEffect.h
 *
 *  Created on: Nov 28, 2013
 *      Author: agustin
 */

#ifndef FESimpleButtonEffect_H_
#define FESimpleButtonEffect_H_


#include <effect_handler/Effect.h>
#include <overlay_effects/OverlayEffect.h>
#include <frontend/element/button/FESimpleButton.h>

namespace ui {

// @brief By default this class will deactivate the FrontEnd element before
//        starting the overlay effect and then when the overlay effect finishes
//        we will activate the element again.
//

class FESimpleButtonEffect : public effect::Effect
{
public:
    FESimpleButtonEffect();
    virtual
    ~FESimpleButtonEffect();

    // @brief Set the button we want to animate
    // @param button     FrontEndElement
    //
    inline void
    setFESimpleButton(FESimpleButton* button);
    inline FESimpleButton*
    button(void);
    inline const FESimpleButton*
    button(void) const;

    // @brief Set the OverlayEffect to be applied
    // @param oe        Overlay effect
    //
    inline void
    setOverlayEffect(OverlayEffect* oe);
    inline OverlayEffect*
    overlayEffect(void);
    inline const OverlayEffect*
    overlayEffect(void) const;

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

protected:
    FESimpleButton* mElement;
    OverlayEffect* mEffect;
};







////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
FESimpleButtonEffect::setFESimpleButton(FESimpleButton* button)
{
    mElement = button;
}
inline FESimpleButton*
FESimpleButtonEffect::button(void)
{
    return mElement;
}
inline const FESimpleButton*
FESimpleButtonEffect::button(void) const
{
    return mElement;
}

inline void
FESimpleButtonEffect::setOverlayEffect(OverlayEffect* oe)
{
    mEffect = oe;
}
inline OverlayEffect*
FESimpleButtonEffect::overlayEffect(void)
{
    return mEffect;
}
inline const OverlayEffect*
FESimpleButtonEffect::overlayEffect(void) const
{
    return mEffect;
}


} /* namespace ui */
#endif /* FESimpleButtonEffect_H_ */
