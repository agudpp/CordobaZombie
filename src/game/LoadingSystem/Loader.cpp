/*
 * Loader.cpp
 *
 *  Created on: 15/05/2012
 *      Author: agustin
 */

#include <OgreString.h>
#include <OgreStringConverter.h>

#include "DebugUtil.h"
#include "Loader.h"

/**
 * Parse auxiliaries
 */
void Loader::parseAABB(TiXmlElement *elem, sm::AABB &aabb)
{
	ASSERT(elem);

	// get the aabb
	const char *aux = elem->Attribute("tlx");
	ASSERT(aux);
	float tlx = Ogre::StringConverter::parseReal(aux);

	aux = elem->Attribute("tly");
	ASSERT(aux);
	float tly = Ogre::StringConverter::parseInt(aux);

	aux = elem->Attribute("brx");
	ASSERT(aux);
	float brx = Ogre::StringConverter::parseInt(aux);

	aux = elem->Attribute("bry");
	ASSERT(aux);
	float bry = Ogre::StringConverter::parseInt(aux);

	ASSERT(tlx <= brx);
	ASSERT(tly >= bry);

	aabb.setSize(brx - tlx, tly - bry);
	aabb.setPosition(sm::Vector2((tlx + brx)/2.0f, (tly + bry)/2.0f));
}

void Loader::parseVector(TiXmlElement *elem, sm::Vector2 &vec)
{
	ASSERT(elem);
	const char *aux = elem->Attribute("x");
	ASSERT(aux);
	vec.x = Ogre::StringConverter::parseReal(aux);
	aux = elem->Attribute("x");
	ASSERT(aux);
	vec.y = Ogre::StringConverter::parseReal(aux);
}
