/*
 * WorldObjectBuilder.h
 *
 *  Created on: Feb 5, 2014
 *      Author: agustin
 */

#ifndef WORLDOBJECTBUILDER_H_
#define WORLDOBJECTBUILDER_H_

#include <OgreSceneManager.h>


// Forward
//
namespace core {
class Asset;
}
namespace w_o {
class WorldObject;
}

namespace w_o {

// @brief This class will be used to construct a WorldObject from different
//        sources (Asset, etc).
//


class WorldObjectBuilder
{
public:

    // @brief This method will construct a WorldObject from an asset. We will
    //        create and allocate a new physic / coll2D / graphic representation
    //        for each asset, we will not check if we already have a representation
    //        of the same type created. This could be used for debug or fast
    //        test but is NOT EFFICIENT!!!
    // @param scnMngr   The scene manager to be used.
    // @param asset     The asset we want to use to create the world object
    // @param wo        The world object to be filled with the different
    //                  representations. Note if we already have some representation
    //                  created, it will create a leak!
    // @return true on succes | false otherwise.
    // @note The memory for each representation should be freed by the caller.
    //
    static bool
    buildFromAsset(const core::Asset& asset,
                   Ogre::SceneManager* scnMngr,
                   WorldObject& wo);

    // @brief This method will be used to destroy a plain WorldObject, we will
    //        assume that all the elements inside the World object (each
    //        representation and everything else was allocated and we can
    //        freely deallocate that memory).
    //        Probably it will be used only for debug this method.
    // @param scnMngr   The scene manager to be used.
    // @param wo        The world object to be destroyed
    //
    static void
    destroyPlainWorldObject(Ogre::SceneManager* scnMngr, WorldObject& wo);
};

}

#endif /* WORLDOBJECTBUILDER_H_ */
