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

#include <frontend/FEInfo.h>  // UVCoords

namespace cz {

// Define the weapons IDs
// Weapons order in texture must match WeaponID enum!
//
enum WeaponID {
    WID_9MM = 0,
    WID_FAL,

    WID_COUNT,
    WID_INVALID,
};

// Define weapons textures UVs, for all defined WeaponIDs
// Weapons order in texture must match WeaponID enum!
//
static const ui::UVCoord WeaponUV[WeaponID::WID_COUNT] = {
	ui::UVCoord(0.0, 0.5, 0.0, 1.0),  // WID_9MM
	ui::UVCoord(0.5, 1.0, 0.0, 1.0)   // WID_FAL
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
