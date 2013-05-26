/*
 * WeaponBuilder.cpp
 *
 *  Created on: 11/06/2012
 *      Author: agustin
 */

#include <OgreStringConverter.h>
#include <OgreMath.h>

#include "WeaponBuilder.h"
#include "GlobalObjects.h"
#include "DebugUtil.h"
// weapons
#include "ShortWeapon.h"
#include "BodyWeapon.h"
#include "LongWeapon.h"
#include "GrenadeWeapon.h"
#include "Gun9mm.h"
#include "Shotgun.h"
#include "WeaponIDs.h"


////////////////////////////////////////////////////////////////////////////////
void WeaponBuilder::configureShoots(int n, c_p::CollisionManager *cm,
		UpdObjsManager *uom)
{
	ASSERT(n > 0);

	Shoot::setCollisionManager(cm);
	Shoot::setUpdObjsManager(uom);
	mShoots.freeAll();

	for(int i = 0; i < n; ++i){
		Shoot *s = new Shoot;
		// we build it of a random size
		Ogre::Real rnd = Ogre::Math::RangeRandom(5,10);
		s->build(rnd);
	}

}

////////////////////////////////////////////////////////////////////////////////
Weapon *WeaponBuilder::buildWeapon(const TiXmlElement *elem)
{
	ASSERT(elem);
	static int counter = 0;

	const char *aux = elem->Value();
	if(Ogre::String(aux) != "Weapon"){
		debugRED("We receive a element that is not a weapon %s\n",elem->Value());
		return 0;
	}
	/*
	<Weapon name="9mm" entityName="9mm.mesh" type="W_SHORT" range="100.0"
	        power="30" effectName="nombreEffecto"/>
	*/
	ASSERT(elem->Attribute("entityName"));
	ASSERT(elem->Attribute("type"));
	ASSERT(elem->Attribute("range"));
	ASSERT(elem->Attribute("power"));
	ASSERT(elem->Attribute("effectName"));
	ASSERT(elem->Attribute("ammo"));

	// create the weapon
	Ogre::String type = elem->Attribute("type");
	Weapon *result = 0;
	if(type == "W_BODY"){
		result = buildBody(elem);
	} else if(type == "W_SHORT") {
		result = buildShort(elem);
	} else if(type == "W_LONG") {
		result = buildLong(elem);
	} else if(type == "W_GRANADE") {
		result = buildGrenade(elem);
	}
	ASSERT(result);

	// all the common things
	Ogre::Entity *ent = 0;
	ent = GLOBAL_SCN_MNGR->createEntity(elem->Attribute("entityName"));
	ASSERT(ent);
	result->setEntity(ent);

	result->setPower(Ogre::StringConverter::parseReal(elem->Attribute("power")));
	Ogre::Real range = Ogre::StringConverter::parseReal(elem->Attribute("range"));
	result->setSqrRange(range*range);	// the squared range

	// Ammunition
	Weapon::Ammunition ammo;
	ammo.ammo = Ogre::StringConverter::parseReal(elem->Attribute("ammo"));
	result->setAmmunition(ammo);

	// Animation velocity (by default is 1.0f)
	if(elem->Attribute("animVelocityFactor")){
		Ogre::Real velAnim = Ogre::StringConverter::parseReal(
				elem->Attribute("animVelocityFactor"));
		result->setAnimVelFactor(velAnim);
	}

	debugERROR("No estamos cargando effecto...\n");

	// set the ID of the object
	result->setID(getWeaponID(elem->Attribute("name")));

	return result;
}

////////////////////////////////////////////////////////////////////////////////
Weapon *WeaponBuilder::buildShort(const TiXmlElement *elem)
{
	ASSERT(elem);
	Ogre::String name = elem->Attribute("name");
	ASSERT(!name.empty());

	ShortWeapon *result = 0;
	if(name == "9mm"){
		result = new Gun9mm;
	} else {
		result = new ShortWeapon;
		// configure the magazine size
		ASSERT(elem->Attribute("magazine"));
		int m = Ogre::StringConverter::parseInt(elem->Attribute("magazine"));
		result->configAmmunition(m);
	}


	return result;
}

////////////////////////////////////////////////////////////////////////////////
Weapon *WeaponBuilder::buildGrenade(const TiXmlElement *elem)
{
	ASSERT(elem);
}


////////////////////////////////////////////////////////////////////////////////
Weapon *WeaponBuilder::buildLong(const TiXmlElement *elem)
{
	ASSERT(elem);

	LongWeapon *result = 0;

	Ogre::String name = elem->Attribute("name");
	ASSERT(!name.empty());

	if(name == "shotgun"){
		result = new Shotgun;
	} else {
		result = new LongWeapon;
		// configure the magazine size
		ASSERT(elem->Attribute("magazine"));
		int m = Ogre::StringConverter::parseInt(elem->Attribute("magazine"));
		result->configAmmunition(m);
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////
Weapon *WeaponBuilder::buildBody(const TiXmlElement *elem)
{
	ASSERT(elem);
}

////////////////////////////////////////////////////////////////////////////////
unsigned short WeaponBuilder::getWeaponID(const Ogre::String &name)
{
	if(name == "9mm"){
		return WP_ID_9MM;
	} else if (name == "fal"){
		return WP_ID_FAL;
	} else if (name == "frag"){
		return WP_ID_GRENADE_NORMAL;
	} else if (name == "sniper"){
		return WP_ID_SNIPER;
	} else if (name == "tonfa"){
		return WP_ID_TONFA;
	} else if (name == "machete"){
		return WP_ID_MACHETE;
	} else if (name == "mazo"){
		return WP_ID_MAZO;
	} else if (name == "shotgun"){
		return WP_ID_SHOTGUN;
	}
	debugERROR("Weapon not identified %s\n", name.c_str());
	ASSERT(false);
	return WP_ID_NONE;
}

////////////////////////////////////////////////////////////////////////////////
WeaponBuilder::WeaponBuilder()
{

}

////////////////////////////////////////////////////////////////////////////////
WeaponBuilder::~WeaponBuilder()
{

}

////////////////////////////////////////////////////////////////////////////////
void WeaponBuilder::setFilename(const Ogre::String &fname)
{
	mHelper.setFilename(fname);
}

////////////////////////////////////////////////////////////////////////////////
Weapon *WeaponBuilder::createWeapon(const Ogre::String &weaponName)
{

	debugRED("TODO: Para cada una de las armas que queremos construir estamos "
			"abriendo, parseando y cerrando el archivo xml... Capaz que no "
			"conviene.... Pero para evitar tener memoria ocupada al dope\n");

	// load the XML and get the weapon
	mHelper.openXml();
	// get the element by name
	const TiXmlElement *wxml = mHelper.getRootElement();
	if(!wxml){
		debugRED("Couldn't get the root element\n");
		return 0;
	}
	wxml = mHelper.findElement(weaponName.c_str(),"name");
	if(!wxml){
		debugRED("The weapon %s wasn't found\n", weaponName.c_str());
		return 0;
	}

	// parse the weapon
	Weapon *result = buildWeapon(wxml);

	mHelper.closeXml();

	return result;
}
