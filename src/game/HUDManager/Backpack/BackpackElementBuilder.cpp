/*
 * BackpackElementBuilder.cpp
 *
 *  Created on: May 4, 2013
 *      Author: agustin
 */

#include "BackpackElementBuilder.h"

#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>

#include <debug/DebugUtil.h>
#include <tinyxml/tinyxml.h>
#include <XMLHelper/XMLHelper.h>
#include <GameUnits/PlayerUnit/Backpack/BackpackDefines.h>
#include <GameUnits/Weapon/Bombs/Bomb.h>
#include <GameUnits/Weapon/Weapon.h>
#include <GameUnits/Weapon/WeaponIDs.h>
#include <MenuSystem/GUI/Button/CallbackMenuButton.h>

#include "BackpackElement.h"

namespace hud {


////////////////////////////////////////////////////////////////////////////////
BackpackElementBuilder::GralElementID
BackpackElementBuilder::getIDFromString(const Ogre::String& name) const
{
    //          Weapons
    if (name == "9mm") {
        return GralElementID::Weapon_9mm;
    } else if (name == "Fal") {
        return GralElementID::Weapon_Fal;
    } else if (name == "Grenade") {
        return GralElementID::Weapon_Grenade;
    } else if (name == "Machete") {
        return GralElementID::Weapon_Machete;
    } else if (name == "Mazo") {
        return GralElementID::Weapon_Mazo;
    } else if (name == "Shotgun") {
        return GralElementID::Weapon_Shotgun;
    }
    //          Bombs
    else if (name == "BR") {
        return GralElementID::Bomb_Br;
    } else if (name == "BT") {
        return GralElementID::Bomb_Bt;
    } else if (name == "BP") {
        return GralElementID::Bomb_Bp;
    }
    //          Items
    // ..............
    else {
        // ERROR
        debugERROR("Unknown type of item: %s\n", name.c_str());
        ASSERT(false);
    }
}

BackpackElementBuilder::GralElementID
BackpackElementBuilder::getIDFromItem(const BackpackItem* item) const
{
    ASSERT(item);

    switch (item->type) {
    case BackpackDef::ItemType::ITEM:
        // TODO: nothing for now
        ASSERT(false); // TODO: implement the return type
        break;

    case BackpackDef::ItemType::BOMB:
    {
        Bomb* bomb = static_cast<Bomb*>(item->userDef);
        ASSERT(bomb);
        switch (bomb->getType()) {
        case Bomb::BOMB_TIME: return GralElementID::Bomb_Bt;
        case Bomb::BOMB_REMOTE: return GralElementID::Bomb_Br;
        case Bomb::BOMB_PROXIMITY: return GralElementID::Bomb_Bp;
        default:
            debugERROR("Unknown bomb type: %d\n", bomb->getType());
            ASSERT(false); // TODO: implement the return type
        }
    }
        break;

    case BackpackDef::ItemType::WEAPON:
    {
        Weapon* weapon = static_cast<Weapon*>(item->userDef);
        ASSERT(weapon);
        switch (weapon->getID()) {
        case WP_ID_9MM: return GralElementID::Weapon_9mm;
        case WP_ID_FAL: return GralElementID::Weapon_Fal;
        case WP_ID_GRENADE_NORMAL: return GralElementID::Weapon_Grenade;
        case WP_ID_MACHETE: return GralElementID::Weapon_Machete;
        case WP_ID_MAZO: return GralElementID::Weapon_Mazo;
        case WP_ID_SHOTGUN: return GralElementID::Weapon_Shotgun;
        case WP_ID_SNIPER:
        case WP_ID_TONFA:
        default:
            debugERROR("Unknown weapon type: %d\n", weapon->getID());
            ASSERT(false); // TODO: implement the return type
        }
    }
        break;

    default:
            debugERROR("Unknown type: %d\n", item->type);
            ASSERT(false); // TODO: implement the return type
            break;
    }

    ASSERT(false);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
bool
BackpackElementBuilder::parseElementXmlNode(ElementXmlNode& node,
                                            const TiXmlElement* xml) const
{
    ASSERT(xml);
    if (!xml->Attribute("overlayName")) {
        debugERROR("No overlayName attribute found\n");
        return false;
    }
    if (!xml->Attribute("name")) {
        debugERROR("No name attribute found\n");
        return false;
    }

    node.name = xml->Attribute("name");
    node.overlayName = xml->Attribute("overlayName");

    return true;
}

////////////////////////////////////////////////////////////////////////////////
BackpackElementPtr
BackpackElementBuilder::buildFromElementXmlNode(const ElementXmlNode& node,
                                                const BackpackItem* item) const
{
    ASSERT(node);
    ASSERT(item);

    // Get the container to be used in the button
    const Ogre::String overlayName(node.overlayName);
    Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
    Ogre::OverlayContainer *cont = static_cast<Ogre::OverlayContainer*>(
                    om.getOverlayElement(overlayName));

    // create the button and configure it with the container
    CallbackMenuButtonPtr button(new CallbackMenuButton());
    button->configureAll(cont);

    // create the BackpackElement to with all this info
    BackpackElementPtr result(new BackpackElement(item, button));
    return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
BackpackElementBuilder::BackpackElementBuilder()
{
}

////////////////////////////////////////////////////////////////////////////////
BackpackElementBuilder::~BackpackElementBuilder()
{
}

////////////////////////////////////////////////////////////////////////////////
bool
BackpackElementBuilder::load(const Ogre::String& xmlFile)
{
    // clear the old data
    clear();

    // read and open the xml
    core::XMLHelper xmlHelper;
    xmlHelper.setFilename(xmlFile);
    xmlHelper.openXml();

    // check if it is the correct xml
    const TiXmlElement* root = xmlHelper.getRootElement();
    if (root == 0) {
        debugERROR("No root element for file %s\n", xmlFile.c_str());
        return false;
    }

    if (root->Value() == 0 || root->Value() != "BackpackItems") {
        debugERROR("Invalid XML file, root element different than %s, it is %s",
            "BackpackItems", (root->Value() != 0) ? root->Value() : "null");
        return false;
    }

    // iterate over all the elements
    const TiXmlElement* elem = root->FirstChildElement("BackpackItem");
    while (elem != 0) {
        ElementXmlNode xmlNode;
        if (!parseElementXmlNode(xmlNode, elem)) {
            debugERROR("Ill formed xml!\n");
            return false;
        }
        const Ogre::String itemName(elem->Attribute("name"));
        GralElementID id = getIDFromString(itemName);

        // add the element to the map
        mElements[id] = xmlNode;

        elem = elem->NextSiblingElement("BackpackItem");
    }

    xmlHelper.closeXml();
}

////////////////////////////////////////////////////////////////////////////////
void
BackpackElementBuilder::clear(void) const
{
    mElements.clear();
}

////////////////////////////////////////////////////////////////////////////////
BackpackElementPtr
BackpackElementBuilder::buildElement(const BackpackItem* item, PlayerUnit* player)
{
    GralElementID id = getIDFromItem(item);
    ElementMap::iterator it = mElements.find(id);

    if (it == mElements.end()) {
        debugERROR("We cannot load that item: %d\n", id);
        ASSERT(false);
        return BackpackElementPtr();
    }

    // we have the element we want, create it
    BackpackElementPtr result = buildFromElementXmlNode(it->second, item);
    result->setPlayerUnit(player);

    return result;
}

} /* namespace hud */
