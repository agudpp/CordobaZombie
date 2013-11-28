/*
 * Asset.h
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#ifndef ASSET_H_
#define ASSET_H_

#include <string>

#include "AssetType.h"
#include "AssetMaterialType.h"

namespace core {

struct Asset
{
    // The type of the asset (3D static world object / 2D dynamic object / etc)
    AssetType type;
    // The asset material type if it has, or could be NONE by default
    AssetMaterialType materialType;
    // the name of the asset, this is only for pretty info
    std::string name;
    // this is the id used by the asset
    std::string meshName;
    // the full path to the asset file info (including the asset file)
    std::string fullAssetPath;
    // the collision 2D representation file (full path)
    std::string coll2DRepFile;
    // the collision 3D (physics) representation file (full path)
    std::string coll3DRepFile;

    Asset() :
        type(AssetType::ASSET_INVALID)
    ,   materialType(AssetMaterialType::ASSET_MAT_NONE)
    {}
};

} /* namespace core */
#endif /* ASSET_H_ */
