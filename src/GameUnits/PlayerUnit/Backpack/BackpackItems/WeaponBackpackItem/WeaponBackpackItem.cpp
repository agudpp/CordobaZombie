/*
 * WeaponBackpackItem.cpp
 *
 *  Created on: 29/05/2012
 *      Author: agustin
 */

#include <OgreOverlayManager.h>

#include "DebugUtil.h"
#include "WeaponBackpackItem.h"
#include "GUIHelper.h"
#include "BackpackDefines.h"
#include "PlayerUnit.h"



/**
 * When the button is left Pressed
 */
void WeaponBackpackItem::leftPressed(void)
{
	ASSERT(mOwner);
	ASSERT(mUserDef);

	// if the actual weapon of the player is this weapon, we do nothing
	if(mOwner->getActualWeapon() == mUserDef){
		// the same weapon, do nothing
		return;
	}
	// else set the actual weapon
	Weapon *w = static_cast<Weapon *>(mUserDef);
	mOwner->setWeapon(w->getType());
}

/**
 * Position the text area were we want
 */
void WeaponBackpackItem::positionText(void)
{
	// get the position of the parent and set the text in the middle
	Ogre::Real midX = mText->getLeft() - mText->getWidth()/2.0f;
	Ogre::Real midY = mText->getTop() - mText->getHeight()/2.0f;

	mText->setTop(0.5f - midY);
	mText->setLeft(0.5f - midX);
}

/**
 * Get the text associated to the weapon ammunition
 */
void WeaponBackpackItem::getAmmoText(Ogre::String &txt)
{

}

WeaponBackpackItem::WeaponBackpackItem() :
BackpackItem(BackpackDef::SECTION_WEAPON),
mText(0)
{

}

WeaponBackpackItem::~WeaponBackpackItem()
{
	// TODO Auto-generated destructor stub
}


void WeaponBackpackItem::setWeapon(Weapon *w)
{
	ASSERT(w);
	ASSERT(!mUserDef);

	mUserDef = w;

	// check if we have to create the overlay
	if(w->getType() != Weapon::W_BODY){
		debugERROR("TODO\n");
//		ASSERT(false);
		/*
		mText = static_cast<Ogre::TextAreaOverlayElement *> (
				Ogre::OverlayManager::getSingleton().getOverlayElement(
						WBI_AMMUNITION_OVERLAY_NAME));
		ASSERT(mText);

		// configure the alignment
		mText->setVerticalAlignment(Ogre::GVA_CENTER);
		mText->setHorizontalAlignment(Ogre::GHA_CENTER);
		*/
	}
}

/**
 * This function is called when the visual item was positioned in the
 * backpack.
 */
void WeaponBackpackItem::updated(void)
{
	if(!mText) return;

	// here we have to put the number of units if we can
	Ogre::String str;
	getAmmoText(str);
	if(str.empty()){
		// hide the text?
		return;
	}
	// set the text
	mText->setCaption(str);
	positionText();
}



