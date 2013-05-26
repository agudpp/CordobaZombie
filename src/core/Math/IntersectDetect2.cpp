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
bool IntersectDetect::checkLineAABB(const core::Point &p1, const core::Point &p2,
		const core::AABB &aabb)
{
	core::Point result[2];
	int npoint;
	npoint = IntersectDetect::getIntPointsAABBLine(p1, p2, aabb, result);
	return npoint != 0;
}

}
