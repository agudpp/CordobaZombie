/*
 * HUDweapon.h
 *
 *  HUDelement for showing the weapon currently selected by main player.
 *
 *  Created on: Nov 9, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef HUDWEAPON_H_
#define HUDWEAPON_H_

#include <OgrePanelOverlayElement.h>
#include <main_player/weapon/WeaponDefs.h>
#include <main_player/weapon/WeaponAction.h>
#include "HUDelement.h"

namespace cz {

class HUDweapon : public HUDelement
{
	// @brief Weapon texture relative width
	static constexpr float wTexWidth = 1.0f/PlayerWeaponAction::PWA_COUNT;

public:
	HUDweapon();
	virtual ~HUDweapon();

public:
	/**
	 * @brief Set element overlay container visibility to 'visible'
	 */
	void
	setVisible(bool visible);

	/**
	 * @brief  Fill in the structure (create overlays, parse files, etc)
	 * @return true on success, false otherwise
	 */
	bool
	build(Ogre::Overlay* ov);

	/**
	 * @brief Set current selected weapon to 'id'
	 */
	void
	setWeapon(WeaponID id);

private:
	Ogre::PanelOverlayElement* mWeaponsPanel;  // Weapon panel display
};


///////////////////////////////////////////////////////////////////////////////
inline void
HUDweapon::setVisible(bool visible)
{
	if (!mWeaponsPanel) {
		debugERROR("Called before creating the weapon panel.\n");
	} else if (visible && !mWeaponsPanel->isVisible()) {
		mWeaponsPanel->show();
	} else if (!visible && mWeaponsPanel->isVisible()) {
		mWeaponsPanel->hide();
	}
}


///////////////////////////////////////////////////////////////////////////////
inline void
HUDweapon::setWeapon(WeaponID id)
{
	if (!mWeaponsPanel) {
		debugERROR("Called before creating the weapon panel.\n");
	} else {
		// Set texture UV coordinates for selected weapon
		mWeaponsPanel->setUV(wTexWidth*(id) ,	// left
							 0.0f,				// bottom
							 wTexWidth*(id+1),	// right
							 1.0f);				// up
	}
	return;
}


} /* namespace cz */

#endif /* HUDWEAPON_H_ */
