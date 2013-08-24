/*
 * IntersectDetect.cpp
 *
 *  Created on: 12/04/2012
 *      Author: agustin
 *
 */


#include "CommonMath.h"
#include "IntersectDetect2.h"

namespace core {

/**
 * Check if a line intersects a AABB
 */
bool IntersectDetect::checkLineAABB(const core::Vector2 &p1, const core::Vector2 &p2,
		const core::AABB &aabb)
{
	core::Vector2 result[2];
	int npoint;
	npoint = IntersectDetect::getIntPointsAABBLine(p1, p2, aabb, result);
	return npoint != 0;
}

}
