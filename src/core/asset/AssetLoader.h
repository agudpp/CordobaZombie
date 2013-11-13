/*
 * AssetLoader.h
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#ifndef ASSETLOADER_H_
#define ASSETLOADER_H_

#include <memory>

#include <tinyxml/tinyxml.h>


#include "Asset.h"

namespace core {

// @brief This class will be used to load an Asset from a .assetInfo file.
//        This class assume that you have already load all the assets before
//        calling any method.
//



class AssetLoader
{
public:
    AssetLoader();
    ~AssetLoader();

    // @brief Set the full path to the root resources directory
    // @param rootDir       The root resources directory
    //
    void
    setRootDirectory(const std::string& rootDir);

    // @brief Parse an asset from a ogre resource name and a group name.
    //        Note that for the case of the meshes we will get the associated
    //        .assetInfo file as next:
    //        If the resourceName = "zombie.mesh" we will look for the file
    //        "zombie.assetInfo" in the same directory than the zombie.mesh is.
    //
    // @param resourceName      The resource name we want to load
    // @param groupName         The Ogre resource group name where the resource
    //                          is. If this parameter is empty string we will
    //                          try to look it in all the groups
    // @param result            The resulting asset.
    // @return true on success | false otherwise
    //
    bool
    loadAssetFromOgreResource(const std::string& resourceName,
                              const std::string& groupName,
                              Asset& result);

private:
    // @brief Parse a .assetInfo file and fill the asset structure
    // @param asset     The asset structure to be filled
    // @return true on success | false otherwise
    //
    bool
    parseAssetFile(Asset& asset) const;


private:
    TiXmlDocument mXMLDoc;
};

} /* namespace core */
#endif /* ASSETLOADER_H_ */
