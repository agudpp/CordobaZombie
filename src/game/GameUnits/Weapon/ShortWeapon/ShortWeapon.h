/*
 * ShortWeapon.h
 *
 *  Created on: 12/03/2012
 *      Author: agustin
 */

#ifndef SHORTWEAPON_H_
#define SHORTWEAPON_H_

#include "Weapon.h"

class GameUnit;

class ShortWeapon : public Weapon
{
public:
	// Short weapons sound codes
	enum {
		SS_SHOOT_CODE,		// Shoot
		SS_RELOAD_CODE		// Reload magazine
	};

public:
	ShortWeapon();
	virtual ~ShortWeapon();

	/**
	 * Function called when we perform a shoot
	 */
	virtual bool shoot(const math::Vector2 &p);

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

#endif /* SHORTWEAPON_H_ */
