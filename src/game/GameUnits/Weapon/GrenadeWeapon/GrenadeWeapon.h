/*
 * GrenadeWeapon.h
 *
 *  Created on: 11/06/2012
 *      Author: agustin
 */

#ifndef GRENADEWEAPON_H_
#define GRENADEWEAPON_H_

#include "Weapon.h"

class GrenadeWeapon
{
public:
	// Grenades sound codes
	enum {
		SS_THROW_CODE,		// Throw grenade into the air
		SS_EXPLODE_CODE		// Explosion
	};

public:
	GrenadeWeapon();
	virtual ~GrenadeWeapon();
};

#endif /* GRENADEWEAPON_H_ */
