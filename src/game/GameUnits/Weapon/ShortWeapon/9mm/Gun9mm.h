/*
 * Gun9mm.h
 *
 *  Created on: 28/04/2012
 *      Author: agustin
 */

#ifndef GUN9MM_H_
#define GUN9MM_H_

#include "ShortWeapon.h"

class Gun9mm : public ShortWeapon
{
	static const float SHOOT_VELOCITY;
public:
	Gun9mm();
	virtual ~Gun9mm();


	/**
	 * Function called when we perform a shoot
	 * Returns true if we can perform the shoot or false otherwise.
	 * If false is returned then we have to check ammunition, if ammunition == 0
	 * no more bullets to fire, otherwise, we have to perform a reload
	 * We although can check this by calling "canShoot"
	 *
	 * @param	p	The target position to shoot
	 */
	bool shoot(const core::Vector2 &p);

};

#endif /* GUN9MM_H_ */
