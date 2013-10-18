/*
 * WeaponFAL.h
 *
 *  Created on: Oct 10, 2013
 *      Author: agustin
 */

#ifndef WEAPONFAL_H_
#define WEAPONFAL_H_


#include "BulletBasedWeapon.h"


namespace cz {

class WeaponFAL : public BulletBasedWeapon
{

    // States of the weapon
    //
    enum State {
        S_FIRING = 0,
        S_RELOADING,
        S_IDLE,

        S_COUNT,
    };

    static const char* ANIM_NAMES[S_COUNT] = {
        "dispara",
        "recarga",
        0
    };

public:
    WeaponFAL();
    virtual
    ~WeaponFAL();

    ////////////////////////////////////////////////////////////////////////////
    //                  Methods to implement by each weapon                   //
    ////////////////////////////////////////////////////////////////////////////

    // @brief This method will be called to add bullets / magazines
    // @param magazines     The number of magazines we want to add (it is assumed
    //                      that each magazine contains the maximum number of
    //                      bullets)
    // @param bullets       The number of bullets to the current magazine
    //
    virtual void
    addAmmunition(unsigned short magazines, unsigned short bullets);

    // @brief Check if the current weapon can fire or not (probably because it
    //        hasn't more bullets or because it is in the middle of any process).
    //
    virtual bool
    canFire(void) const;

    // @brief Check if we can reload right now (probably we cannot because we
    //        are firing / already loading / no more bullets / any).
    //
    virtual bool
    canReload(void) const;

    // @brief This method will be called to reload the current weapon.
    // @return true if it is able to reload | false otherwise
    //
    virtual bool
    reload(void);

    // @brief This method will fire the weapon and return true if it could do it
    //        or false otherwise
    // @return true if it could | false otherwise
    //
    virtual bool
    fire(void);

    // @brief This method should be called each frame to update the logic of the
    //        weapon (this implies the current animation / etc).
    //
    virtual void
    update(float timeFrame);

    // @brief Perform any particular operation to this weapon. This could be
    //        used to activate / deactivate specific features to the weapon.
    // @param feature       The feature argument, for now this will be an int...
    //
    virtual void
    setFeature(WFeature feature);

private:
    State mState;
};

} /* namespace cz */
#endif /* WEAPONFAL_H_ */
