/*
 * CollisionObject.cpp
 *
 *  Created on: 19/04/2012
 *      Author: agustin
 *
 */

#include "DebugUtil.h"
#include "IntersectDetect.h"

#include "CollisionObject.h"


namespace c_p {

/**
 * Get the intersection point from an AABB and the object
 */
void CollisionObject::getIntPoints(const core::AABB &aabb,
		std::vector<core::Vector2> &result) const
{
	ASSERT(false);
}

/**
 * Get the intersection between this object and a line
 */
void CollisionObject::getIntPoints(const core::Vector2 &p1, const core::Vector2 &p2,
		std::vector<core::Vector2> &result) const
{
	// TODO: aca tenemos que agregar el caso que el objeto no sea un aabb

	// if the object is only an AABB
	// check all the lines
	result.clear();
	float xRes, yRes;
	if(core::IntersectDetect::lineIntersec(p1.x,p1.y,p2.x,p2.y,bb.tl.x, bb.tl.y,
			bb.br.x, bb.tl.y,xRes,yRes)){
		result.push_back(core::Point(xRes, yRes));
	}
	if(core::IntersectDetect::lineIntersec(p1.x,p1.y,p2.x,p2.y,bb.tl.x, bb.tl.y,
			bb.tl.x, bb.br.y,xRes,yRes)){
		result.push_back(core::Point(xRes, yRes));
	}
	if(core::IntersectDetect::lineIntersec(p1.x,p1.y,p2.x,p2.y,bb.tl.x, bb.br.y,
			bb.br.x, bb.br.y,xRes,yRes)){
		result.push_back(core::Point(xRes, yRes));
	}
	if(core::IntersectDetect::lineIntersec(p1.x,p1.y,p2.x,p2.y,bb.br.x, bb.tl.y,
			bb.br.x, bb.br.y,xRes,yRes)){
		result.push_back(core::Point(xRes, yRes));
	}

}

/**
 * Get the intersection between this object and other object
 */
void CollisionObject::getIntPoints(const CollisionObject *other,
		std::vector<core::Vector2> &result) const
{
	ASSERT(false);
}

}
