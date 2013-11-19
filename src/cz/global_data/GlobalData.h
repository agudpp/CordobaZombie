/*
 * GlobalData.h
 *
 *  Created on: Sep 10, 2013
 *      Author: agustin
 */

#ifndef GLOBALDATA_H_
#define GLOBALDATA_H_

#include <string>


// Forward stuff
//
namespace coll {
class CollisionHandler;
}

namespace Ogre {
class SceneManager;
class Camera;
}

namespace cz {

struct GlobalData
{
    // Ogre stuff
    //

    // SceneManager
    static Ogre::SceneManager* sceneMngr;
    // Camera
    static Ogre::Camera* camera;

    // Own classes
    //

    // Collision handler used
    static coll::CollisionHandler* collHandler;
    // last frame elapsed time
    static float lastTimeFrame;

    ////////////////////////////////////////////////////////////////////////////
    // Helper methods
    //

    // @brief Get the root resources directory (in this case is the dropbox for
    //        now). This will be changed later in the final version and we will
    //        return the real resource path.
    // @param path      The resulting path where the resources are (with the already
    //                  ending bar added).
    // @return true on success | false otherwise
    //
    static bool
    getRootResourcesPath(std::string& path);

};

} /* namespace cz */
#endif /* GLOBALDATA_H_ */
