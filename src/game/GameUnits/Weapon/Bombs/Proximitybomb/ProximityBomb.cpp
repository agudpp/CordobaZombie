/*
 * ProximityBomb.cpp
 *
 *  Created on: 02/05/2012
 *      Author: agustin
 */

#include "ProximityBomb.h"
#include "GlobalObjects.h"



const float  ProximityBomb::CHECK_TIME = 0.5f;


ProximityBomb::ProximityBomb() :
Bomb(Bomb::BOMB_PROXIMITY),
mActivationTime(0),
mAuxTime(0)
{


}

ProximityBomb::~ProximityBomb()
{

}


/**
 * Set the "activation" time (the time that have to pass for the bomb be
 * "activated").
 * @param t		The time in seconds
 */
void ProximityBomb::setActivationTime(float t)
{
	mActivationTime = t;
}

/**
 * Function called before the object start to bee updated
 */
void ProximityBomb::beforeUpdate(void)
{
	// configure the query
	ASSERT(mNode);
	const Ogre::Vector3 &pos = mNode->getPosition();
	mProximityQuery.setSize(mRadius * 2.0f, mRadius *2.0f);
	mProximityQuery.setPosition(math::Vector2(pos.x,pos.z));

	mAuxTime = 0;

	debugGREEN("init bomb\n");
}

/**
 * Function to be implemented.
 * This function is called every time the Manager is updated (every frame...)
 */
void ProximityBomb::update(void)
{
	mAuxTime -= GLOBAL_TIME_FRAME;

	if(mActivationTime > 0.0f){
		mActivationTime -= GLOBAL_TIME_FRAME;

		// we will emmit a sound
		if(mAuxTime < 0.0f){
			debugBLUE("Si quermeos aca deberiamos emitir el beep de la bomba\n");
			mAuxTime = CHECK_TIME;
		}
		return;
	}

	// else... here we have to check if there are units that can trigger the
	// bomb
	if(mAuxTime < 0.0f){
		mAuxTime = CHECK_TIME;

		// perform the check
		mCollMngr->getCollisionObjects(mProximityQuery, PROXIMITY_MASK,
				mCollResult);
		if(mCollResult.empty())
		{
			// nothing happens... return
			return;
		}

		// else... we have to blow the bomb
		debugRED("Aca tenemos que:\n "
					"1) Reproducir efecto de explosion.\n"
					"2) Reproducir sonido de explosion\n"
					"Notar que estamos explotando la bomba aun asi si no tenemos"
					" \"linea de vision\" directa con el objetivo. Tendriamos"
					" en todo caso que implementar una funcion en Bomb que permita"
					" chequear esto\n");
		performExplosion(mRadius, mPower);
		stopUpdate();
	}
}

/**
 * Function called when the object stop been updated
 */
void ProximityBomb::updateStopped(void)
{
	debugRED("Deberiamos volverla a poner en alguna queue o algo asi\n");
	hide();
}
