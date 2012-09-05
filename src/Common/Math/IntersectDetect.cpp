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
bool IntersectDetect::checkLineAABB(const sm::Point &p1, const sm::Point &p2,
		const sm::AABB &aabb)
{
	sm::Point result[2];
	int npoint;
	npoint = IntersectDetect::getIntPointsAABBLine(p1, p2, aabb, result);
	return npoint != 0;
}
