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
#include <ResourceHandler.h>

#include "AssetFile.h"


namespace core {



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

    // now parse it and return the value
    return AssetFile::assetFromFile(filePath, result);
}

} /* namespace core */
