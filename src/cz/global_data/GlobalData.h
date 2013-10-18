/*
 * GlobalData.h
 *
 *  Created on: Sep 10, 2013
 *      Author: agustin
 */

#ifndef GLOBALDATA_H_
#define GLOBALDATA_H_


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

};

} /* namespace cz */
#endif /* GLOBALDATA_H_ */
