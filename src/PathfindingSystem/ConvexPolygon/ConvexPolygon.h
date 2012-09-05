/*
 * ConvexPolygon.h
 *
 *  Created on: 19/01/2012
 *      Author: agustin
 */

#ifndef CONVEXPOLYGON_H_
#define CONVEXPOLYGON_H_

#include "ConvexPolyStructs.h"

class ConvexPolygon
{
public:
	ConvexPolygon();
	virtual ~ConvexPolygon() = 0;

	/* Check if a point is inside the poly */
	virtual bool isPointInside(const Point &p) = 0;

	/* Check if the poly intersects with the rectangle */
	virtual bool boxIntersects(const Rectangle &r) = 0;
};

#endif /* CONVEXPOLYGON_H_ */
