/*
 * ShortWeapon.cpp
 *
 *  Created on: 12/03/2012
 *      Author: agustin
 */

#include "GameUnitDefines.h"
#include "GameUnit.h"
#include "LevelManager.h"
#include "GlobalObjects.h"
#include "ShortWeapon.h"




////////////////////////////////////////////////////////////////////////////////
void ShortWeapon::configAmmunition(int magazineSize)
{
	mMagazineSize = magazineSize;
}


ShortWeapon::ShortWeapon() :
Weapon(Weapon::W_SHORT)
{
	// TODO Auto-generated constructor stub

}

ShortWeapon::~ShortWeapon()
{
	// TODO Auto-generated destructor stub
}


/**
 * Function called when we perform a shoot
 */
bool ShortWeapon::shoot(const math::Vector2 &p)
{
	// TODO: deberiamos alomejor implementar aca un disparo de arma normal...
	// como cualquier arma corta...
	ASSERT(false);
	return false;

}


/**
 * Check if we can shoot the weapon or not, if we cannot shoot then
 * probably have to do a reload, if ammo == 0, then we cannot shoot
 */
bool ShortWeapon::canShoot(void)
{
	return mActualMagazine > 0;
}

/**
 * Function called when we perform the reload action
 */
void ShortWeapon::reload(void)
{
	ASSERT(mMagazineSize > 0);
	int diff = (mAmmunition.ammo < mMagazineSize) ? mAmmunition.ammo : mMagazineSize;
	mActualMagazine = diff;
	mAmmunition.ammo -= diff;

	// trigger the signal
	mSignal(this);
}

/**
 * Function called when we add ammunition to the weapon
 */
void ShortWeapon::setAmmunition(Ammunition &ammo)
{
	mAmmunition = ammo;
	mActualMagazine = (ammo.ammo < mMagazineSize)?ammo.ammo : mMagazineSize;
	mAmmunition.ammo -= mMagazineSize;

    // trigger the signal
    mSignal(this);
}

/**
 * Get the ammunition of the weapon or -1 if have infinite
 */
int ShortWeapon::getAmmunition(void)
{
	return mAmmunition.ammo;
}
