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



/**
 * Get the intersection point from an AABB and the object
 */
void CollisionObject::getIntPoints(const sm::AABB &aabb,
		std::vector<sm::Vector2> &result) const
{
	ASSERT(false);
}

/**
 * Get the intersection between this object and a line
 */
void CollisionObject::getIntPoints(const sm::Vector2 &p1, const sm::Vector2 &p2,
		std::vector<sm::Vector2> &result) const
{
	// TODO: aca tenemos que agregar el caso que el objeto no sea un aabb

	// if the object is only an AABB
	// check all the lines
	result.clear();
	float xRes, yRes;
	if(IntersectDetect::lineIntersec(p1.x,p1.y,p2.x,p2.y,bb.tl.x, bb.tl.y,
			bb.br.x, bb.tl.y,xRes,yRes)){
		result.push_back(sm::Point(xRes, yRes));
	}
	if(IntersectDetect::lineIntersec(p1.x,p1.y,p2.x,p2.y,bb.tl.x, bb.tl.y,
			bb.tl.x, bb.br.y,xRes,yRes)){
		result.push_back(sm::Point(xRes, yRes));
	}
	if(IntersectDetect::lineIntersec(p1.x,p1.y,p2.x,p2.y,bb.tl.x, bb.br.y,
			bb.br.x, bb.br.y,xRes,yRes)){
		result.push_back(sm::Point(xRes, yRes));
	}
	if(IntersectDetect::lineIntersec(p1.x,p1.y,p2.x,p2.y,bb.br.x, bb.tl.y,
			bb.br.x, bb.br.y,xRes,yRes)){
		result.push_back(sm::Point(xRes, yRes));
	}

}

/**
 * Get the intersection between this object and other object
 */
void CollisionObject::getIntPoints(const CollisionObject *other,
		std::vector<sm::Vector2> &result) const
{
	ASSERT(false);
}
