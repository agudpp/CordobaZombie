/*
 * IntersectDetect.cpp
 *
 *  Created on: 12/04/2012
 *      Author: agustin
 *
 */


#include "CommonMath.h"
#include "IntersectDetect.h"



/**
 * Check if a line intersects a AABB
 */
bool IntersectDetect::checkLineAABB(const math::Point &p1, const math::Point &p2,
		const math::AABB &aabb)
{
	math::Point result[2];
	int npoint;
	npoint = IntersectDetect::getIntPointsAABBLine(p1, p2, aabb, result);
	return npoint != 0;
}
