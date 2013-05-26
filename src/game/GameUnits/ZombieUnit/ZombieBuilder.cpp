/*
 * ZombieBuilder.cpp
 *
 *  Created on: 11/06/2012
 *      Author: agustin
 */


#include <OgreMaterialManager.h>
#include <OgreMaterial.h>

#include "DebugUtil.h"
#include "GlobalObjects.h"
#include "ZombieFSMBuilder.h"


#include "ZombieBuilder.h"

////////////////////////////////////////////////////////////////////////////////
ZombieUnit *ZombieBuilder::buildZombie(const TiXmlElement *elem)
{
	ASSERT(elem);

	ASSERT(Ogre::String(elem->Value()) == "Zombie");
	ASSERT(elem->Attribute("normalVelocity"));
	ASSERT(elem->Attribute("attackVelocity"));
	ASSERT(elem->Attribute("life"));
	ASSERT(elem->Attribute("attackPower"));
	ASSERT(elem->Attribute("entityName"));
	ASSERT(elem->Attribute("materialName"));
	ASSERT(elem->Attribute("type"));

	Ogre::String type = elem->Attribute("type");
	// build using the type
	ZombieUnit *result = 0;
	if(type == "NORMAL"){
		result = buildNormalZombie(elem);
		ASSERT(result);
	} else {
		ASSERT(false);
	}

	// get the common attributes
	result->setLife(live_t(Ogre::StringConverter::parseInt(
			elem->Attribute("life"))));
	result->setAttackPower(Ogre::StringConverter::parseReal(
			elem->Attribute("attackPower")));
	result->setWalkVelocity(Ogre::StringConverter::parseReal(
			elem->Attribute("normalVelocity")));
	result->setMaxVelocity(Ogre::StringConverter::parseReal(
			elem->Attribute("attackVelocity")));

	Ogre::String entityName = elem->Attribute("entityName");
	Ogre::String matName	= elem->Attribute("materialName");

	// check if already exists
	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity(entityName);
#ifdef DEBUG
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(matName);
	ASSERT(!mat.isNull());
#endif
	ent->setMaterialName(matName);
	result->setEntity(ent);

	Ogre::SceneNode *node =
			GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	result->setSceneNode(node);

	debugERROR("TODO: FALTA setear los flags de los zombies en los entity para las "
			"colisiones, cosa que cuando hagamos el raycast de entities obtengamos "
			" el zombie y podamos castearlo (igual para cualquier otro objeto"
			" seleccionable.\n");

	// build the zombie
	result->build();
}

////////////////////////////////////////////////////////////////////////////////
ZombieUnit *ZombieBuilder::buildNormalZombie(const TiXmlElement *elem)
{
	ASSERT(elem);
	ZombieUnit *result = new ZombieUnit;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ZombieBuilder::ZombieBuilder()
{

}

////////////////////////////////////////////////////////////////////////////////
ZombieBuilder::~ZombieBuilder()
{

}

////////////////////////////////////////////////////////////////////////////////
void ZombieBuilder::setFilename(const Ogre::String &fname)
{
	mHelper.setFilename(fname);
}

////////////////////////////////////////////////////////////////////////////////
void ZombieBuilder::configure(CollisionManager *cm, billboard::BillboardBatery *bbb,
		ZombieQueue *q)
{
	if(bbb)	ZombieUnit::setBillboardBBlood(bbb);
	if(cm && !GameObject::collMng) GameObject::collMng = cm;
	if(!ZombieUnit::getSMTransitionTable()){
		// create a transition table
		ZombieSMTTable *tt = ZombieFSMBuilder::build();
		ASSERT(tt);
		ZombieUnit::setSMTransitionTable(tt);
	}
	if(q)	ZombieUnit::setQueue(q);
}


////////////////////////////////////////////////////////////////////////////////
ZombieUnit *ZombieBuilder::createZombie(const Ogre::String &id)
{
	debugOPTIMIZATION("TODO: Para cada uno de los zombies que queremos construir estamos "
				"abriendo, parseando y cerrando el archivo xml... Capaz que no "
				"conviene.... Pero para evitar tener memoria ocupada al dope\n");

	// load the XML and get the zombie xml
	mHelper.openXml();
	// get the element by name
	const TiXmlElement *wxml = mHelper.getRootElement();
	if(!wxml){
		debugERROR("Couldn't get the root element\n");
		return 0;
	}
	wxml = mHelper.findElement(id.c_str(),"name");
	if(!wxml){
		debugERROR("The zombie %s wasn't found\n", id.c_str());
		return 0;
	}

	// parse the weapon
	ZombieUnit *result = buildZombie(wxml);

	mHelper.closeXml();

	return result;
}
