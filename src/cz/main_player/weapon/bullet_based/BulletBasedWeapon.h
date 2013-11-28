/*
 * BulletBasedWeapon.h
 *
 *  Created on: Oct 9, 2013
 *      Author: agustin
 */

#ifndef BULLETBASEDWEAPON_H_
#define BULLETBASEDWEAPON_H_

#include <main_player/weapon/Weapon.h>


namespace cz {

// Forward
//
class Bullet;

class BulletBasedWeapon : public Weapon
{
public:
    BulletBasedWeapon() {};
    virtual
    ~BulletBasedWeapon() {};

    // @brief We need to set the associated firing system to this kind of classes
    //        as an static pointer (shared for all the classes).
    //        This firing system will contain the information needed to perform
    //        shoots.
    // TODO: complete these!

    // @brief Set the possible weapon movement ranges when shooting a bullet.
    // @param deltaX        The possible movement in the X axis (this will be in
    //                      the range of [currentX - deltaX, currentX + deltaX].
    // @param deltaY        The possible movement in the Y axis (this will be in
    //                      the range of [currentY - deltaY, currentY + deltaY].
    //
    inline void
    setMovementRanges(float deltaX, float deltaY);

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
    addAmmunition(unsigned short magazines, unsigned short bullets) = 0;

    // @brief Check if the current weapon can fire or not (probably because it
    //        hasn't more bullets or because it is in the middle of any process).
    //
    virtual bool
    canFire(void) const = 0;

    // @brief Check if we can reload right now (probably we cannot because we
    //        are firing / already loading / no more bullets / any).
    //
    virtual bool
    canReload(void) const = 0;

    // @brief This method will be called to reload the current weapon.
    // @return true if it is able to reload | false otherwise
    //
    virtual bool
    reload(void) = 0;

    // @brief This method will fire the weapon and return true if it could do it
    //        or false otherwise
    // @return true if it could | false otherwise
    //
    virtual bool
    fire(void) = 0;

    // @brief This method should be called each frame to update the logic of the
    //        weapon (this implies the current animation / etc).
    //
    virtual void
    update(float timeFrame) = 0;

    // @brief Perform any particular operation to this weapon. This could be
    //        used to activate / deactivate specific features to the weapon.
    // @param feature       The feature argument, for now this will be an int...
    //
    virtual void
    setFeature(WFeature feature) = 0;

protected:

    // @brief This method will be used to fire a bullet using the current
    //        firing direction information and the FiringSystem associated.
    //        This method will use the associated delta movements when firing.
    // @param bullet        The bullet instance we will fire.
    //
    void
    fireBullet(Bullet* bullet);

protected:
    float mDeltaX;
    float mDeltaY;
};




////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
BulletBasedWeapon::setMovementRanges(float deltaX, float deltaY)
{
    mDeltaX = deltaX;
    mDeltaY = deltaY;
}

} /* namespace cz */
#endif /* BULLETBASEDWEAPON_H_ */
