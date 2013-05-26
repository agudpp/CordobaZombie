/*
 * LongWeapon.cpp
 *
 *  Created on: 28/04/2012
 *      Author: agustin
 *
 */

#include "DebugUtil.h"
#include "LongWeapon.h"
#include "GameUnit.h"


const float LongWeapon::SHOOT_VELOCITY = 10.0f;


////////////////////////////////////////////////////////////////////////////////
void LongWeapon::configAmmunition(int magazineSize)
{
	mMagazineSize = magazineSize;
}


////////////////////////////////////////////////////////////////////////////////
LongWeapon::LongWeapon() :
Weapon(Weapon::W_LONG)
{
	// force to predefine the shoot queue
	ASSERT(mShootCont);
}


////////////////////////////////////////////////////////////////////////////////
LongWeapon::~LongWeapon() {
	// TODO Auto-generated destructor stub
}

////////////////////////////////////////////////////////////////////////////////
bool LongWeapon::shoot(const core::Vector2 &p)
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

	// trigger the signal
	mSignal(this);

	// configure the shoot
	const core::Vector2 &pos = mOwner->getPosition();
	shoot->setStartPos(Ogre::Vector3(pos.x, mOwner->getPosYAxis(),
			pos.y));

	// create the translate vector
	core::Vector2 tv = p;
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

////////////////////////////////////////////////////////////////////////////////
bool LongWeapon::canShoot(void)
{
	return mActualMagazine > 0;
}

////////////////////////////////////////////////////////////////////////////////
void LongWeapon::reload(void)
{
	ASSERT(mMagazineSize > 0);
	int diff = (mAmmunition.ammo < mMagazineSize) ? mAmmunition.ammo : mMagazineSize;
	mActualMagazine = diff;
	mAmmunition.ammo -= diff;

	// trigger the signal
	mSignal(this);
}

////////////////////////////////////////////////////////////////////////////////
void LongWeapon::setAmmunition(Ammunition &ammo)
{
	mAmmunition = ammo;
	mActualMagazine = (ammo.ammo < mMagazineSize)?ammo.ammo : mMagazineSize;
	mAmmunition.ammo -= mMagazineSize;

	// trigger the signal
	mSignal(this);
}

////////////////////////////////////////////////////////////////////////////////
int LongWeapon::getAmmunition(void)
{
	return mAmmunition.ammo;
}
