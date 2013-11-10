/*
 * HUDweapon.cpp
 *
 *  HUDelement for showing the weapon currently selected by main player.
 *
 *  Created on: Nov 9, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#include <OgreOverlay.h>
#include <OgreOverlayContainer.h>

#include "HUDDefines.h"
#include "HUDweapon.h"

namespace cz {

///////////////////////////////////////////////////////////////////////////////
HUDweapon::HUDweapon() :
	mWeaponsPanel(0),
	mName(HUDWEAPON_PANEL_NAME)
{
	// Auto-generated constructor stub
}

///////////////////////////////////////////////////////////////////////////////
HUDweapon::~HUDweapon() { /* Auto-generated destructor stub */ }


///////////////////////////////////////////////////////////////////////////////
bool
HUDweapon::build(Ogre::Overlay* ov)
{
	if (!ov) {
		debugERROR("Called with NULL overlay.\n");
		return false;
	}

	// Parse weapons panel from HUD overlay file
	mWeaponsPanel = ov->getChild(HUDWEAPON_PANEL_NAME);
	if (!mWeaponsPanel) {
		debugERROR("Couldn't open panel %s from overlay %s.\n",
					HUDWEAPON_PANEL_NAME, HUD_OV_NAME);
		return false;
	}
	mWeaponsPanel->show();

	return true;
}


} /* namespace cz */
