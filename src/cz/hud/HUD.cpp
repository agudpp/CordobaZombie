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
	bool result(true);

	// Build main HUD overlay
	mOverlay = Ogre::OverlayManager::getSingleton().getByName(HUD_OV_NAME);
	if (!mOverlay) {
		debugERROR("Error creating HUD overlay.\n");
		return false;
	}

	// Call build for every registered HUDelement
	result = mWeapons.build(mOverlay);
	if (!result) {
		debugERROR("Error building HUDelement \"%s\".\n", mWeapons.getName());
	}

	// Start off hidden
	mOverlay->hide();
	return result;
}


///////////////////////////////////////////////////////////////////////////////
void
HUD::updateWeapon(Weapon* w, PlayerWeaponAction act)
{
	if (!w) {
		debugERROR("Called with NULL Weapon*.\n");
		return;
	}

	switch (act) {

	case PWA_SWAP_WEAPON:
		swapWeapon(w->weaponID());
		break;

	case PWA_FIRE:
		fireWeapon(w);
		break;

	case PWA_RELOAD:
		reloadWeapon(w);
		break;

	default:
		debugERROR("Unsupported PlayerWeaponAction: %s", PWAstr(act));
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////
void
HUD::fireWeapon(Weapon* w)
{
	// TODO HUD::fireWeapon
}


///////////////////////////////////////////////////////////////////////////////
void
HUD::reloadWeapon(Weapon* w)
{
	// TODO HUD::reloadWeapon
}


}
