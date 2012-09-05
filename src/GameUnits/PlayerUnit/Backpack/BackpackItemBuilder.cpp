/*
 * BackpackItemBuilder.cpp
 *
 *  Created on: 23/07/2012
 *      Author: agustin
 *
 */
#include <OgreStringConverter.h>

#include "BackpackItem.h"
#include "BpItemAdditionalInfo.h"
#include "DebugUtil.h"

#include "WeaponBackpackItem.h"
#include "BombBackpackItem.h"

#include "BackpackItemBuilder.h"


////////////////////////////////////////////////////////////////////////////////
BackpackItem *BackpackItemBuilder::createWeaponBpItem(const TiXmlElement *elem)
{
	WeaponBackpackItem *result = new WeaponBackpackItem;
	// TODO: tenemos que meter algo mas aca?

	return result;
}

////////////////////////////////////////////////////////////////////////////////
BackpackItem *BackpackItemBuilder::createBombBpItem(const TiXmlElement *elem)
{
	BombBackpackItem *result = new BombBackpackItem;
	// TODO: tenemos que meter algo mas aca?
	debugERROR("TODO: falta que asociemos el BombItemAction aca dependiendo"
			" del tipo de bomba que estamos por construir, deberia haber una"
			" por cada tipo de bomba. (Esto es lo que deberia generar en el"
			"\"InputManager\" un nuevo objeto o algo que haga cambiar el mouse"
			" pointer y ademas encolar la accion asociada (por ejemplo, hacer"
			" que el player vaya a poner la bomba).\n");

	return result;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
BackpackItemBuilder::BackpackItemBuilder(const Ogre::String &fname)
{
	setFilename(fname);
}

////////////////////////////////////////////////////////////////////////////////
BackpackItemBuilder::~BackpackItemBuilder()
{

}

////////////////////////////////////////////////////////////////////////////////
void BackpackItemBuilder::setFilename(const Ogre::String &fname)
{
	mHelper.setFilename(fname);
}

////////////////////////////////////////////////////////////////////////////////
BackpackItem *BackpackItemBuilder::createBackpackItem(const Ogre::String &ID)
{
	debugOPTIMIZATION("TODO: Para cada uno de los BackpackItems que queremos "
			"construir estamos abriendo, parseando y cerrando el archivo "
			"xml... Capaz que no conviene.... Pero para evitar tener "
			"memoria ocupada al dope\n");

	// load the XML and get the zombie xml
	mHelper.openXml();
	// get the element by name
	const TiXmlElement *wxml = mHelper.getRootElement();
	if(!wxml){
		debugERROR("Couldn't get the root element\n");
		return 0;
	}
	wxml = mHelper.findElement(ID.c_str(),"name");
	if(!wxml){
		debugERROR("The BackpackItem %s wasn't found\n", ID.c_str());
		return 0;
	}

	// parse the common backpackItem fields
	ASSERT(wxml->Attribute("overlayName"));
	ASSERT(wxml->Attribute("section"));
	ASSERT(wxml->Attribute("detailsMatName"));

	Ogre::String section = wxml->Attribute("section");
	BackpackItem *result = 0;
	if(section == "SECTION_WEAPON"){
		result = createWeaponBpItem(wxml);
	} else if(section == "SECTION_BOMB"){
		result = createBombBpItem(wxml);
	} else {
		debugERROR("Section type not supported by now? %s\n", section.c_str());
		ASSERT(false);
		return 0;
	}

	if(!result){
		debugERROR("Error creating the Backpackitem %s\n", ID.c_str());
		return 0;
	}
	// set common attrs
	Ogre::String overlayName = wxml->Attribute("overlayName");
	Ogre::String detailsMatName = wxml->Attribute("detailsMatName");
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	Ogre::OverlayContainer *cont = static_cast<Ogre::OverlayContainer*>(
			om.getOverlayElement(overlayName));
	ASSERT(cont);
	result->configureAll(cont);
//	result->loadFromOverlay(overlayName);

	BpItemAdditionalInfo info;
	info.setMaterialName(detailsMatName);
	result->setAdditionalInfo(info);
	result->show();

	mHelper.closeXml();

	return result;
}
