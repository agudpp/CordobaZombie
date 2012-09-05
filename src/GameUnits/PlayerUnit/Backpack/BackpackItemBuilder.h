/*
 * BackpackItemBuilder.h
 *
 *  Created on: 23/07/2012
 *      Author: agustin
 *
 */

#ifndef BACKPACKITEMBUILDER_H_
#define BACKPACKITEMBUILDER_H_

#include <OgreString.h>

#include "XMLHelper.h"
#include "tinyxml.h"

class BackpackItem;
class TiXmlElement;

#define BPIBUILDER_DEFAULT_FNAME	"BackpackItems.xml"

class BackpackItemBuilder {
public:
	BackpackItemBuilder(const Ogre::String &fname = BPIBUILDER_DEFAULT_FNAME);
	~BackpackItemBuilder();

	/**
	 * Set the filename to be used
	 */
	void setFilename(const Ogre::String &fname);

	/**
	 * Create a BackPackItem from an ID
	 */
	BackpackItem *createBackpackItem(const Ogre::String &ID);

private:
	BackpackItem *createWeaponBpItem(const TiXmlElement *elem);
	BackpackItem *createBombBpItem(const TiXmlElement *elem);

private:
	XMLHelper		mHelper;
};

#endif /* BACKPACKITEMBUILDER_H_ */
