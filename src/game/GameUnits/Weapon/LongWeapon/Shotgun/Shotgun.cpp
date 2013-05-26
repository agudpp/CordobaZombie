/*
 * Shotgun.cpp
 *
 *  Created on: 04/07/2012
 *      Author: agustin
 */

#include "DebugUtil.h"
#include "Shotgun.h"
#include "GameUnit.h"


const float  Shotgun::SHOOT_VELOCITY = 5.0f;
const float  Shotgun::SPREAD_BULLETS_POWER = 1.0f/NUM_SPREAD_BULLETS;


Shotgun::Shotgun()
{
	// We configure the magazineSize as 8
	configAmmunition(8);
	debugWARNING("Hardcodeamos el tamanio de los magazine de la shotgun a 8\n");
}

Shotgun::~Shotgun()
{

}


/**
 * Function called when we perform a shoot
 * Returns true if we can perform the shoot or false otherwise.
 * If false is returned then we have to check ammunition, if ammunition == 0
 * no more bullets to fire, otherwise, we have to perform a reload
 * We although can check this by calling "canShoot"
 *
 * @param	p	The target position to shoot
 *
 */
bool Shotgun::shoot(const core::Vector2 &p)
{
	// check if we can shoot
	if(mActualMagazine <= 0){
		return false;
	}

	// else we performe the shoot
	Shoot *shoot[NUM_SPREAD_BULLETS];
	int i = 0;
	for(; i < NUM_SPREAD_BULLETS; ++i){
		shoot[i] = mShootCont->getAvailableObject();
		if(!shoot[i]) break;
	}
	if(i < NUM_SPREAD_BULLETS-1){
		debugWARNING("Cannot get all the bullets we need: %d\n", i);
	}
	--mActualMagazine;

	// configure the shoot
	const core::Vector2 &pos = mOwner->getPosition();
	Ogre::Vector3 spos(pos.x, mOwner->getPosYAxis(), pos.y);
	core::Vector2 tv;
	const float realPower = mPower * SPREAD_BULLETS_POWER;
	for(int j = 0; j < i; ++j){
		shoot[j]->setStartPos(spos);

		// create the translate vector
		tv = p;
		tv -= mOwner->getPosition();
		tv.normalize();
		// add some random
		tv.x += Ogre::Math::RangeRandom(-0.2f, 0.2f);
		tv.y += Ogre::Math::RangeRandom(-0.2f, 0.2f);
		tv.normalize();
		tv *= SHOOT_VELOCITY;
		shoot[j]->setTranslateVec(Ogre::Vector3(tv.x,
				Ogre::Math::RangeRandom(-0.05f, 0.05f),
				tv.y));
		shoot[j]->setHitPower(realPower);
		shoot[j]->startUpdate();
	}
}


