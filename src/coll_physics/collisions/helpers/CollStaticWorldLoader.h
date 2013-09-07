/*
 * CollStaticWorldLoader.h
 *
 *  Created on: Sep 6, 2013
 *      Author: agustin
 */

#ifndef COLLSTATICWORLDLOADER_H_
#define COLLSTATICWORLDLOADER_H_

#include <OgreSceneManager.h>

#include <string>

#include <math/AABB.h>
#include <math/Vec2.h>
#include <types/StackVector.h>
#include <collisions/CollPreciseInfoBuilder.h>


namespace coll {

// forward
class CollisionHandler;


namespace CollStaticWorldLoader {

    // Helper class used to handle the level information
    //
    struct LevelInfo {
        static const unsigned int MAX_NUM_OBJS = 128;
        static const unsigned int MAX_NUM_VERTEX_PER_OBJECT = 32;
        typedef core::StackVector<core::Vector2,
                                  MAX_NUM_OBJS*MAX_NUM_VERTEX_PER_OBJECT> VerticesVec;
        typedef core::StackVector<std::pair<unsigned int, unsigned int>,
                                  MAX_NUM_OBJS> ObjectIndicesVec;

        core::AABB staticsBB;
        ObjectIndicesVec indices;
        VerticesVec vertices;

    };


    ////////////////////////////////////////////////////////////////////////////
    //                              Export methods                            //
    ////////////////////////////////////////////////////////////////////////////

    // @brief Convert a .scene file into a .sco file. This sco file will contain
    //        all the meshes in the scene (that should be static elements) and
    //        will calculate also the bounding box that contains all the static
    //        objects.
    // @param scene         The scene filename to load, we will use the
    //                      DotSceneLoader. So call this method only offline
    // @param scnMngr       The Ogre SceneManager to be used.
    // @param output        The output filename
    // @return true on succes | false otherwise
    //
    bool
    exportFromScene(const std::string& scene,
                    Ogre::SceneManager* scnMngr,
                    const std::string& output = "level.sco");

    ////////////////////////////////////////////////////////////////////////////
    //                              Import methods                            //
    ////////////////////////////////////////////////////////////////////////////

    // @brief This method will load all the information from a .sco file and
    //        will fill the information into the LevelInfo structure.
    // @param filename      The .sco filename
    // @param levelInfo     The level information loaded.
    // @return true on success | false otherwise
    //
    bool
    importFromFile(const std::string& filename,
                   LevelInfo& levelInfo);
}

} /* namespace coll */
#endif /* COLLSTATICWORLDLOADER_H_ */
