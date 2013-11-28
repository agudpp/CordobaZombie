/*
 * XMLResourceParser.cpp
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#include "XMLResourceParser.h"

#include <xml/XMLHelper.h>
#include <debug/DebugUtil.h>
#include <ResourceGroup.h>
#include <ResourceHandler.h>
#include <os_utils/OSHelper.h>

namespace tool {

////////////////////////////////////////////////////////////////////////////////
bool
XMLResourceParser::parseLocations(const TiXmlElement* xml)
{
    if ((xml == 0) ||
        (std::strcmp(xml->Value(), "ResourceLocationsToLoad") != 0)) {
        debugERROR("Invalid locations name from the xml. We need %s and we have %s\n",
            "ResourceLocationsToLoad", xml->Value());
        return false;
    }

    // clear all the current locations
    mLocations.clear();

    // read all the locations
    const TiXmlElement* location = xml->FirstChildElement("Location");
    XMLResourceLocation loc;
    while (location != 0) {
        const char* fileAttr = location->Attribute("cfgFile");
        if (fileAttr == 0) {
            debugERROR("Attribute cfgFile not found for one of the locations\n");
            return false;
        }
        const char* locationAttr = location->Attribute("location");
        if (locationAttr == 0) {
            debugERROR("Attribute location not found for one of the locations\n");
            return false;
        }

        loc.filename = fileAttr;
        loc.location = locationAttr;

        mLocations.push_back(loc);

        location = location->NextSiblingElement("Location");
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
XMLResourceParser::setType(XMLResource& resource)
{
    resource.type = ResourceType::XMLRT_NONE;

    if (resource.resource.find(".mesh")) {
        resource.type = ResourceType::XMLRT_MESH;
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
XMLResourceParser::parseResources(const TiXmlElement* xml)
{
    if ((xml == 0) || (std::strcmp(xml->Value(), "Resources") != 0)) {
        debugERROR("Invalid resource name from the xml. We need %s and we have %s\n",
            "Resources", xml->Value());
        return false;
    }

    // void clear all the current resources
    mResources.clear();

    const TiXmlElement* rsc = xml->FirstChildElement("Resource");
    XMLResource resource;
    while (rsc != 0) {
        const char* nameAttr = rsc->Attribute("name");
        if (nameAttr == 0) {
            debugERROR("Attribute name not found for one of the resources\n");
            return false;
        }

        resource.resource = nameAttr;
        setType(resource);

        mResources.push_back(resource);

        rsc = rsc->NextSiblingElement("Resource");
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
XMLResourceParser::XMLResourceParser() :
    mSceneMngr(0)
{
}

////////////////////////////////////////////////////////////////////////////////
XMLResourceParser::~XMLResourceParser()
{
}
////////////////////////////////////////////////////////////////////////////////
bool
XMLResourceParser::parse(const std::string& xml)
{
    mXMLDoc.reset(core::XMLHelper::loadXmlDocument(xml.c_str()));
    if (mXMLDoc.get() == 0) {
        debugERROR("Error parsing the xml file %s\n", xml.c_str());
        return false;
    }

    // now check that we have the 2 sections we need
    const TiXmlElement* root = mXMLDoc->RootElement();
    if (std::strcmp(root->Value(), "XMLResources") != 0) {
        debugERROR("Invalid root element of the xml file %s, expected %s and is "
            "%s\n", xml.c_str(), "XMLResources", root->Value());
        return false;
    }

    // parse sections and resources
    return parseLocations(root->FirstChildElement("ResourceLocationsToLoad")) &&
        parseResources(root->FirstChildElement("Resources"));

}

////////////////////////////////////////////////////////////////////////////////
bool
XMLResourceParser::loadResourceLocation(void)
{
    rrh::ResourceHandler rh;

    // get the resource path
    char* envVar = 0;;
    if (!core::OSHelper::getEnvVar(RESOURCES_PATH, envVar)) {
        debugERROR("We couldn't find the environment variable %s\n",
            RESOURCES_PATH);
        return false;
    }

    // set the root path
    rh.setResourceRootPath(envVar);

    Ogre::String fullPath;
    std::string localFilePath;
    for (XMLResourceLocation& rl : mLocations) {
        rrh::ResourceGroup rg;

        localFilePath = rl.location;
        core::OSHelper::addEndPathVar(localFilePath);
        localFilePath += rl.filename;
        // build the full path for each location
        rh.getResourceFullPath(localFilePath, fullPath);
        rg.setOgreResourceFile(fullPath);

        // load that resource
        if (!rh.loadResourceGroup(rg)) {
            debugERROR("Error loading resource grop with path %s, filename %s, "
                "location: %s\n",
                rg.ogreResourceFile().c_str(),
                rl.filename.c_str(),
                rl.location.c_str());
            return false;
        }
        debugBLUE("Resource %s loaded correctly\n", rg.ogreResourceFile().c_str());
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
XMLResourceParser::buildResrouce(const XMLResource& resource,
                                 Ogre::Entity*& result) const
{
    ASSERT(mSceneMngr);

    if (resource.type != ResourceType::XMLRT_MESH) {
        debugERROR("This resource is not a mesh => not an entity!\n");
        result = 0;
        return false;
    }

    // try to load an entity
    result = mSceneMngr->createEntity(resource.resource);

    return true;
}

} /* namespace tool */
