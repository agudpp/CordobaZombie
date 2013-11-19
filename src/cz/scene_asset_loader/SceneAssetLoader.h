/*
 * SceneAssetLoader.h
 *
 *  Created on: Nov 18, 2013
 *      Author: agustin
 */

#ifndef SCENEASSETLOADER_H_
#define SCENEASSETLOADER_H_

#include <OgreSceneManager.h>

#include <types/StackVector.h>
#include <debug/DebugUtil.h>

namespace cz {

// forward
//
class SceneAssetBuilder;

class SceneAssetLoader
{
public:

public:
    SceneAssetLoader();
    ~SceneAssetLoader();

    // @brief Add a SceneAssetBuilder to be used to build the assets
    // @param builder       The builder instance.
    // @note this class is NOT the owner of the memory, so you should free the
    //       builder instance later
    //
    inline void
    addBuilder(SceneAssetBuilder* builder);

    // @brief Load a scene file using the AssetLoader to get the AssetInformation
    //        of the elements of the scene.
    //        This will call the associated SceneAssetBuilder for each asset.
    // @param scenePath     The scene path info
    // @param rootRscPath   The root resources path to use.
    // @param sceneMngr     The scene manager to use (to built the nodes and entities)
    // @return true on success | false on error
    //
    bool
    loadScene(const std::string& scenePath,
              const std::string& rootRscPath,
              Ogre::SceneManager* sceneMngr);

private:
    // we will only be able to handle 4 builders, its enough for now, if not, will
    // crash :)
    core::StackVector<SceneAssetBuilder*, 4> mBuilders;
};





////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
SceneAssetLoader::addBuilder(SceneAssetBuilder* builder)
{
    ASSERT(builder);
    mBuilders.push_back(builder);
}

} /* namespace cz */
#endif /* SCENEASSETLOADER_H_ */
