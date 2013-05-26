/*
 * BodyWeapon.cpp
 *
 *  Created on: 11/06/2012
 *      Author: agustin
 */


#include "GameUnitDefines.h"
#include "GameUnit.h"
#include "LevelManager.h"
#include "GlobalObjects.h"
#include "BodyWeapon.h"

BodyWeapon::BodyWeapon() :
Weapon(Weapon::W_BODY)
{
	// TODO Auto-generated constructor stub

}

BodyWeapon::~BodyWeapon()
{
	// TODO Auto-generated destructor stub
}





/**
 * Function called when we perform a shoot
 */
bool BodyWeapon::shoot(const core::Vector2 &p)
{
	ASSERT(mOwner);

	if(mOwner->getPosition().squaredDistance(p) > mRange){
		return true;
	}

	// TODO: tendriamos que ver de cambiar los flags aca si queremos por ejemplo
	// poder romper una caja? no es un zombie... asique deberiamos poder pegarle
	GLOBAL_LVL_MNGR->getCollisionManager()->getCollisionObjects(p,
			c_p::COL_FLAG_UNIT_ZOMBIE, mCollResult);

	if(mCollResult.empty()){
		debug("No object found\n");
		return true;
	}

	// else we have impact something, get the closest
	GameUnit *gu = static_cast<GameUnit *>(mCollResult[0]->userDefined);
	float minDist = gu->getPosition().squaredDistance(p);
	for(int i = mCollResult.size(); i >= 1; --i){
		const float dist = mCollResult[i]->getPosition().squaredDistance(p);
		if(dist < minDist){
			gu = static_cast<GameUnit *>(mCollResult[i]->userDefined);
		}
	}

	// gu is the closer game unit (zombie)... hit the mother fucker
	mHit.hitDir = p;
	mHit.hitDir -= mOwner->getPosition();
	mHit.power = mPower;
	mHit.shooter = 0;
	gu->beenHit(mHit);

	return true;
}


/**
 * Check if we can shoot the weapon or not, if we cannot shoot then
 * probably have to do a reload, if ammo == 0, then we cannot shoot
 */
bool BodyWeapon::canShoot(void)
{
	return true;
}

/**
 * Function called when we perform the reload action
 */
void BodyWeapon::reload(void)
{

	return ;
}

/**
 * Function called when we add ammunition to the weapon
 */
void BodyWeapon::setAmmunition(Ammunition &ammo)
{
	// do nothing
	return;
}

/**
 * Get the ammunition of the weapon or -1 if have infinite
 */
int BodyWeapon::getAmmunition(void)
{
	return -1;
}
