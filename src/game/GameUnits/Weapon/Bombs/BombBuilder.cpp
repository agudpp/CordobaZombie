/*
 * BombBuilder.cpp
 *
 *  Created on: 17/07/2012
 *      Author: agustin
 */

#include "DebugUtil.h"
#include "ProximityBomb.h"
#include "RemoteBomb.h"
#include "TimeBomb.h"
#include "BombBuilder.h"
#include "GlobalObjects.h"

Bomb *
BombBuilder::createProximityBomb(const TiXmlElement *elem)
{
    ProximityBomb *result = new ProximityBomb;
    return result;
}

Bomb *
BombBuilder::createTimeBomb(const TiXmlElement *elem)
{
    TimeBomb *result = new TimeBomb;
    return result;
}

Bomb *
BombBuilder::createRemoteBomb(const TiXmlElement *elem)
{
    RemoteBomb *result = new RemoteBomb;
    return result;
}

BombBuilder::BombBuilder()
{
}

BombBuilder::~BombBuilder()
{
}

/**
 * Configure the builder.
 * @param	cm		The collisionManager used
 * @param	uom		The UpdatableObjectManager
 */
void
BombBuilder::configure(c_p::CollisionManager *cm, UpdObjsManager *uom)
{
    ASSERT(cm);
    ASSERT(uom);
    Bomb::setCollisionManager(cm);
    Bomb::setUpdObjsManager(uom);
}

/**
 * Set the filename to use where there are all the weapons
 */
void
BombBuilder::setFilename(const Ogre::String &fname)
{
    mHelper.setFilename(fname);
}

/**
 * Create a bomb from and ID (name)
 * @param	ID		The name -ID- of the bomb to create
 */
Bomb *
BombBuilder::createBomb(const Ogre::String &ID)
{
    debugRED("TODO: Para cada una de las bombas que queremos construir estamos "
    "abriendo, parseando y cerrando el archivo xml... Capaz que no "
    "conviene.... Pero para evitar tener memoria ocupada al dope\n");

    // load the XML and get the bomb
    mHelper.openXml();
    // get the element by name
    const TiXmlElement *wxml = mHelper.getRootElement();
    if (!wxml) {
        debugERROR("Couldn't get the root element\n");
        return 0;
    }
    wxml = mHelper.findElement(ID.c_str(), "name");
    if (!wxml) {
        debugERROR("The bomb %s wasn't found\n", ID.c_str());
        return 0;
    }

    // parse the bomb
    ASSERT(wxml->Attribute("type"));
    ASSERT(wxml->Attribute("name"));
    ASSERT(wxml->Attribute("entity"));
    ASSERT(wxml->Attribute("visible"));
    ASSERT(wxml->Attribute("power"));
    ASSERT(wxml->Attribute("range"));

    Bomb *result = 0;
    Ogre::String type = wxml->Attribute("type");
    if (type == "BOMB_TIME") {
        result = createTimeBomb(wxml);
    } else if (type == "BOMB_REMOTE") {
        result = createRemoteBomb(wxml);
    } else if (type == "BOMB_PROXIMITY") {
        result = createProximityBomb(wxml);
    } else {
        debugERROR("No estamos soportando las bombas de tipo %s\n",
            type.c_str());
        ASSERT(false);
    }
    if (!result) {
        debugERROR("Error creating the bomb of type %s\n", type.c_str());
        return result;
    }

    // configure the common bomb attributes
    Ogre::String entName = wxml->Attribute("entity");
    Ogre::Entity *entity = GLOBAL_SCN_MNGR->createEntity(entName);
    ASSERT(entity);
    result->setEntity(entity);
    result->setPower(
        Ogre::StringConverter::parseReal(wxml->Attribute("power")));
    result->setRadius(
        Ogre::StringConverter::parseReal(wxml->Attribute("range")));

    Ogre::String visible = wxml->Attribute("visible");
    if (visible == "false") {
        result->hide();
    } else {
        result->show();
    }

    mHelper.closeXml();

    return result;
}
