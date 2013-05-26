/*
 * ConvexPolyStructs.h
 *
 *  Created on: 19/01/2012
 *      Author: agustin
 */

#ifndef CONVEXPOLYSTRUCTS_H_
#define CONVEXPOLYSTRUCTS_H_

namespace math {

// define a point used by the convex poly
struct Point {
	float	x;
	float 	y;
};


typedef Point	Vertex;

// Edge
struct Edge {
	Vertex	*v1;
	Vertex	*v2;
};

// Box2D
struct Rectangle {
	Vertex	*p1;
	Vertex	*p2;
	Vertex	*p3;
	Vertex	*p4;
};

}

#endif /* CONVEXPOLYSTRUCTS_H_ */
