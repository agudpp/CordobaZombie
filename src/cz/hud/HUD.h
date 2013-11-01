/*
 * HUD.h
 *
 *  This is the highest level class of the HUD system.
 *  It offers the methods to callback from other game classes.
 *  It handles internally the HUDelement objects.
 *
 *  Created on: Nov 1, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */


#include <main_player/weapon/Weapon.h>


namespace cz {

class HUD
{
	void
	updateWeapon(WeaponID id);

	void
	updateBullets(Weapon *w, EventType e);

private:
	// Prevent the compiler from generating methods to copy the instance.
	HUD(HUD const&);             // Don't implement!
	void operator=(HUD const&);  // Don't implement!
};

}
