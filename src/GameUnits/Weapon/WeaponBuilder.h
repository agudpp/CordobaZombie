/*
 * WeaponBuilder.h
 *
 *  Created on: 11/06/2012
 *      Author: agustin
 */

#ifndef WEAPONBUILDER_H_
#define WEAPONBUILDER_H_

#include <OgreString.h>

#include "XMLHelper.h"
#include "Weapon.h"
#include "Shoot.h"
#include "tinyxml.h"

class TiXmlElement;
class CollisionManager;
class UpdObjsManager;

class WeaponBuilder
{
public:
	WeaponBuilder();
	~WeaponBuilder();

	/**
	 * Configure the number of shoots to use
	 * @param	n		The number of shoots to create for the container
	 * @param	cm		The collisionManager used
	 * @param	uom		The UpdateableObjectsManager
	 */
	void configureShoots(int n, CollisionManager *cm, UpdObjsManager *uom);

	/**
	 * Set the filename to use where there are all the weapons
	 */
	void setFilename(const Ogre::String &fname);

	/**
	 * Returns a weapon by name
	 * @return	w		New allocated weapon (must freed later), or 0 in error
	 */
	Weapon *createWeapon(const Ogre::String &weaponName);

private:
	/**
	 * Create a weapon from an xml element
	 */
	Weapon *buildWeapon(const TiXmlElement *elem);
	Weapon *buildShort(const TiXmlElement *elem);
	Weapon *buildGrenade(const TiXmlElement *elem);
	Weapon *buildLong(const TiXmlElement *elem);
	Weapon *buildBody(const TiXmlElement *elem);

	/**
	 * Get the weapon ID from a name
	 */
	unsigned short getWeaponID(const Ogre::String &name);

private:
	XMLHelper		mHelper;
	ShootContainer	mShoots;
};

#endif /* WEAPONBUILDER_H_ */
