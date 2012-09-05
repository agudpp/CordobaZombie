/*
 * BombBuilder.h
 *
 *  Created on: 17/07/2012
 *      Author: agustin
 */

#ifndef BOMBBUILDER_H_
#define BOMBBUILDER_H_

#include <OgreString.h>

#include "XMLHelper.h"
#include "Bomb.h"
#include "tinyxml.h"

class TiXmlElement;

class CollisionManager;
class UpdObjsManager;

class BombBuilder
{
public:
	BombBuilder();
	~BombBuilder();

	/**
	 * Configure the builder.
	 * @param	cm		The collisionManager used
	 * @param	uom		The UpdatableObjectManager
	 */
	void configure(CollisionManager *cm, UpdObjsManager *uom);

	/**
	 * Set the filename to use where there are all the weapons
	 */
	void setFilename(const Ogre::String &fname);

	/**
	 * Create a bomb from and ID (name)
	 * @param	ID		The name -ID- of the bomb to create
	 */
	Bomb *createBomb(const Ogre::String &ID);


private:

	Bomb *createProximityBomb(const TiXmlElement *elem);
	Bomb *createTimeBomb(const TiXmlElement *elem);
	Bomb *createRemoteBomb(const TiXmlElement *elem);

private:
	XMLHelper		mHelper;
};

#endif /* BOMBBUILDER_H_ */
