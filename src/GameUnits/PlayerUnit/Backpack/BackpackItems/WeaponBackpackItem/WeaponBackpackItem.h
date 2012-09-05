/*
 * WeaponBackpackItem.h
 *
 *  Created on: 29/05/2012
 *      Author: agustin
 */

#ifndef WEAPONBACKPACKITEM_H_
#define WEAPONBACKPACKITEM_H_

#include <OgreTextAreaOverlayElement.h>
#include <OgreString.h>

#include "BackpackItem.h"
#include "MenuButton.h"
#include "Weapon.h"



// Define the overlay name used to set the text (weapon ammunitions)
#define	WBI_AMMUNITION_OVERLAY_NAME			"AmmunitionOverlay"

class WeaponBackpackItem : public BackpackItem
{
public:
	WeaponBackpackItem();
	virtual ~WeaponBackpackItem();

	/**
	 * Set/Get the weapon associated to this item
	 */
	void setWeapon(Weapon *w);
	inline Weapon *getWeapon(void);

	/**
	 * This function is called when the visual item was positioned in the
	 * backpack.
	 */
	virtual void updated(void);

protected:
	////////////////////////////////////////////////////////////////////////////
	////						CALLBACKS TO MPLEMENT						////
	////////////////////////////////////////////////////////////////////////////

	/**
	 * When the button is left Pressed
	 */
	virtual void leftPressed(void);


private:
	/**
	 * Position the text area were we want
	 */
	void positionText(void);

	/**
	 * Get the text associated to the weapon ammunition
	 */
	void getAmmoText(Ogre::String &txt);

private:
	Ogre::TextAreaOverlayElement	*mText;
};




inline Weapon *WeaponBackpackItem::getWeapon(void)
{
	return static_cast<Weapon *>(mUserDef);
}


#endif /* WEAPONBACKPACKITEM_H_ */
