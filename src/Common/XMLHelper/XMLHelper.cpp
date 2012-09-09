/*
 * XMLHelper.cpp
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#include "Util.h"
#include "XMLHelper.h"

#include <cstring>

XMLHelper::XMLHelper() :
mDocument(0)
{

}

XMLHelper::~XMLHelper()
{
	delete mDocument;
}


/**
 * Set the xml filename
 */
void XMLHelper::setFilename(const Ogre::String &name)
{
	mFileName = name;
}

/**
 * Open/Close the filename
 */
void XMLHelper::openXml(void)
{
	if(mDocument) return;
	mDocument = Common::Util::loadXmlDocument(mFileName.c_str());
	if(!mDocument){
		debugERROR("Error opening the file %s\n", mFileName.c_str());
		ASSERT(false);
	}
}

void XMLHelper::closeXml(void)
{
	delete mDocument;
	mDocument = 0;
}

/**
 * Find an element (with attribute name) in the first nested elements
 * (starting from rootElement->getFirstChildElement())
 * @return	0	On error, or the element if we find it
 */
const TiXmlElement *XMLHelper::findElement(const char *name, const char *attrName) const
{
	ASSERT(mDocument);
	ASSERT(name);
	ASSERT(attrName);

	// find it
	const TiXmlElement *first = mDocument->RootElement();
	ASSERT(first);
	first = first->FirstChildElement();
	const char *value = 0;
	while(first){
		value = first->Attribute(attrName);
		if(value && std::strcmp(name, value) == 0){
			return first;
		}
		first = first->NextSiblingElement();
	}

	return 0;
}

/**
 * Returns all the first childs element of the root element
 */
void XMLHelper::getFirstElements(std::vector<const TiXmlElement *> &elements)
{
	ASSERT(mDocument);

	elements.clear();

	// find it
	const TiXmlElement *first = mDocument->RootElement();
	ASSERT(first);
	first = first->FirstChildElement();
	while(first){
		elements.push_back(first);
		first = first->NextSiblingElement();
	}
}

/**
 * Returns the root element of the xml
 */
const TiXmlElement *XMLHelper::getRootElement(void)
{
	if(!mDocument) return 0;
	return mDocument->RootElement();
}

////////////////////////////////////////////////////////////////////////////
////						Parse Static Functions						////
////////////////////////////////////////////////////////////////////////////

/**
 * Find a child element from a top level element
 * @param	root	The root element where we will find the child element
 * @param	name	The name of the element to find (value of the attr)
 * @param	atName	The attribute name ("name" as default)
 * @return	element	The founded element, or 0 if we cannot find it
 */
const TiXmlElement *XMLHelper::findChild(const TiXmlElement *root,
		const char *name,
		const char *atName)
{
	ASSERT(root);
	ASSERT(name);
	ASSERT(atName);

	// find it
	const TiXmlElement *first = root;
	ASSERT(first);
	first = first->FirstChildElement();
	while(first){
		const char *value = first->Attribute(atName);
		if(value && std::strcmp(name, value) == 0) {
			return first;
		}
		first = first->NextSiblingElement();
	}
	return 0;
}

