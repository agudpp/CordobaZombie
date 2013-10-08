/*
 * BillboardLoader.cpp
 *
 *  Created on: Oct 8, 2013
 *      Author: agustin
 */

#include "BillboardLoader.h"

#include <map>

#include <tinyxml/tinyxml.h>
#include <xml/XMLHelper.h>
#include <types/StackVector.h>

#include "BillboardGroupHandler.h"


namespace {


// @brief Parse a BillboardStack info from an xml.
// @param xml       The xml
// @param id        The id of the stack
// @param bsi       BillboardStackInfo
// @return true on succes | false otherwise
//
bool
parseBillboardStackInfo(const TiXmlElement* xml,
                        unsigned int& id,
                        cz::BillboardStack::Args& bsi)
{
    ASSERT(xml);
    if (std::string(xml->Value()) != "BillboardStack") {
        debugERROR("Invalid xml element to be parsed, expected: %s, we have: %s\n",
            "BillboardStack", xml->Value());
        return false;
    }
    if (xml->Attribute("id") == 0 ||
        xml->Attribute("numBillboards") == 0 ||
        xml->Attribute("width") == 0 ||
        xml->Attribute("height") == 0 ||
        xml->Attribute("matName") == 0) {
        debugERROR("Some missing attribute in BillboardStack\n");
        return false;
    }
    core::XMLHelper::parseUnsignedInt(xml, "id", id);
    core::XMLHelper::parseFloat(xml, "width", bsi.width);
    core::XMLHelper::parseFloat(xml, "height", bsi.height);
    core::XMLHelper::parseUnsignedInt(xml, "numBillboards", bsi.numBillboards);
    bsi.materialName = xml->Attribute("matName");

    debugWARNING("We are hardcoding some values here...\n");
    bsi.direction = Ogre::Vector3::UNIT_Z;
    bsi.upVector = Ogre::Vector3::UNIT_Y;

    return true;
}

// @brief Parse a Group (UVCoord + id) structure from an xml element
// @param xml       The xml
// @param stackID   The associated stack id
// @param uvs       The uvs coords
// @return true on success | false otherwise
//
bool
parseGroup(const TiXmlElement* xml, unsigned int& stackID, cz::UVCoord& uvs)
{
    ASSERT(xml);
    if (xml->Attribute("stackID") == 0 ||
        xml->Attribute("u0") == 0 ||
        xml->Attribute("u1") == 0 ||
        xml->Attribute("v0") == 0 ||
        xml->Attribute("v1") == 0) {
        debugERROR("Some missing attribute in UVCoord\n");
        return false;
    }
    core::XMLHelper::parseUnsignedInt(xml, "stackID", stackID);
    core::XMLHelper::parseFloat(xml, "u0", uvs.u0);
    core::XMLHelper::parseFloat(xml, "u1", uvs.u1);
    core::XMLHelper::parseFloat(xml, "v0", uvs.v0);
    core::XMLHelper::parseFloat(xml, "v1", uvs.v1);

    return true;
}

}

namespace cz {

namespace BillboardLoader {

//  <BillboardGroupInfo>
//      <BillboardStack id="1"
//                      numBillboards="10"
//                      width="10"
//                      height="10"
//                      matName="bloodBillboard1">
//      </BillboardStack>
//
//  </BillboardGroupInfo>

bool
loadAndConfigure(const TiXmlElement* xml, BillboardGroupHandler& handler)
{
    ASSERT(xml);
    if (std::string(xml->Value()) != "BillboardGroupInfo") {
        debugERROR("Invalid xml, expected BillboardGroupInfo and get %s\n",
            xml->Value());
        return false;
    }

    // clear all the old data
    handler.clear();

    debugOPTIMIZATION("Use core::List or core::Map instead of stl\n");
    std::map<int, BillboardStack*> stackMap;

    // parse each BillboardStack now and configure it
    const TiXmlElement* stack = xml->FirstChildElement("BillboardStack");
    bool error = false;
    while (stack != 0 && !error) {
        // first parse the stack
        BillboardStack::Args args;
        unsigned int id;
        if (!parseBillboardStackInfo(stack, id, args)) {
            error = true;
            break;
        }

        // check if the stack already exists
        if (stackMap.find(id) != stackMap.end()) {
            debugWARNING("We already has that BillboardStack ID: %d\n", id);
            continue;
        }

        // create the new stack
        BillboardStack* bs = new BillboardStack;
        stackMap[id] = bs;
        bs->configure(args);

        stack = stack->NextSiblingElement("BillboardStack");
    }

    if (error) {
        // remove all the memory allocated
        for (std::map<int, BillboardStack*>::iterator it = stackMap.begin(),
            end = stackMap.end(); it != end; ++it) {
            delete it->second;
        }
        return false;
    }

    // now parse the groups
    const TiXmlElement* group = xml->FirstChildElement("Group");

#ifdef DEBUG
    std::set<int> checker;
#endif
    while (group != 0 && !error) {
        BillboardGroupHandler::GroupInfo gi;
        unsigned int stackID;
        if (!parseGroup(group, stackID, gi.coords)) {
           error = true;
           break;
       }

        // we could parse it, check if we have that stack ID
        if (stackMap.find(stackID) == stackMap.end()) {
            error = true;
            debugERROR("Invalid StackID in some of the groups: %d\n", stackID);
            break;
        }

        // put the stack into the handler
        gi.stack = stackMap[stackID];
        gi.stack->setVisible(true);
        handler.addGroup(gi);

#ifdef DEBUG
        // debug
        checker.insert(stackID);
#endif

        group = group->NextSiblingElement("Group");
    }

    // check for errors
    if (error) {
        for (std::map<int, BillboardStack*>::iterator it = stackMap.begin(),
            end = stackMap.end(); it != end; ++it) {
            delete it->second;
        }
        return false;
    }

#ifdef DEBUG
    // check that all the stacks were used (avoid leaks)
    ASSERT(checker.size() == stackMap.size());
#endif

    // everything fine
    return true;
}

}

} /* namespace cz */
