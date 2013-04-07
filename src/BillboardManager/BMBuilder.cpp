/*
 * BMBuilder.cpp
 *
 *  Created on: Apr 7, 2013
 *      Author: agustin
 */

#include "BMBuilder.h"

#include <cstring>

#include <OgreStringConverter.h>

#include <Common/tinyxml/tinyxml.h>
#include <Common/DebugUtil/DebugUtil.h>

#include "BillboardManager.h"
#include "BillboardDefs.h"

// Helper functions
//
#define CHECK_XML_ATTR(elem, attrName) \
    if(!elem.Attribute(attrName)){\
        debugWARNING("Attribute %s not exists in %s\n", attrName, elem.Value());\
        ASSERT(false);\
    }


namespace {
// TODO: use the XMLHelper methods once we have decoupled from Pathfinding
//  (Utils.h)
////////////////////////////////////////////////////////////////////////////////
inline void
parseUnsigned(const TiXmlElement &elem, const char *attrName, unsigned int &result)
{
    ASSERT(attrName);
#ifdef DEBUG
    CHECK_XML_ATTR(elem, attrName)
#endif
    result = Ogre::StringConverter::parseUnsignedInt(elem.Attribute(attrName));
}

inline void
parseFloat(const TiXmlElement &elem, const char *attrName, float &result)
{
    ASSERT(attrName);
#ifdef DEBUG
    CHECK_XML_ATTR(elem, attrName)
#endif
    result = Ogre::StringConverter::parseReal(elem.Attribute(attrName));
}

inline void
parseBool(const TiXmlElement &elem, const char *attrName, bool &result)
{
    ASSERT(attrName);
#ifdef DEBUG
    CHECK_XML_ATTR(elem, attrName)
#endif
    result = Ogre::StringConverter::parseBool(elem.Attribute(attrName));
}



// Parse a UVcoords struct from a xml node
//
inline void
parseUVs(const TiXmlElement &node, billboard::UVCoords &result)
{
    // info:
    //  <UVCoords u0="0" v0="0" u1="1" v1="1" />

    ASSERT(std::strcmp(node.Value(), "UVCoords") == 0);

    parseFloat(node, "u0", result.u0);
    parseFloat(node, "v0", result.v0);
    parseFloat(node, "u1", result.u1);
    parseFloat(node, "v1", result.v1);
}

// Parse an AtlasInfo struct from an xml node
//
inline void
parseAtlasInfo(const TiXmlElement &node, billboard::AtlasInfo &result)
{
    // info:
    // <AtlasInfo width="x" height="x" count="x">
    //  <UVCoords .../>
    // </AtlasInfo>

    ASSERT(std::strcmp(node.Value(), "AtlasInfo") == 0);
    parseFloat(node, "width", result.width);
    parseFloat(node, "height", result.height);
    parseUnsigned(node, "height", result.count);

    ASSERT(node.FirstChildElement("UVCoords"));
    parseUVs(*node.FirstChildElement("UVCoords"), result.uvs);
}

// Parse a BatteryInfo from an xml node
//
void
parseBatteryInfo(const TiXmlElement &node, billboard::BatteryInfo &result)
{
    ASSERT(false);
}

}


namespace billboard {

bool
BMBuilder::configure(const TiXmlElement *xml, BillboardManager &manager)
{
    ASSERT(xml);

    // check we are in the correct node
    if (std::strcmp(xml->Value(), "BillboardManagerInfo") != 0) {
        debugERROR("Error, the xml is wrong: current node: %s, expected: %s\n",
            xml->Value(), "BillboardManagerInfo");
        return false;
    }

    // info:
    // <BillboardManagerInfo materialName="nameOfMaterial">
    //      <AtlasInfoItems>
    //          <AtlasInfoItem id="xxx">
    //              <AtlasInfo> .... </AtlasInfo>
    //          </AtlasInfo>
    //          ....
    //          <AtlasInfoItem id="xxx">
    //              <AtlasInfo> .... </AtlasInfo>
    //          </AtlasInfo>
    //      </AtlasInfoItems>
    // </BillboardManagerInfo>
    // TODO: we probably want also the billboards batteries

    const char *matName = xml->Attribute("materialName");
    if (!matName) {
        debugERROR("No material name found!\n");
        return false;
    }

    // parse all the AtlasInfoItems
    const TiXmlElement *child = xml->FirstChildElement("AtlasInfoItems");
    if (!child) {
        debugERROR("No AtlasInfoItems found!... we have nothing to configure?\n");
        return false;
    }

    typedef std::map<unsigned int, AtlasInfo> AtlasMap;
    typedef std::pair<unsigned int, AtlasInfo> AtlasElement;
    AtlasMap atlasMap;

    child = child->FirstChildElement("AtlasInfoItem");

    while (child != 0) {
        unsigned int itemId;
        parseUnsigned(*child, "id", itemId);

        AtlasInfo atlasInfo;
        const TiXmlElement *atlasInfoNode = child->FirstChildElement("AtlasInfo");
        if (atlasInfoNode == 0) {
            debugERROR("AtlasInfoItem without AtlasInfo node?!\n");
            return false;
        }

        parseAtlasInfo(*atlasInfoNode, atlasInfo);

        // add the atlasInfo into the resulting map.. we have to ensure
        // that the element is not there
        AtlasMap::iterator it = atlasMap.find(itemId);
        if (it != atlasMap.end()) {
            debugERROR("The element %u already exists! The xml is bad formed\n",
                itemId);
            return false;
        }
        atlasMap.insert(AtlasElement(itemId, atlasInfo));

        child = child->NextSiblingElement("AtlasInfoItem");
    }

    // now we have to configure the manager
    manager.createAtlas(matName, atlasMap);

    return true;
}

} /* namespace billboard */
