/*
 * StaticAssetBuilder.cpp
 *
 *  Created on: Nov 18, 2013
 *      Author: agustin
 */

#include "StaticAssetBuilder.h"

#include <debug/DebugUtil.h>



namespace cz {

////////////////////////////////////////////////////////////////////////////////
StaticAssetBuilder::StaticAssetBuilder() :
    mShapeHolder(0)
,   mWorldObjects(0)
{
    debugOPTIMIZATION("We are using a hash_map here, we can optmize this probably\n");
}

////////////////////////////////////////////////////////////////////////////////
StaticAssetBuilder::~StaticAssetBuilder()
{

}


////////////////////////////////////////////////////////////////////////////////
void
StaticAssetBuilder::reset(void)
{
    ASSERT(mShapeHolder);
    ASSERT(mWorldObjects);
    mShapeMap.clear();
    if (!mShapeHolder->elements().empty()) {
        debugWARNING("The shape holder is not empty\n");
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
StaticAssetBuilder::canProcess(const core::Asset& asset) const
{
    // we will process the asset if the type is the static one
    return asset.type == core::AssetType::ASSET_STATIC_WORLD_ELEMENT;
}

////////////////////////////////////////////////////////////////////////////////
bool
StaticAssetBuilder::build(const core::Asset& asset,
                          Ogre::SceneNode* node,
                          Ogre::Entity* entity)
{
    ASSERT(node);
    ASSERT(entity);

    // we will build the static object here

    // - construct the new world object
    // - build the collision precise information if it have (2DColl)
    // - build the physics representation if it have (3DColl)
    // - configure the StaticWorldObject with that information and the node + entity
    // - take into account the node scale (print a warning if different from 1)
}

////////////////////////////////////////////////////////////////////////////////
void
StaticAssetBuilder::finish(void);

} /* namespace cz */
