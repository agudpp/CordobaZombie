/*
 * BodyWeapon.h
 *
 *  Created on: 11/06/2012
 *      Author: agustin
 */

#ifndef BODYWEAPON_H_
#define BODYWEAPON_H_


#include "Weapon.h"

class BodyWeapon : public Weapon
{
public:
	// Melee weapons sound codes
	enum {
		SS_USE_CODE		// Swing body weapon (could hit or miss)
	};

public:
	BodyWeapon();
	virtual ~BodyWeapon();




	/**
	 * Function called when we perform a shoot
	 */
	virtual bool shoot(const core::Vector2 &p);

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
};

#endif /* BODYWEAPON_H_ */
