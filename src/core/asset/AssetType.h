/*
 * AssetType.h
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#ifndef ASSETTYPE_H_
#define ASSETTYPE_H_

#include <string>

namespace core {

// Define the different types of asset supported
//
enum AssetType {
    ASSET_INVALID = 0,
    ASSET_STATIC_WORLD_ELEMENT,
    ASSET_STATIC_WORLD_FLOOR,
};


// @brief Method used to transform a string type into a type enum
// @param strType       The type to be transformed
// @param enumType      The resulting type (if is invalid then it will be ASSET_INVALID)
// @return true on success | false on error
static inline bool
assetStringTypeToEnumType(const std::string& strType,
                          AssetType& enumType)
{
    enumType = AssetType::ASSET_INVALID;
    if (strType == "ASSET_STATIC_WORLD_ELEMENT") {
        enumType = AssetType::ASSET_STATIC_WORLD_ELEMENT;
    } else if (strType == "ASSET_STATIC_WORLD_FLOOR") {
        enumType = AssetType::ASSET_STATIC_WORLD_FLOOR;
    }

    return enumType != AssetType::ASSET_INVALID;
}

}


#endif /* ASSETTYPE_H_ */
