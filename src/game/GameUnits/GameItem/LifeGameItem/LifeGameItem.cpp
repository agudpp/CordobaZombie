/*
 * LifeGameItem.cpp
 *
 *  Created on: 01/08/2012
 *      Author: agustin
 *
 */

#include "LifeGameItem.h"
#include "PlayerUnit.h"

LifeGameItem::LifeGameItem() :
mLife(0)
{


}

LifeGameItem::~LifeGameItem()
{
}


/**
 * Use the medikit in the same unit
 */
void LifeGameItem::useIt(void)
{
	ASSERT(mOwner);
	mOwner->setLife(mOwner->getLife() + mLife);

	debugWARNING("We assume that the owner is a player\n");

	// remove the item from the player
	static_cast<PlayerUnit *>(mOwner)->removeItem(this);
}

