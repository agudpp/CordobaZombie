/*
 * TimeBomb.cpp
 *
 *  Created on: 02/05/2012
 *      Author: agustin
 */

#include "GlobalObjects.h"
#include "TimeBomb.h"

TimeBomb::TimeBomb() :
Bomb(Bomb::BOMB_TIME),
mBlowTime(2)
{

}

TimeBomb::~TimeBomb()
{
}


/**
 * Set the time that this bomb have to wait to blow up
 */
void TimeBomb::setBlowTime(float t)
{
	ASSERT(t > 0);
	mBlowTime = t;
}

/**
 * Function called before the object start to bee updated
 */
void TimeBomb::beforeUpdate(void)
{
	// do nothing
	debugBLUE("Probablemente estaria bueno tener una especie de overlays sobre"
			" la bomba que indique el tiempo... cuanto le queda para explotar\n");
}

/**
 * Function to be implemented.
 * This function is called every time the Manager is updated (every frame...)
 */
void TimeBomb::update(void)
{
	mBlowTime -= GLOBAL_TIME_FRAME;
	if(mBlowTime >= 0.0f){
		return;
	}

	// else we fucking blow the bomb
	debugRED("Reproducir sonido + efecto\n");
	performExplosion(mRadius, mPower);

	// stop the update
	stopUpdate();
}

/**
 * Function called when the object stop been updated
 */
void TimeBomb::updateStopped(void)
{
	debugRED("ponerlo de nuevo en la cola de libres\n");
	hide();
}

