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
	PWA_FIRE = 0,	// Primary fire
	PWA_FIRE_SEC,	// Secondary fire
	PWA_RELOAD,		// Reload magazine
	PWA_ZOOM_IN,	// Scope zoom in
	PWA_ZOOM_OUT,	// Scope zoom out
	PWA_COUNT,
	PWA_INVALID = (1<<31)
};

// Callback definitions for main player weapon usage
typedef std::function<void (WeaponID)>						SwapWeaponCb;
typedef std::function<void (Weapon*, PlayerWeaponAction)>	UseWeaponCb;

}

#endif
