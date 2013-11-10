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
HUD::updateWeapon(WeaponID id)
{
	mWeapons.setWeapon(id);
	// TODO: change panel with bullet type && change magazine count numbers
}


///////////////////////////////////////////////////////////////////////////////
void
HUD::updateBullets(Weapon *w, PlayerWeaponAction act)
{
	// TODO: HUD::updateBullets
}


}
