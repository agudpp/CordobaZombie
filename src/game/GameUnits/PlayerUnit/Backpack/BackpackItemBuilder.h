/*
 * BackpackItemBuilder.h
 *
 *  Created on: 23/07/2012
 *      Author: agustin
 *
 */

#ifndef BACKPACKITEMBUILDER_H_
#define BACKPACKITEMBUILDER_H_

#include "BackpackItem.h"


// Forwards
//
class Weapon;
class Bomb;
class GameItem;


namespace BackpackItemBuilder {

	/**
	 * @brief Create a BackPackItem from an object
	 * @returns The allocated backpackitem
	 */
	BackpackItemPtr
	createBackpackItem(Weapon*);
	BackpackItemPtr
	createBackpackItem(Bomb*);
	BackpackItemPtr
	createBackpackItem(GameItem*);

}

#endif /* BACKPACKITEMBUILDER_H_ */
