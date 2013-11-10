/*
 * HUD.cpp
 *
 *  This is the highest level class of the HUD system.
 *  It offers the methods to callback from other game classes.
 *  It handles internally the HUDelement objects.
 *
 *  Created on: Nov 1, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */


#include <OgreOverlay.h>
#include <OgreOverlayManager.h>

#include <main_player/weapon/Weapon.h>
#include <debug/DebugUtil.h>
#include "HUDDefines.h"
#include "HUD.h"


namespace cz {

///////////////////////////////////////////////////////////////////////////////
HUD::HUD() : mOverlay(0) { /* Auto-generated constructor stub */ }

///////////////////////////////////////////////////////////////////////////////
HUD::~HUD()
{
	mOverlay->clear();
	delete mOverlay;
}


///////////////////////////////////////////////////////////////////////////////
bool
HUD::build(void)
{
	bool wres(true), bres(true);

	// Build main HUD overlay
	mOverlay = Ogre::OverlayManager::getSingleton().getByName(HUD_OV_NAME);
	if (!mOverlay) {
		debugERROR("Error creating HUD overlay.\n");
		return false;
	}

	// Call build for every registered HUDelement
	wres = mWeapons.build(mOverlay);
	if (!wres) {
		debugERROR("Error building HUDelement \"%s\".\n", mWeapons.getName());
	}
	bres = mBullet.build(mOverlay);
	if (!bres) {
		debugERROR("Error building HUDelement \"%s\".\n", mBullet.getName());
	}

	// Start off hidden
	mOverlay->hide();
	return (wres && bres);
}


///////////////////////////////////////////////////////////////////////////////
void
HUD::updateWeapon(const Weapon* w, PlayerWeaponEvent wev)
{
	if (!w) {
		debugERROR("Called with NULL Weapon*.\n");
		return;
	}

	switch (wev) {

	case PWE_SWAP_WEAPON:
		mWeapons.setWeapon(w->weaponID());
		mBullet.setBulletType(w->weaponID());
		mBullet.setBulletCount(w->weaponInfo());
		break;

	case PWE_FIRE:
		mBullet.setBulletCount(w->weaponInfo());
		break;

	case PWE_RELOAD:
		mBullet.setBulletCount(w->weaponInfo());
		break;

	default:
		debugERROR("Unsupported PlayerWeaponEvent: %s", PWEstr(wev));
		break;
	}
}


}
