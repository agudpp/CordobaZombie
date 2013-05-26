/*
 * GUIBuilder.h
 *
 *  Created on: 22/05/2012
 *      Author: agustin
 */

#ifndef GUIBUILDER_H_
#define GUIBUILDER_H_

#include <OgreString.h>

#include "GUIObject.h"
#include "tinyxml.h"
#include "CommonMath.h"
#include "StaticContainer.h"
#include "MenuButton.h"


class TiXmlElement;

class GUIBuilder
{
public:
	struct GUIObjectsInfo {
		GUIObject		*obj;
		Ogre::String	type;
	};
public:
	GUIBuilder();
	~GUIBuilder();

	/**
	 * Load all the GUIObjects from an xml.
	 * @return	true	on success
	 * 			false	otherwise
	 */
	bool load(const TiXmlElement *xml, std::vector<GUIObjectsInfo> &result);

private:
	/**
	 * Parse a GUIObject xml
	 */
	GUIObject *parseGUIObject(const TiXmlElement *xml, Ogre::String &type);
	void parseGeometry(const TiXmlElement *elem, math::Vector2 &tl, math::Vector2 &dim);

	/**
	 * Parse different kind of GUIObject
	 */
	StaticContainer *parseStaticCont(const TiXmlElement *xml);
	MenuButton *parseMenuButton(const TiXmlElement *xml);
};

#endif /* GUIBUILDER_H_ */
