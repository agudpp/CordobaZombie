/*
 * BackpackItemAssociator.cpp
 *
 *  Created on: 31/07/2012
 *      Author: agustin
 *
 */

#include "BackpackItemAssociator.h"


#include "WeaponBackpackItem.h"
#include "Weapon.h"
#include "WeaponIDs.h"
#include "Bomb.h"
//#include "Item.h"
#include "BackpackItem.h"
#include "DebugUtil.h"


BackpackItemBuilder		BackpackItemAssociator::mBuilder;


////////////////////////////////////////////////////////////////////////////////
void BackpackItemAssociator::setBuilderFilename(const Ogre::String &fname)
{
	mBuilder.setFilename(fname);
}

////////////////////////////////////////////////////////////////////////////////
BackpackItem *BackpackItemAssociator::createBackpackItem(Weapon *w, PlayerUnit *owner)
{
	ASSERT(w);
	ASSERT(owner);

	// we will do some magic here to try to transform the entity weapon name
	// into the real name
	BackpackItem *wbpi = 0;
	unsigned short wid = w->getID();

	switch(wid){
	case WP_ID_9MM:				wbpi = mBuilder.createBackpackItem("9mm");break;
	case WP_ID_FAL:				wbpi = mBuilder.createBackpackItem("Fal");break;
	case WP_ID_GRENADE_NORMAL:	wbpi = mBuilder.createBackpackItem("Grenade");break;
	case WP_ID_MACHETE:			wbpi = mBuilder.createBackpackItem("Machete");break;
	case WP_ID_SNIPER:			wbpi = mBuilder.createBackpackItem("sniper");break;
	case WP_ID_TONFA:			wbpi = mBuilder.createBackpackItem("tonfa");break;
	case WP_ID_MAZO:			wbpi = mBuilder.createBackpackItem("Mazo");break;
	case WP_ID_SHOTGUN:			wbpi = mBuilder.createBackpackItem("Shotgun");break;
	default:
		debugERROR("The type %u of weapon is unknown\n", wid);
		ASSERT(false);
		return 0;
	}
#ifdef DEBUG
	if(!wbpi){
		debugERROR("Weapon couldn't be builded %d\n", static_cast<int>(wid));
		ASSERT(wbpi);
	}
#endif
	// associate the owner and the weapon itself
	wbpi->setOwner(owner);
	static_cast<WeaponBackpackItem *>(wbpi)->setWeapon(w);


	return wbpi;
}

////////////////////////////////////////////////////////////////////////////////
BackpackItem *BackpackItemAssociator::createBackpackItem(Bomb *b, PlayerUnit *owner)
{
	ASSERT(b);
	ASSERT(owner);

	unsigned short bid = b->getType();
	BackpackItem *result = 0;

	switch(bid){
	case Bomb::BOMB_TIME:		result = mBuilder.createBackpackItem("BT"); break;
	case Bomb::BOMB_PROXIMITY:	result = mBuilder.createBackpackItem("BP"); break;
	case Bomb::BOMB_REMOTE:		result = mBuilder.createBackpackItem("BR"); break;
	default:
		debugERROR("Trying to create a bomb that it is unknown %d\n",
				static_cast<int>(bid));
		ASSERT(false);
		return 0;
	}

	ASSERT(result);
	result->setOwner(owner);
	result->setUserDef(b);

	return result;
}

////////////////////////////////////////////////////////////////////////////////
BackpackItem *BackpackItemAssociator::createBackpackItem(GameItem *i,
		PlayerUnit *owner)
{
	ASSERT(false);	return 0;
}
