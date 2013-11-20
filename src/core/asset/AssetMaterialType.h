/*
 * AssetMaterialType.h
 *
 *  Created on: Nov 19, 2013
 *      Author: agustin
 */

#ifndef ASSETMATERIALTYPE_H_
#define ASSETMATERIALTYPE_H_

namespace core {

// The different material types for the assets
//
enum AssetMaterialType {
    ASSET_MAT_NONE = 0,
    ASSET_MAT_METAL,
    ASSET_MAT_WOOD,
    ASSET_MAT_ROCK,
};

// @brief Method used to transform a string type into a material type enum
// @param strType       The type to be transformed
// @param enumType      The resulting type (if is invalid then it will be ASSET_MAT_NONE)
// @return true on success | false on error
static inline bool
assetStringTypeToMaterialEnumType(const std::string& strType,
                                  AssetMaterialType& enumType)
{
    enumType = AssetMaterialType::ASSET_MAT_NONE;
    if (strType == "ASSET_MAT_METAL") {
        enumType = AssetMaterialType::ASSET_MAT_METAL;
    } else if (strType == "ASSET_MAT_WOOD") {
        enumType = AssetMaterialType::ASSET_MAT_WOOD;
    } else if (strType == "ASSET_MAT_ROCK") {
        enumType = AssetMaterialType::ASSET_MAT_ROCK;
    }

    return enumType != AssetMaterialType::ASSET_MAT_NONE;
}

}


#endif /* ASSETMATERIALTYPE_H_ */
