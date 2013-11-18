/*
 * MainPlayerWeaponEvent.h
 *
 *  Created on: Nov 10, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef WEAPON_ACTION_H_
#define WEAPON_ACTION_H_

#include <functional>


namespace cz {

class Weapon;

// Event types that main player can perform with weapons
enum PlayerWeaponEvent {
	PWE_SWAP_WEAPON = 0,
	PWE_FIRE,		// Primary fire
	PWE_FIRE_SEC,	// Secondary fire
	PWE_RELOAD,		// Reload magazine
	PWE_ZOOM_IN,	// Scope zoom in
	PWE_ZOOM_OUT,	// Scope zoom out

	PWE_COUNT,
	PWE_INVALID = (1<<31)
};


// Callback definitions for main player weapon usage
typedef std::function<void (Weapon*, PlayerWeaponEvent)>	WeaponEventCb;


// Debug printing
#ifndef DEBUG
#  define  PWEstr(pwe)
#else
#  define  PWEstr(pwe)  PWEtoStr(pwe)
static inline const char*
PWEtoStr(PlayerWeaponEvent pwe)
{
	switch (pwe) {
	case PWE_SWAP_WEAPON:
		return "PWE_SWAP_WEAPON";
		break;
	case PWE_FIRE:
		return "PWE_FIRE";
		break;
	case PWE_FIRE_SEC:
		return "PWE_FIRE_SEC";
		break;
	case PWE_RELOAD:
		return "PWE_RELOAD";
		break;
	case PWE_ZOOM_IN:
		return "PWE_ZOOM_IN";
		break;
	case PWE_ZOOM_OUT:
		return "PWE_ZOOM_OUT";
		break;
	default:
		return "<unrecognized player weapon event>";
		break;
	}
	return "";
}
#endif


}

#endif
