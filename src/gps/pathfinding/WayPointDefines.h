/*
 * WayPointDefines.h
 *
 *  Created on: Aug 24, 2013
 *      Author: agustin
 */

#ifndef WAYPOINTDEFINES_H_
#define WAYPOINTDEFINES_H_

#include <limits>

#include <types/basics.h>
#include <math/Vec2.h>


namespace gps {

typedef core::Vector2 Vertex;
typedef core::uint16_t index_t;

static const index_t INVALID_INDEX = std::numeric_limits<index_t>::max();


#define WPG_MAX_NUM_NODES       250

#define WPN_MAX_NUM_NEIGHBORS   5



// Useful classes
//

struct WayPointPath {
    // some useful defines
    //
    static const unsigned int MAX_PATH_SIZE = 25;

    enum Type {
        SAME_POINT = 0, // startPoint = endPoint
        IMPOSSIBLE,     // there is no possible path from start -> end
        NORMAL,         // normal path
    };

    core::uint32_t size;
    Vertex node[MAX_PATH_SIZE];
};

}


#endif /* WAYPOINTDEFINES_H_ */
