/*
 * HUD.h
 *
 *  This is the highest level class of the HUD system.
 *  It offers the methods to callback from other game classes.
 *  It handles internally the HUDelement objects.
 *
 *  Created on: Nov 1, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#include <vector>
#include <OgreOverlay.h>

#include <main_player/weapon/Weapon.h>
#include <main_player/weapon/WeaponAction.h>
#include "HUDDefines.h"
#include "HUDweapon.h"


namespace cz {

class HUD
{

public:
	/**
	 * @brief Just create object: no files are parsed, no overlays are created
	 */
	HUD();
	virtual ~HUD();

	/**
	 * @brief   Fill in the structure (create overlays, parse files, etc)
	 * @return  true on success, false otherwise
	 * @remarks Recursively call build for all registered HUDelements
	 * @remarks Starts off hidden. To show HUD call "setVisible(true)"
	 */
	bool
	build(void);

	/**
	 * @brief Tells whether HUD is currently visible
	 */
	bool
	isVisible(void) const;


	/*************************************************************************
	 **  XXX  Methods offered for callbacks binding	                        **
	 **                                                                     **/

	/**
	 * @brief Set visibility of all elements loaded in HUD to 'visible'
	 */
	void
	setVisible(bool visible);

	/**
	 * @brief  Update HUD state due to one of the following events:
	 * 				a) change current selected weapon
	 * 				b) update bullets in magazine due to firing
	 * 				c) update bullets in magazine due to reload
	 */
	void
	updateWeapon(Weapon *w, PlayerWeaponAction act);

	/**                                                                     **
	 **	 <END>  Methods offered for callbacks binding						**
	 *************************************************************************/

private:
	// Prevent the compiler from generating methods to copy the instance.
	HUD(HUD const&);             // Don't implement!
	void operator=(HUD const&);  // Don't implement!

	// Auxiliary internal functions
	void swapWeapon(WeaponID id);
	void fireWeapon(Weapon* w);
	void reloadWeapon(Weapon* w);

private:
	Ogre::Overlay*  mOverlay;
	HUDweapon       mWeapons;
};


///////////////////////////////////////////////////////////////////////////////
inline bool
HUD::isVisible(void) const
{
	if (!mOverlay) {
		debugERROR("Called before creating the overlay.\n");
		return false;
	} else {
		return mOverlay->isVisible();
	}
}


///////////////////////////////////////////////////////////////////////////////
inline void
HUD::setVisible(bool visible)
{
	if (!mOverlay) {
		debugERROR("Called before creating the overlay.\n");
	} else if (visible && !mOverlay->isVisible()) {
		mOverlay->show();
	} else if (!visible && mOverlay->isVisible()) {
		mOverlay->hide();
	}
}


///////////////////////////////////////////////////////////////////////////////
inline void HUD::swapWeapon(WeaponID id) { mWeapons.setWeapon(id); }


}
