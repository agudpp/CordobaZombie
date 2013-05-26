/*
 * Weapon.h
 *
 *  Created on: 12/03/2012
 *      Author: agustin
 */

#ifndef WEAPON_H_
#define WEAPON_H_

#include <OgreEntity.h>

#include <boost/signal.hpp>
#include <boost/signals/slot.hpp>

#include <DebugUtil/DebugUtil.h>
#include <Math/CommonMath.h>
#include <GameUnits/GameUnitDefines.h>
#include <SoundSystem/SoundAPI.h>
#include <CollisionSystem/CollisionManager.h>

#include "Shoot/Shoot.h"
#include "WeaponIDs.h"


class Weapon
{
public:

    // Define signals types
    //
    typedef boost::signal<void (Weapon*)> Signal;
    typedef boost::signals::scoped_connection Connection;

public:
    // Ammunition struct
    struct Ammunition {
        int		ammo;
        Ammunition (int am=0) {ammo=am;}
    };

    // Weapons sound codes
    enum {
        SS_EQUIP_WEAPON_CODE	// Grab from back-pack for use as current weapon
    };

    // type of the weapon
    enum {
        W_BODY = 0,
        W_SHORT,
        W_LONG,
        W_GRANADE,
        W_NONE,
    };

public:
    Weapon(int wtype = W_NONE, unsigned short id = WP_ID_NONE);
    virtual ~Weapon();

    /**
     * Set/Get weapon ID
     */
    inline unsigned short getID(void) const;
    inline void setID(unsigned short id);

    /**
     * Get/Set the entity
     */
    inline Ogre::Entity *getEntity(void);
    inline void setEntity(Ogre::Entity *ent);

    /**
     * Set/Get Owner
     */
    void setOwner(GameUnit *u);
    inline GameUnit *getOwner(void);

    /**
     * Set/Get the weapon type
     */
    inline int getType(void) const;
    inline void setType(int t);

    /**
     * Get/Set the range of the weapon
     */
    inline float getSqrRange(void) const;
    inline void setSqrRange(float r);

    /**
     * Get/Set the animation velocity factor (this will be used to incrase the
     * player's animation velocity).
     * @param	velFactor		Animation Velocity Factor
     * @note	By default the value for this is 1.0f, so the velocity will
     * 			be the same as the gameunit skeleton has.
     */
    inline float getAnimVelFactor(void) const;
    inline void setAnimVelFactor(float velFactor);

    /**
     * Set/Get the power of the weapon
     */
    inline float getPower(void) const;
    inline void setPower(float p);

    /**
     * Has effect or not (TODO)
     */
    inline bool hasEffect(void) const;

    /**
     * Get/Set the effect (TODO)
     */
    //inline Effect* getEffect(void) const;
    //inline void setEffect(Effect *e);

    ////////////////////////////////////////////////////////////////////////////
    //						ACTIONS OF THE WEAPON							////
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Function called when we perform a shoot
     * Returns true if we can perform the shoot or false otherwise.
     * If false is returned then we have to check ammunition, if ammunition == 0
     * no more bullets to fire, otherwise, we have to perform a reload
     * We although can check this by calling "canShoot"
     *
     * @param	p	The target position to shoot
     */
    virtual bool shoot(const core::Vector2 &p) = 0;

    /**
     * Second shoot always can be performed
     */
    virtual void secondShoot(const core::Vector2 &p);

    /**
     * Check if we can shoot the weapon or not, if we cannot shoot then
     * probably have to do a reload, if ammo == 0, then we cannot shoot
     */
    virtual bool canShoot(void) = 0;

    /**
     * Function called when we perform the reload action
     */
    virtual void reload(void) = 0;

    /**
     * Function called when we add ammunition to the weapon
     */
    virtual void setAmmunition(Ammunition &ammo) = 0;

    /**
     * Get the ammunition of the weapon or -1 if have infinite
     */
    virtual int getAmmunition(void) = 0;

    /**
     * @brief Subscribe to this weapon to get events when the weapon has changed
     * @param subscriber    The one who want to receive the events
     * @returns conn        The associated connection
     */
    Connection
    addCallback(const Signal::slot_type& subscriber);

protected:
    unsigned short  mID;
    float           mPower;
    float           mRange;
    Ogre::Entity    *mEntity;
    int             mType;
    Hit_t           mHit;
    GameUnit        *mOwner;
    mm::SoundAPI    mSAPI;
    float           mAnimVelocityFactor;
    Signal          mSignal;
    // TODO: Effect		*mEffect;

    static ShootContainer	*mShootCont;
    static c_p::CollisionResult	mCollResult;
};




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////




inline unsigned short Weapon::getID(void) const
{
    return mID;
}
inline void Weapon::setID(unsigned short id)
{
    mID = id;
}
////////////////////////////////////////////////////////////////////////////////
inline Ogre::Entity *Weapon::getEntity(void)
{
    return mEntity;
}
inline void Weapon::setEntity(Ogre::Entity *ent)
{
    ASSERT(ent);
    ASSERT(!mEntity);
    mEntity = ent;
}


////////////////////////////////////////////////////////////////////////////////
inline GameUnit *Weapon::getOwner(void)
{
    return mOwner;
}

////////////////////////////////////////////////////////////////////////////////
inline int Weapon::getType(void) const
{
    return mType;
}
inline void Weapon::setType(int t)
{
    mType = t;
}

////////////////////////////////////////////////////////////////////////////////
inline float Weapon::getSqrRange(void) const
{
    return mRange;
}
inline void Weapon::setSqrRange(float r)
{
    ASSERT(r > 0.0f);
    mRange = r;
}
////////////////////////////////////////////////////////////////////////////////
inline float Weapon::getAnimVelFactor(void) const
{
    return mAnimVelocityFactor;
}
inline void Weapon::setAnimVelFactor(float velFactor)
{
    ASSERT(velFactor > 0);
    mAnimVelocityFactor = velFactor;
}

////////////////////////////////////////////////////////////////////////////////
inline float Weapon::getPower(void) const
{
    return mPower;
}
inline void Weapon::setPower(float p)
{
    ASSERT(p > 0.0f);
    mPower = p;
}

////////////////////////////////////////////////////////////////////////////////
inline bool Weapon::hasEffect(void) const
{
    ASSERT(false); // TODO: implementar
}

#endif /* WEAPON_H_ */
