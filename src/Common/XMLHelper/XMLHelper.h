/*
 * XMLHelper.h
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#ifndef XMLHelper_H_
#define XMLHelper_H_

#include <OgreString.h>

#include <vector>

#include "Util.h"
#include "tinyxml.h"

class TiXmlElement;
class TiXmlDocument;

class XMLHelper
{
public:
	XMLHelper();
	~XMLHelper();

	/**
	 * Set the xml filename
	 */
	void setFilename(const Ogre::String &name);

	/**
	 * Open/Close the filename
	 */
	void openXml(void);
	void closeXml(void);

	/**
	 * Find an element (with attribute name) in the first nested elements
	 * (starting from rootElement->getFirstChildElement())
	 * @param	name		The name to looking for
	 * @param	attrName	The attribute name (usually "name")
	 * @return	0			On error, or the element if we find it
	 */
	const TiXmlElement *findElement(const char *name, const char *attrName = "name");

	/**
	 * Returns all the first childs element of the root element
	 */
	void getFirstElements(std::vector<const TiXmlElement *> &elements);

	/**
	 * Returns the root element of the xml
	 */
	const TiXmlElement *getRootElement(void);

private:
	Ogre::String		mFileName;
	TiXmlDocument		*mDocument;

};

#endif /* XMLHelper_H_ */
