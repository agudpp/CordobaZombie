/*
 * Gun9mm.cpp
 *
 *  Created on: 28/04/2012
 *      Author: agustin
 */

#include <OgreMath.h>
#include <OgreVector3.h>

#include "Gun9mm.h"
#include "GameUnit.h"



const float Gun9mm::SHOOT_VELOCITY = 10.0f;



Gun9mm::Gun9mm()
{
	// TODO: cambiar esto
	configAmmunition(15);

}

Gun9mm::~Gun9mm()
{
	// TODO Auto-generated destructor stub
}



/**
 * Function called when we perform a shoot
 * Returns true if we can perform the shoot or false otherwise.
 * If false is returned then we have to check ammunition, if ammunition == 0
 * no more bullets to fire, otherwise, we have to perform a reload
 * We although can check this by calling "canShoot"
 *
 * @param	p	The target position to shoot
 */
bool Gun9mm::shoot(const math::Vector2 &p)
{
	// check if we can shoot
	if(mActualMagazine <= 0){
		return false;
	}

	// else we performe the shoot
	Shoot *shoot = mShootCont->getAvailableObject();
	if(!shoot){
		ASSERT(shoot);
		return false;
	}
	--mActualMagazine;

	// configure the shoot
	const math::Vector2 &pos = mOwner->getPosition();
	shoot->setStartPos(Ogre::Vector3(pos.x, mOwner->getPosYAxis(),
			pos.y));

	// create the translate vector
	math::Vector2 tv = p;
	tv -= mOwner->getPosition();
	tv.normalize();
	tv *= SHOOT_VELOCITY;

	// add some random
	tv.x += Ogre::Math::RangeRandom(-0.0009f, 0.0009f);
	tv.y += Ogre::Math::RangeRandom(-0.0009f, 0.0009f);
//	const float rndPos = Ogre::Math::RangeRandom(-0.0009f, 0.0009f);
	shoot->setTranslateVec(Ogre::Vector3(tv.x,
			0 , tv.y));
	shoot->setHitPower(mPower);
	shoot->startUpdate();
}



