/*
 * EngineConfiguration.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#include "EngineConfiguration.h"

#include <string.h>
#include <sstream>

#include <debug/DebugUtil.h>
#include <xml/XMLHelper.h>


// Helper stuff
namespace {

// @brief Return the attribute pointer if exists | 0 otherwise
//
const char*
getAttrPtr(const TiXmlElement* xml, const char* childXml, const char* attrName)
{
    if (xml == 0) {
        return 0;
    }
    const TiXmlElement* child = xml->FirstChildElement(childXml);
    if (child == 0) {
        return 0;
    }
    return child->Attribute(attrName);
}
}

namespace engine {

////////////////////////////////////////////////////////////////////////////////
EngineConfiguration::EngineConfiguration()
{

}
////////////////////////////////////////////////////////////////////////////////
EngineConfiguration::~EngineConfiguration()
{
}

////////////////////////////////////////////////////////////////////////////////
bool
EngineConfiguration::load(const std::string& path)
{
    mDoc.Clear();
    mDoc.ClearError();
    if (!mDoc.LoadFile(path.c_str())) {
        debugERROR("Error loading file %s\n", path.c_str());
        return false;
    }

    if (mDoc.RootElement() == 0 ||
        strcmp(mDoc.RootElement()->Value(), "EngineConfiguration") != 0){
        debugERROR("Invalid configuration file\n");
        return false;
    }

    // file loaded correctly
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
EngineConfiguration::getValue(const std::string& moduleName,
                              const std::string& key,
                              std::string& val) const
{
    // Get the root element
    const TiXmlElement* root = mDoc.RootElement();
    if (root == 0) {
        debugERROR("Error getting the root element of the document\n");
        return false;
    }
    const char* attr = getAttrPtr(root, moduleName.c_str(), key.c_str());
    if (attr == 0) {
        debugERROR("We couldn't find element %s in module %s\n",
                   key.c_str(), moduleName.c_str());
        return false;
    }

    val = attr;
    return true;
}
bool
EngineConfiguration::getValue(const std::string& moduleName,
                              const std::string& key,
                              unsigned int& val) const
{
    // Get the root element
    const TiXmlElement* root = mDoc.RootElement();
    if (root == 0) {
        debugERROR("Error getting the root element of the document\n");
        return false;
    }
    const char* attr = getAttrPtr(root, moduleName.c_str(), key.c_str());
    if (attr == 0) {
        debugERROR("We couldn't find element %s in module %s\n",
                   key.c_str(), moduleName.c_str());
        return false;
    }

    std::stringstream ss;
    ss << attr;
    ss >> val;
    return true;
}
bool
EngineConfiguration::getValue(const std::string& moduleName,
                              const std::string& key,
                              int& val) const
{
    // Get the root element
    const TiXmlElement* root = mDoc.RootElement();
    if (root == 0) {
        debugERROR("Error getting the root element of the document\n");
        return false;
    }
    const char* attr = getAttrPtr(root, moduleName.c_str(), key.c_str());
    if (attr == 0) {
        debugERROR("We couldn't find element %s in module %s\n",
                   key.c_str(), moduleName.c_str());
        return false;
    }

    std::stringstream ss;
    ss << attr;
    ss >> val;
    return true;
}
bool
EngineConfiguration::getValue(const std::string& moduleName,
                              const std::string& key,
                              float& val) const
{
    // Get the root element
    const TiXmlElement* root = mDoc.RootElement();
    if (root == 0) {
        debugERROR("Error getting the root element of the document\n");
        return false;
    }
    const char* attr = getAttrPtr(root, moduleName.c_str(), key.c_str());
    if (attr == 0) {
        debugERROR("We couldn't find element %s in module %s\n",
                   key.c_str(), moduleName.c_str());
        return false;
    }

    std::stringstream ss;
    ss << attr;
    ss >> val;
    return true;
}


} /* namespace engine */
