/*
 * LongWeapon.h
 *
 *  Created on: 28/04/2012
 *      Author: agustin
 *
 */

#ifndef LONGWEAPON_H_
#define LONGWEAPON_H_

#include "Weapon.h"

class LongWeapon : public Weapon
{
public:
	static const float SHOOT_VELOCITY;

	// Long weapons sound codes
	enum {
		SS_SHOOT_CODE,		// Shoot
		SS_RELOAD_CODE		// Reload magazine/shell
	};

public:
	LongWeapon();
	virtual ~LongWeapon();

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
	 *
	 */
	virtual bool shoot(const sm::Vector2 &p);

	/**
	 * Check if we can shoot the weapon or not, if we cannot shoot then
	 * probably have to do a reload, if ammo == 0, then we cannot shoot
	 */
	virtual bool canShoot(void);

	/**
	 * Function called when we perform the reload action
	 */
	virtual void reload(void);

	/**
	 * Function called when we add ammunition to the weapon
	 */
	virtual void setAmmunition(Ammunition &ammo);

	/**
	 * Get the ammunition of the weapon or -1 if have infinite
	 */
	virtual int getAmmunition(void);

	/**
	 * Set the magazine size of the ammunition
	 */
	void configAmmunition(int magazineSize);



protected:
	int				mMagazineSize;
	int				mActualMagazine;
	Ammunition		mAmmunition;

};

#endif /* LONGWEAPON_H_ */
