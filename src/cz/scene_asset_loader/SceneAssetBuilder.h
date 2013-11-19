/*
 * SceneAssetBuilder.h
 *
 *  Created on: Nov 18, 2013
 *      Author: agustin
 */

#ifndef SCENEASSETBUILDER_H_
#define SCENEASSETBUILDER_H_

#include <OgreSceneNode.h>
#include <OgreEntity.h>

#include <asset/Asset.h>

// @brief Define the interface of the AssetBuilder objects {StaticAssetBuilder,
//        DynamicAssetBuilder}
//

namespace cz {

class SceneAssetBuilder
{
public:
    SceneAssetBuilder() {};
    virtual
    ~SceneAssetBuilder() {};

    // @brief Reset all the information you are currently using to start a new
    //        load.
    //        This method will be called before starting build the assets
    //
    virtual void
    reset(void) = 0;

    // @brief Check if this builder can process a particular asset
    // @param asset     The asset to be checked
    // @return true if it can process this asset (build) | false otherwise
    // @note if this instance can process the asset then the method process
    //       will be called
    //
    virtual bool
    canProcess(const core::Asset& asset) const = 0;

    // @brief Build a new asset using the asset information and the associated
    //        scene node and entity to be loaded.
    // @param asset     The asset information
    // @param node      The associated scene node
    // @param entity    The associated entity
    // @return true on success | false otherwise
    //
    virtual bool
    build(const core::Asset& asset, Ogre::SceneNode* node, Ogre::Entity* entity) = 0;

    // @brief This method will be called after all the asset of the scene
    //        were parsed.
    //
    virtual void
    finish(void) = 0;
};

} /* namespace cz */
#endif /* SCENEASSETBUILDER_H_ */
