/*
 * BackpackElementBuilder.h
 *
 *  Created on: May 4, 2013
 *      Author: agustin
 */

#ifndef BACKPACKELEMENTBUILDER_H_
#define BACKPACKELEMENTBUILDER_H_


#include <map>
#include <OgreString.h>

#include "BackpackElement.h"


// forward
//
class BackpackItem;
class PlayerUnit;
class TiXmlElement;


namespace hud {

class BackpackElementBuilder
{

    // Private enum used to list all the elements we can create.
    //
    enum GralElementID {
        NONE = 0,

        // Weapons types
        Weapon_9mm,
        Weapon_Fal,
        Weapon_Grenade,
        Weapon_Machete,
        Weapon_Mazo,
        Weapon_Shotgun,

        // Bomb types
        Bomb_Br,
        Bomb_Bt,
        Bomb_Bp,

        // Item types
    };

    // Private information representing each node of the xml file
    //
    struct ElementXmlNode {
        Ogre::String name;
        Ogre::String overlayName;
        // TODO: if there are more USEFUL information we need to put it here
    };

public:
    BackpackElementBuilder();
    ~BackpackElementBuilder();

    /**
     * @brief Load all the BackpackElementPtrs from an xml file and track the
     *        associated information (but not the elements itself)
     * @param xmlFile   The xmlfile to use (BackpackItems.xml)
     * @returns true on success || false otherwise
     * @note If we already loaded some backpack elements we will remove them first
     */
    bool
    load(const Ogre::String& xmlFile);

    /**
     * @brief Clear all the current elements
     */
    void
    clear(void) const;

    /**
     * @brief Build a BackpackElementPtr from a BackpackItemPtr.
     * @param item      The item used to construct the BackpackElement
     * @param player    The associated PlayerUnit to this element
     * @returns The BackpackElementPtr, this class will NOT handle the memory
     *          of the element
     */
    BackpackElementPtr
    buildElement(const BackpackItem* item, PlayerUnit* player);

private:

    // Get the GralElementID from a string name
    //
    GralElementID
    getIDFromString(const Ogre::String& name) const;

    // Get the GralElementID from a item
    //
    GralElementID
    getIDFromItem(const BackpackItem* item) const;

    // Parse an ElementXmlNode from a TiXmlElement node
    //
    bool
    parseElementXmlNode(ElementXmlNode& node, const TiXmlElement* xml) const;


    // Build a backpackElement from a ElementXmlNode
    //
    BackpackElementPtr
    buildFromElementXmlNode(const ElementXmlNode& node, const BackpackItem* item) const;

private:
    typedef std::map<GralElementID, ElementXmlNode> ElementMap;

    ElementMap mElements;
};

} /* namespace hud */
#endif /* BACKPACKELEMENTBUILDER_H_ */
