/*
 * PolygonStructs.h
 *
 *  Created on: 20/01/2012
 *      Author: agustin
 *
 */

#ifndef POLYGONSTRUCTS_H_
#define POLYGONSTRUCTS_H_

#include "CommonMath.h"

namespace math {

typedef Point Vertex;

// Edge
struct Edge {
	math::Vertex *v1;
	Vertex *v2;
};

// Box2D
struct Rectangle {
	Vertex p1;
	Vertex p2;
	Vertex p3;
	Vertex p4;
};

}

#endif /* POLYGONSTRUCTS_H_ */
