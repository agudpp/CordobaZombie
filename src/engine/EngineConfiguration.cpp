/*
 * EngineConfiguration.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#include <string.h>
#include <sstream>

#include <QMessageBox>

#include <xml/XMLHelper.h>
#include "EngineConfiguration.h"


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

// @brief Sets "attrValue" as value of the attribute "attrName"
// @returns true if the attribute exists and was written | false otherwise
//
bool
setAttrValue(TiXmlElement* xml, const char* childXml,
           const char* attrName, const char* attrValue)
{
    if (0 == xml)
        return false;
    TiXmlElement* child = xml->FirstChildElement(childXml);
    if (0 == child)
        return false;
    if (0 == child->Attribute(attrName))
        return false;
    child->SetAttribute(attrName, attrValue);
    return true;
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
        QMessageBox::critical(0, "Error", "Failed loading file " +
                              QString(path.c_str()));
        debugERROR("Error loading file %s\n", path.c_str());
        return false;
    }

    if (mDoc.RootElement() == 0 ||
        strcmp(mDoc.RootElement()->Value(), "EngineConfiguration") != 0) {
        QMessageBox::critical(0, "Error", QString(path.c_str()) +
                              "is an invalid configuration file");
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
        QMessageBox::critical(0, "Error", "Could not get the root element "
                              "of the document\n");
        return false;
    }
    const char* attr = getAttrPtr(root, moduleName.c_str(), key.c_str());
    if (attr == 0) {
        QMessageBox::critical(0, "Error", "Could not find element " +
                              QString(key.c_str()) + " in module " +
                              QString(moduleName.c_str()));
        return false;
    }

    val = attr;
    return true;
}


///////////////////////////////////////////////////////////////////////////////
bool
EngineConfiguration::getValue(const std::string& moduleName,
                              const std::string& key,
                              unsigned int& val) const
{
    // Get the root element
    const TiXmlElement* root = mDoc.RootElement();
    if (root == 0) {
        QMessageBox::critical(0, "Error", "Could not get the root element "
                              "of the document\n");
        return false;
    }
    const char* attr = getAttrPtr(root, moduleName.c_str(), key.c_str());
    if (attr == 0) {
        QMessageBox::critical(0, "Error", "Could not find element " +
                              QString(key.c_str()) + " in module " +
                              QString(moduleName.c_str()));
        return false;
    }

    std::stringstream ss;
    ss << attr;
    ss >> val;
    return true;
}


///////////////////////////////////////////////////////////////////////////////
bool
EngineConfiguration::getValue(const std::string& moduleName,
                              const std::string& key,
                              int& val) const
{
    // Get the root element
    const TiXmlElement* root = mDoc.RootElement();
    if (root == 0) {
        QMessageBox::critical(0, "Error", "Could not get the root element "
                              "of the document\n");
        return false;
    }
    const char* attr = getAttrPtr(root, moduleName.c_str(), key.c_str());
    if (attr == 0) {
        QMessageBox::critical(0, "Error", "Could not find element " +
                              QString(key.c_str()) + " in module " +
                              QString(moduleName.c_str()));
        return false;
    }

    std::stringstream ss;
    ss << attr;
    ss >> val;
    return true;
}


///////////////////////////////////////////////////////////////////////////////
bool
EngineConfiguration::getValue(const std::string& moduleName,
                              const std::string& key,
                              float& val) const
{
    // Get the root element
    const TiXmlElement* root = mDoc.RootElement();
    if (root == 0) {
        QMessageBox::critical(0, "Error", "Could not get the root element "
                              "of the document\n");
        return false;
    }
    const char* attr = getAttrPtr(root, moduleName.c_str(), key.c_str());
    if (attr == 0) {
        QMessageBox::critical(0, "Error", "Could not find element " +
                              QString(key.c_str()) + " in module " +
                              QString(moduleName.c_str()));
        return false;
    }

    std::stringstream ss;
    ss << attr;
    ss >> val;
    return true;
}


///////////////////////////////////////////////////////////////////////////////
bool
EngineConfiguration::setValue(const std::string& moduleName,
                              const std::string& key,
                              const std::string& val)
{
    // Get the root element
    TiXmlElement* root = mDoc.RootElement();
    if (root == 0) {
        QMessageBox::critical(0, "Error", "Could not get the root element "
                              "of the document\n");
        return false;
    }
    if(setAttrValue(root, moduleName.c_str(), key.c_str(), val.c_str())) {
        mDoc.SaveFile();
        return true;
    } else {
        QMessageBox::critical(0, "Error", "Could not find element " +
                              QString(key.c_str()) + " in module " +
                              QString(moduleName.c_str()));
        return false;
    }
}


///////////////////////////////////////////////////////////////////////////////
bool
EngineConfiguration::setValue(const std::string& moduleName,
                              const std::string& key,
                              const unsigned int& val)
{
    // NOTE: could be inlined in the header. However: http://goo.gl/YIZjCr
    std::stringstream ss;
    ss << val;
    return setValue(moduleName, key, ss.str());
}


///////////////////////////////////////////////////////////////////////////////
bool
EngineConfiguration::setValue(const std::string& moduleName,
                              const std::string& key,
                              const int& val)
{
    // NOTE: could be inlined in the header. However: http://goo.gl/YIZjCr
    std::stringstream ss;
    ss << val;
    return setValue(moduleName, key, ss.str());
}


///////////////////////////////////////////////////////////////////////////////
bool
EngineConfiguration::setValue(const std::string& moduleName,
                              const std::string& key,
                              const float& val)
{
    // NOTE: could be inlined in the header. However: http://goo.gl/YIZjCr
    std::stringstream ss;
    ss << val;
    return setValue(moduleName, key, ss.str());
}


} /* namespace engine */
