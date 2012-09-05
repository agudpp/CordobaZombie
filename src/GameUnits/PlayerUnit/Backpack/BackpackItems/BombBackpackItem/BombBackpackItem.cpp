/*
 * BombBackpackItem.cpp
 *
 *  Created on: 01/08/2012
 *      Author: agustin
 *
 */

#include "BombBackpackItem.h"
#include "DebugUtil.h"


////////////////////////////////////////////////////////////////////////////////
BombBackpackItem::BombBackpackItem() :
mAction(0)
{

}

////////////////////////////////////////////////////////////////////////////////
BombBackpackItem::~BombBackpackItem()
{

}

////////////////////////////////////////////////////////////////////////////////
void BombBackpackItem::updated(void)
{
	debugWARNING("NOT IMPLEMENTED\n");
}

////////////////////////////////////////////////////////////////////////////////
void BombBackpackItem::leftPressed(void)
{
	if(!mAction){
		debugERROR("NO Action Set!\n");
		return;
	}
	// execute the action
	ASSERT(mOwner);
	(*mAction)(mOwner, static_cast<Bomb*>(mUserDef));
}
