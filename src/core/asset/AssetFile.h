/*
 * AssetFile.h
 *
 *  Created on: Feb 4, 2014
 *      Author: agustin
 */

#ifndef ASSETFILE_H_
#define ASSETFILE_H_

#include <string>

namespace core {

// forward
//
class Asset;

// @brief This class will be used to handle and manipulate the .assetInfo file
//        or any other file we will use associated with the asset.
//        This class should let us save an asset to a file and load an asset from
//        a file.
//

class AssetFile
{
public:

    // @brief Save the asset to a file
    // @param asset         The asset to be saved
    // @param filename      The asset filename with the extension included
    // @return true on success | false otherwise
    //
    static bool
    assetToFile(const Asset& asset, const std::string& filename);

    // @brief Load an asset from a assetInfo file.
    // @param filename      The filename to be loaded
    // @param asset         The asset to be filled with that information
    // @return true on success | false otherwise
    //
    static bool
    assetFromFile(const std::string& filename, core::Asset& asset);


};

} /* namespace core */
#endif /* ASSETFILE_H_ */
