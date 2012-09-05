/*
 * RemoteBomb.cpp
 *
 *  Created on: 17/07/2012
 *      Author: agustin
 */

#include "RemoteBomb.h"

RemoteBomb::RemoteBomb() :
Bomb(Bomb::BOMB_REMOTE),
mExploded(false)
{

}

RemoteBomb::~RemoteBomb()
{

}

void RemoteBomb::blow(void)
{
	ASSERT(!mExploded);

	debugERROR("Aca tenemos que reproducir sonido + efectos + etc\n");
	performExplosion(mRadius, mPower);
	mExploded = true;
	hide();
}
