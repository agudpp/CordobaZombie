/*
 * WeaponAction.h
 *
 *  Created on: Nov 9, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef WEAPON_ACTION_H_
#define WEAPON_ACTION_H_

#include <functional>
#include "Weapon.h"

namespace cz {

// Action types that main player can perform with weapons
enum PlayerWeaponAction {
	PWA_SWAP_WEAPON = 0,
	PWA_FIRE,		// Primary fire
	PWA_FIRE_SEC,	// Secondary fire
	PWA_RELOAD,		// Reload magazine
	PWA_ZOOM_IN,	// Scope zoom in
	PWA_ZOOM_OUT,	// Scope zoom out

	PWA_COUNT,
	PWA_INVALID = (1<<31)
};


// Callback definitions for main player weapon usage
typedef std::function<void (Weapon*, PlayerWeaponAction)>	UpdateHUDWeaponCb;


// Debug printing
#ifndef DEBUG
#  define  PWAstr(pwa)
#else
#  define  PWAstr(pwa)  PWAtoStr(pwa)
static inline const char*
PWAtoStr(PlayerWeaponAction pwa)
{
	switch (pwa) {
	case PWA_SWAP_WEAPON:
		return "PWA_SWAP_WEAPON";
		break;
	case PWA_FIRE:
		return "PWA_FIRE";
		break;
	case PWA_FIRE_SEC:
		return "PWA_FIRE_SEC";
		break;
	case PWA_RELOAD:
		return "PWA_RELOAD";
		break;
	case PWA_ZOOM_IN:
		return "PWA_ZOOM_IN";
		break;
	case PWA_ZOOM_OUT:
		return "PWA_ZOOM_OUT";
		break;
	default:
		return "<unrecognized player weapon action>";
		break;
	}
	return "";
}
#endif


}

#endif
