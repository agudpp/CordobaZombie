/*
 * GUIBuilder.cpp
 *
 *  Created on: 22/05/2012
 *      Author: agustin
 */

#include <OgreStringConverter.h>

#include "GUIBuilder.h"

////////////////////////////////////////////////////////////////////////////////
GUIObject *GUIBuilder::parseGUIObject(const TiXmlElement *xml, Ogre::String &type)
{
	ASSERT(xml);
	ASSERT(Ogre::String(xml->Value()) == "GUIObject");

	// parse geometry
	core::Vector2 tl, dims;
	parseGeometry(xml, tl, dims);

	ASSERT(xml->Attribute("material"));
	Ogre::String matName = xml->Attribute("material");

	ASSERT(xml->Attribute("layer"));
	int layer = Ogre::StringConverter::parseInt(xml->Attribute("layer"));

	// build depending of the type
	type = xml->Attribute("type");

	GUIObject *result = 0;
	if(type == "Button"){
		result = parseMenuButton(xml);
	} else if(type == "Static"){
		result = parseStaticCont(xml);
	} else {
		debugRED("Invalid type of GUI OBJECT!\n");
		ASSERT(false);
		return 0;
	}
	ASSERT(result);

	// configure the result
	result->setMaterial(matName);
	result->configureGeometry(Ogre::Vector2(tl.x, tl.y),
			Ogre::Vector2(dims.x,dims.y));

	debugBLUE("Set ZOrder ... or layer here..!!\n");

	return result;
}

////////////////////////////////////////////////////////////////////////////////
void GUIBuilder::parseGeometry(const TiXmlElement *elem, core::Vector2 &tl,
		core::Vector2 &dim)
{
	ASSERT(elem);
	const char *tlx = elem->Attribute("topLeftX");
	const char *tly = elem->Attribute("topLeftY");
	const char *dimx = elem->Attribute("dimX");
	const char *dimy = elem->Attribute("dimY");
	ASSERT(tlx);
	ASSERT(tly);
	ASSERT(dimx);
	ASSERT(dimy);

	dim.x = Ogre::StringConverter::parseReal(dimx);
	dim.y = Ogre::StringConverter::parseReal(dimy);
	tl.x = Ogre::StringConverter::parseReal(tlx);
	tl.y = Ogre::StringConverter::parseReal(tly);
}

////////////////////////////////////////////////////////////////////////////////
StaticContainer *GUIBuilder::parseStaticCont(const TiXmlElement *xml)
{
	StaticContainer *result = new StaticContainer;
	debug("Building Static GUIObject\n");
	return result;
}

////////////////////////////////////////////////////////////////////////////////
MenuButton *GUIBuilder::parseMenuButton(const TiXmlElement *xml)
{
	ASSERT(xml);
	ASSERT(xml->Attribute("atlasSize"));
	debug("Building MenuButton GUIObject\n");

	MenuButton *result = new MenuButton;

	result->configure(Ogre::StringConverter::parseInt(xml->Attribute("atlasSize")));
	return result;
}



GUIBuilder::GUIBuilder()
{

}

GUIBuilder::~GUIBuilder()
{

}



/**
	 * Load all the GUIObjects from an xml.
	 * @return	true	on success
	 * 			false	otherwise
	 */
bool GUIBuilder::load(const TiXmlElement *xml, std::vector<GUIObjectsInfo> &result)
{
	ASSERT(xml);
	ASSERT(Ogre::String(xml->Value()) == "GUIInfo");

	result.clear();

	// parse all the GUIObjects
	const TiXmlElement *pElement = xml->FirstChildElement("GUIObject");
	while(pElement)
	{
		GUIObjectsInfo oi;
		oi.obj = parseGUIObject(pElement, oi.type);
		if(!oi.obj){
			debugRED("Error parsing the GUIInfo... %s\n", pElement->ToText()->Value());
			ASSERT(false);
			return false;
		}

		result.push_back(oi);
		pElement = pElement->NextSiblingElement("GUIObject");
	}

	return true;
}

