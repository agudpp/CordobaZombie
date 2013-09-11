/*
 * CollPreciseInfo.cpp
 *
 *  Created on: Aug 30, 2013
 *      Author: agustin
 */

#include "CollPreciseInfo.h"

#include <Box2D/Collision/Shapes/b2CircleShape.h>



// Helper methods
//
namespace {

// @brief Get the associated bounding box for a list of vertices
//
core::AABB
calculateBoundingBox(const core::Vector2* vertices, unsigned int count)
{
    ASSERT(count > 0);
    core::AABB result;
    result.tl = vertices[0];
    result.br = vertices[0];
    for (unsigned int i = 0; i < count; ++i) {
        const core::Vector2& c = vertices[i];
        result.increaseToContain(c);
    }
    return result;
}

}

namespace coll {

b2PolygonShape CollPreciseInfo::b2BoundingBox(true);
b2Transform CollPreciseInfo::identityTransf(b2Vec2(0,0), b2Rot(0));


CollPreciseInfo*
CollPreciseInfo::createPolygonPrecise(const core::Vector2* vertices, unsigned int count)
{
    ASSERT(vertices);
    ASSERT(count > 0);

    // get the bounding box and center point
    core::AABB bb = calculateBoundingBox(vertices, count);
    const core::Vector2 center = bb.center();

    // now we have to move each vertex relative to the center...
    const core::Vector2 translationVec = center * -1.f; // (0,0) - center

    ASSERT(count < 512); // if not, too fucking complex? yeah
    b2Vec2 tpoints[512]; // translated points

    for (unsigned int i = 0; i < count; ++i) {
        tpoints[i].x = vertices[i].x + translationVec.x; // move them to be relative to (0,0)
        tpoints[i].y = vertices[i].y + translationVec.y; // move them to be relative to (0,0)
    }

    // now we can create the b2Polygon shape
    b2PolygonShape* poly = new b2PolygonShape;
    poly->Set(tpoints, count);
    CollPreciseInfo* result = new CollPreciseInfo(poly);

    // set the transformation
    result->setPosition(center);

    return result;
}

CollPreciseInfo*
CollPreciseInfo::createCirclePrecise(float radius, const core::Vector2& position)
{
    if (radius <= 0.f) {
        debugERROR("Invalid radius: %f\n", radius);
        return 0;
    }

    b2CircleShape* shape = new b2CircleShape;
    shape->m_radius = radius;
    CollPreciseInfo* result = new CollPreciseInfo(shape);
    result->setPosition(position);

    return result;
}

void
CollPreciseInfo::getMaximumBB(core::AABB& mbb) const
{
    ASSERT(shape);
    b2AABB b2aabb;
    shape->ComputeAABB(&b2aabb, transform, 0);
    mbb.tl.x = b2aabb.lowerBound.x;
    mbb.br.x = b2aabb.upperBound.x;
    mbb.tl.y = b2aabb.upperBound.y;
    mbb.br.y = b2aabb.lowerBound.y;

    debugERROR("We are getting the current bounding box and not the maximum\n");
}

} /* namespace coll */
