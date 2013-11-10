/*
 * HUDbullet.cpp
 *
 *  Created on: Nov 10, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */

#include "HUDDefines.h"
#include "HUDbullet.h"

namespace cz {

///////////////////////////////////////////////////////////////////////////////
HUDbullet::HUDbullet() :
	mBulletCount(0),
	mBulletPanel(0)
{
	mName = HUDBULLET_PANEL_NAME;
}

///////////////////////////////////////////////////////////////////////////////
HUDbullet::~HUDbullet() { /* TODO Auto-generated destructor stub */ }


///////////////////////////////////////////////////////////////////////////////
bool
HUDbullet::build(Ogre::Overlay* ov)
{
	if (!ov) {
		debugERROR("Called with NULL overlay.\n");
		return false;
	}

	// Parse weapons panel from HUD overlay file
	mBulletPanel = static_cast<Ogre::PanelOverlayElement*>(
						ov->getChild(HUDBULLET_PANEL_NAME));
	if (!mBulletPanel) {
		debugERROR("Couldn't open panel %s from overlay %s.\n",
					HUDBULLET_PANEL_NAME, HUD_OV_NAME);
		return false;
	}

	// Set default initial weapon
	setBulletType(initialWeapon);
	mBulletPanel->show();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
void
HUDbullet::setBulletCount(Weapon* w)
{
	// TODO HUDbullet::setBulletCount
}


} /* namespace cz */
