/* This class will associate a Item {Weapon*, Bomb*, Item*} to a real
 * BackpackItem (GUI). This will create a new one
 * BackpackItemAssociator.h
 *
 *  Created on: 31/07/2012
 *      Author: agustin
 *
 */

#ifndef BACKPACKITEMASSOCIATOR_H_
#define BACKPACKITEMASSOCIATOR_H_

#include <OgreString.h>


#include "BackpackItemBuilder.h"


class Weapon;
class Bomb;
class GameItem;
// class Item;
class BackpackItem;
class PlayerUnit;


class BackpackItemAssociator {
public:

	/**
	 * Configure the filename of the Builder (if we want, this is not necessary,
	 * we can use the "default file")
	 */
	static void setBuilderFilename(const Ogre::String &fname);

	/**
	 * Construct the BackpackItem using the real items.
	 * @return	0	In case of error, or a new BackpackItem (builded with the
	 * 				Builder) in the success case.
	 * @note	If we call 2 times the same function with the same object we will
	 * 			get 2 allocated objects.
	 */
	static BackpackItem *createBackpackItem(Weapon *, PlayerUnit *owner);
	static BackpackItem *createBackpackItem(Bomb *, PlayerUnit *owner);
	static BackpackItem *createBackpackItem(GameItem *, PlayerUnit *owner);


private:

	static	BackpackItemBuilder		mBuilder;

};

#endif /* BACKPACKITEMASSOCIATOR_H_ */
