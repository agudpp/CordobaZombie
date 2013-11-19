/*
 * AssetLoader.cpp
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#include "AssetLoader.h"

#include <OgreString.h>
#include <OgreResourceGroupManager.h>

#include <debug/DebugUtil.h>
#include <os_utils/OSHelper.h>
#include <ResourceHandler.h>

#define CHECK_XML_ATTR(xml, varAttrName, errRetVal) \
    const char* varAttrName = xml->Attribute(#varAttrName);\
    if(varAttrName == 0) {\
        debugERROR("Error parsing the attribute " #varAttrName "\n");\
        return errRetVal;\
    }\


namespace core {

////////////////////////////////////////////////////////////////////////////////
bool
AssetLoader::parseAssetFile(Asset& asset) const
{
    ASSERT(mXMLDoc.RootElement() != 0);

    // now parse the asset file:

//    <Asset  name="zombie"
//            meshName="zombie.mesh"
//            type="ASSET_STATIC_WORLD_ELEMENT"
//            coll2DRepFile="zombie.2DColl"
//            coll3DRepFile="zombie.3DColl"
//            />
//
//    </Asset>

    const TiXmlElement* root = mXMLDoc.RootElement();

    // check it is a correct file
    if (!root || std::strcmp(root->Value(), "Asset") != 0) {
        debugERROR("Invalid xml element: %s, expected %s\n", root->Value(), "Asset");
        return false;
    }

    // get and check for all the attributes
    CHECK_XML_ATTR(root, name, false);
    CHECK_XML_ATTR(root, meshName, false);
    CHECK_XML_ATTR(root, type, false);
    CHECK_XML_ATTR(root, coll2DRepFile, false);
    CHECK_XML_ATTR(root, coll3DRepFile, false);

    // now set the values to the asset.
    asset.coll2DRepFile = coll2DRepFile;
    asset.coll3DRepFile = coll3DRepFile;
    asset.meshName = meshName;
    asset.name = name;
    assetStringTypeToEnumType(type, asset.type);

    return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
AssetLoader::AssetLoader()
{

}

////////////////////////////////////////////////////////////////////////////////
AssetLoader::~AssetLoader()
{

}

////////////////////////////////////////////////////////////////////////////////
void
AssetLoader::setRootDirectory(const std::string& rootDir)
{
    debugERROR("TODO\n");
}

////////////////////////////////////////////////////////////////////////////////
bool
AssetLoader::loadAssetFromOgreResource(const std::string& resourceName,
                                       const std::string& groupName,
                                       Asset& result)
{
    // try to get the resource file path
    Ogre::String filePath;

    if (!rrh::ResourceHandler::getResourcePathSomeGroup(resourceName, filePath)){
        debugERROR("Error getting the resource %s\n", resourceName.c_str());
        return false;
    }

    // we could get the file path, put it in the result asset
    // but first transform the path of the asset into the correct name
    std::string name, ext, path, fullFileName;

    if (!OSHelper::extractFileName(filePath, fullFileName) ||
        !OSHelper::extractPath(filePath, path) ||
        !OSHelper::extractFileExtension(fullFileName, name, ext)) {
        debugERROR("Error extracting the name of the file: %s\n", filePath.c_str());
        return false;
    }
    // use the file path and name to build the real asset file info
    OSHelper::addEndPathVar(path);
    result.fullAssetPath = path + name + ".assetInfo";

    // read it and parse the file
    mXMLDoc.Clear();
    mXMLDoc.ClearError();

    // load the file
    if (!mXMLDoc.LoadFile(result.fullAssetPath.c_str())) {
        debugERROR("Error parsing the file %s\n", result.fullAssetPath.c_str());
        return false;
    }

    // now parse it and return the value
    return parseAssetFile(result);
}

} /* namespace core */
