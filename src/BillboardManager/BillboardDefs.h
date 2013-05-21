/*
 * BillboardDefs.h
 *
 *  Created on: Apr 1, 2013
 *      Author: agustin
 */

#ifndef BILLBOARDDEFS_H_
#define BILLBOARDDEFS_H_

#include <OgrePrerequisites.h>
#include <OgreString.h>


namespace billboard {

// Define the uv coords container
//
struct UVCoords {
    Ogre::Real u0;
    Ogre::Real u1;
    Ogre::Real v0;
    Ogre::Real v1;

    // helper functions
    //
    inline Ogre::Real width(void) const
    {
        return u1 - u0;
    }
    inline Ogre::Real height(void) const
    {
        return v1 - v0;
    }
};

// Battery IDs
enum BatteryID {
    BLOOD = 0,
    EXPLOSIONS,

    SIZE,
};

// Battery info
//
struct BatteryInfo {
    BatteryID id;
    Ogre::Real width;
    Ogre::Real height;
    size_t count;
    Ogre::String materialName;
    std::vector<UVCoords> coords;
};

// Atlas info helper struct
//
struct AtlasInfo {
    UVCoords uvs;
    Ogre::Real width;
    Ogre::Real height;
    size_t count;

    // internal usage, this field will be overwrited later
    unsigned int index;
};

}

#endif /* BILLBOARDDEFS_H_ */
