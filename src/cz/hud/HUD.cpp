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


#include "HUDDefines.h"
#include "HUD.h"


namespace cz {

HUD::HUD()
{
	mElements.reserve(MAX_HUD_ELEMENTS);
}

virtual
HUD::~HUD()
{
	if (mElements.size() > 0) {
		debugWARNING("HUD elements still exist on HUD destruction.\n");
	}
	mElements.clear();
}

///////////////////////////////////////////////////////////////////////////////
void
HUD::updateWeapon(WeaponID id)
{
	// TODO: HUD::updateWeapon
}

///////////////////////////////////////////////////////////////////////////////
void
HUD::updateBullets(Weapon *w, EventType e)
{
	// TODO: HUD::updateBullets
}


}
