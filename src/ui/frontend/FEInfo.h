/*
 * FEInfo.h
 *
 *  Created on: Oct 28, 2013
 *      Author: agustin
 */

#ifndef FEINFO_H_
#define FEINFO_H_

#include <math/Vec2.h>
#include <input/InputHelper.h>

namespace ui {

// We will define the event information here.
//
struct FEInfo {
    core::Vector2 mousePos;
    const input::InputHelper& input;

    FEInfo(const input::InputHelper& ih) : input(ih) {}
};

// define the UVCoords structure
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

#endif /* FEINFO_H_ */
