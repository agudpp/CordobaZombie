/*
 * Shotgun.h
 *
 *  Created on: 04/07/2012
 *      Author: agustin
 */

#ifndef SHOTGUN_H_
#define SHOTGUN_H_


#include "LongWeapon.h"


class Shotgun : public LongWeapon
{
	static const int 	NUM_SPREAD_BULLETS			=	5;
	static const float 	SHOOT_VELOCITY;
	static const float	SPREAD_BULLETS_POWER;
public:
	Shotgun();
	virtual ~Shotgun();

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

};

#endif /* SHOTGUN_H_ */
