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


#define WPN_MAX_NUM_NEIGHBORS   5

}


#endif /* WAYPOINTDEFINES_H_ */
