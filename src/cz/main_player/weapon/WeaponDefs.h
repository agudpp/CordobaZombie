/*
 * WeaponDefs.h
 *
 *  Created on: Oct 10, 2013
 *      Author: agustin
 */

#ifndef WEAPONDEFS_H_
#define WEAPONDEFS_H_

// forward
//
namespace Ogre {
class Camera;
}

namespace cz {

// Define the weapons IDs
//
enum WeaponID {
    WID_9MM = 0,
    WID_FAL,

    WID_COUNT,
    WID_INVALID,
};

// Define the special feature type that can handle the weapons
//
typedef int WFeature;

// Define the CrossFire type information
//
struct CrossFireInfo {
    Ogre::Camera* camera;
    float screenX;

    float screenY;
    CrossFireInfo() : camera(0) {}
};

}


#endif /* WEAPONDEFS_H_ */
