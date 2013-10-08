/*
 * UVCoordsDefs.h
 *
 *  Created on: Oct 3, 2013
 *      Author: agustin
 */

#ifndef UVCOORDSDEFS_H_
#define UVCOORDSDEFS_H_

namespace cz {


struct UVCoord {
    float u0;       // X0
    float u1;       // X1
    float v0;       // Y0
    float v1;       // Y1

    UVCoord(float aU0, float aU1, float aV0, float aV1) :
        u0(aU0), u1(aU1), v0(aV0), v1(aV1)
    {}
    UVCoord() {}
};


}


#endif /* UVCOORDSDEFS_H_ */
