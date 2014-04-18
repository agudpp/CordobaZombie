/*
 * CollPreciseInfo.h
 *
 *  Created on: Aug 30, 2013
 *      Author: agustin
 */

#ifndef COLLPRECISEINFO_H_
#define COLLPRECISEINFO_H_

#include <collisions/Box2D/Collision/Shapes/b2Shape.h>
#include <collisions/Box2D/Collision/Shapes/b2CircleShape.h>
#include <collisions/Box2D/Collision/Shapes/b2PolygonShape.h>
#include <collisions/Box2D/Collision/b2Distance.h>
#include <collisions/Box2D/Common/b2Math.h>


#include <math/Vec2.h>
#include <math/AABB.h>
#include <debug/DebugUtil.h>

#include "CollDefines.h"

namespace coll {

// Forward
//
class CollObject;
class CollisionHandler;
#ifdef DEBUG
class CollDebugDrawer;
#endif


// @brief We will use this class to add more precise information but we don't
//        will let the suer to modify this directly since it could create problems.
//        This class will be handled by the CollObject and the Handler and we
//        will add some utils methods here to be more easy to use it.
//        This class will be the "bridge" between our system and the box2d system
//        so we will implement a lot of methods here.
//

struct CollPreciseInfo
{
public:
    // @brief Here we will define the methods used to construct the CollPreciseInfo
    //        structures since we cannot construct this kind of objects in
    //        any way, we should respect some conditions:
    //        - We will move all the vertices to be in a relative position from
    //          the (0,0) point that will be the middle of the bounding box.
    //        - The bounding box should cover all the possible position of the
    //          element (in any rotation).
    //
    //        We will define a list of constructors to construct different types
    //        of shapes (b2shapes).
    //


    // @brief Construct a PolyShape given a list of points forming the polygon.
    //        The points should be in counter clockwise winding (reverse order).
    //        Also they should respect the b2PolygonShape constraints (read the
    //        b2Manual for that)
    // @param vertices  The vertices conforming the polygon (the last will be
    //                  linked with the first one).
    // @param count     The number of vertices conforming the polygon
    // @returns 0 on error, the CollPreciseInfo allocated object.
    // @note that we will move the shape to the center (0,0) and create a
    //       translation vector for this.
    //
    static CollPreciseInfo*
    createPolygonPrecise(const core::Vector2* vertices, unsigned int count);

    // @brief Construct a Circle shape for a given radius and will set its position
    //        where the parameter position indicates.
    // @param radius    The radius of the circle to be used.
    // @param position  The position of the circle.
    // @returns 0 on error | the CollPreciseInfo object on success.
    //
    static CollPreciseInfo*
    createCirclePrecise(float radius, const core::Vector2& position);

    // TODO: implement createChain, createEdge
    //


    // @brief Calculate the bounding box for the current shape PreciseInfo
    //        if it is valid.
    // @param aabb      The resulting bounding box for the current shape and
    //                  position
    //
    inline void
    getBoundingBox(core::AABB& aabb) const;

private:
    friend class CollObject;
    friend class CollisionHandler;
#ifdef DEBUG
    friend class CollDebugDrawer;
#endif

    // the transformation matrix for this object
    b2Transform transform;
    // The box2D shape
    b2Shape* shape;

    // some static values used in the other methods
    static b2PolygonShape b2BoundingBox; // construct the bb
    static b2Transform identityTransf;

    CollPreciseInfo(b2Shape* s) :
        shape(s)
    {
        ASSERT(s);
        transform.SetIdentity();
    }
    ~CollPreciseInfo()
    {
        delete shape;
    }

    // @brief Update the position and rotation of the b2shape. Note that the
    //        position of the shape (transform) and the shape itself should be
    //        sync to avoid problems. So do not modify the precise information
    //        if you don't know how
    // @param pos       The position of the object
    // @param angle     The angle in radians
    //
    inline void
    setPosition(const core::Vector2& pos);
    inline core::Vector2
    position(void);
    inline void
    setAngle(float angle);

    // @brief Calculate the maximum bounding box for the current b2shape. This
    //        bounding box will be centered in the (0,0) position so when we move
    //        the object (translate it) we will get the same collisions.
    // @param mbb       The maximum bunding box associated
    //
    void
    getMaximumBB(core::AABB& mbb) const;

    // @brief Check if this shapes overlaps with another (they are intersecting)
    // @param other     The other shape we want to check against.
    // @return true if they overlap, false otherwise
    //
    inline bool
    checkOverlap(const CollPreciseInfo& other) const;

    // @brief Check if this shape overlaps with a bounding box or not.
    // @param bb        The bounding box we want to check against
    // @return true if overlaps the bb | false otherwise
    //
    inline bool
    checkOverlap(const core::AABB& bb) const;

    // @brief Check if this shape collide with another one returning a list
    //        of points.
    // @param other     The other shape we want to check against
    // @param result    The resulting list of collision points
    // @return true if there are intersection points, false otherwise
    //
    inline bool
    collidePoints(const CollPreciseInfo& other, CollPointVec& result) const;
    inline bool
    collidePoints(const core::AABB& other, CollPointVec& result) const;


    // @brief Check if a point is inside the shape
    // @param point     The point to test
    // @return true if it is | false otherwise
    //
    inline bool
    testPoint(const core::Vector2& point) const;

    // @brief Get the closest point between a point (with a radius) and this
    //        shape.
    // @param center    The center of the circle
    // @param radius    The radius of the circle (not squared).
    // @param closestA  The closest point from this shape
    // @param closestB  The closest point from the circle
    // @param distance  The distance between both shapes
    //
    inline void
    closestPoint(const core::Vector2& center,
                 const float radius,
                 core::Vector2& closestA,
                 core::Vector2& closestB,
                 float& distance) const;


private:
    // avoid copying
    CollPreciseInfo(const CollPreciseInfo&);
    CollPreciseInfo& operator=(const CollPreciseInfo&);
};






////////////////////////////////////////////////////////////////////////////////
// inline stuff
//

inline void
CollPreciseInfo::getBoundingBox(core::AABB& aabb) const
{
    if (shape) {
        b2AABB bb;
        shape->ComputeAABB(&bb, transform, 0);
        aabb.tl.x = bb.lowerBound.x;
        aabb.tl.y = bb.upperBound.y;
        aabb.br.x = bb.upperBound.x;
        aabb.br.y = bb.lowerBound.y;
    }
}

inline void
CollPreciseInfo::setPosition(const core::Vector2& pos)
{
    transform.p.x = pos.x;
    transform.p.y = pos.y;
}
inline core::Vector2
CollPreciseInfo::position(void)
{
    return core::Vector2(transform.p.x, transform.p.y);
}
inline void
CollPreciseInfo::setAngle(float angle)
{
    transform.q.Set(angle);
}


////////////////////////////////////////////////////////////////////////////////
// Box2D methods

////////////////////////////////////////////////////////////////////////////////
inline bool
CollPreciseInfo::checkOverlap(const CollPreciseInfo& other) const
{
    ASSERT(shape);
    return b2TestOverlap(shape, 0, other.shape, 0, transform, other.transform);
}

////////////////////////////////////////////////////////////////////////////////
inline bool
CollPreciseInfo::checkOverlap(const core::AABB& bb) const
{
    ASSERT(shape);
    b2BoundingBox.updateBox(b2Vec2(bb.tl.x, bb.tl.y), b2Vec2(bb.br.x, bb.br.y));
    return b2TestOverlap(shape, 0, &b2BoundingBox, 0, transform, identityTransf);
}

////////////////////////////////////////////////////////////////////////////////
inline bool
CollPreciseInfo::collidePoints(const CollPreciseInfo& other, CollPointVec& result) const
{
    // TODO: we are not supporting other type right now other than polygon
    ASSERT(shape && shape->m_type == b2Shape::Type::e_polygon);
    ASSERT(other.shape && other.shape->m_type == b2Shape::Type::e_polygon);

    b2Manifold manifold;
    b2PolygonShape* poly1 = static_cast<b2PolygonShape*>(shape);
    b2PolygonShape* poly2 = static_cast<b2PolygonShape*>(other.shape);
    b2CollidePolygons(&manifold,
                      poly1, transform,
                      poly2, other.transform);

    result.clear();
    if (manifold.pointCount == 0) {
        // nothing to do
        return false;
    }
    // transform to global coordinate
    b2WorldManifold wm;
    wm.Initialize(&manifold, transform, poly1->m_radius, other.transform, poly2->m_radius);
    result.resize(manifold.pointCount);
    for (unsigned int i = 0; i < manifold.pointCount; ++i) {
        const b2Vec2& wp = wm.points[i];
        result[i].x = wp.x; result[i].y = wp.y;
    }
    return true;
}
inline bool
CollPreciseInfo::collidePoints(const core::AABB& bb, CollPointVec& result) const
{
    // TODO: we are not supporting other type right now other than polygon
    ASSERT(shape && shape->m_type == b2Shape::Type::e_polygon);

    b2BoundingBox.updateBox(b2Vec2(bb.tl.x, bb.tl.y), b2Vec2(bb.br.x, bb.br.y));

    b2Manifold manifold;
    b2PolygonShape* poly1 = static_cast<b2PolygonShape*>(shape);
    b2CollidePolygons(&manifold,
                      poly1, transform,
                      &b2BoundingBox, identityTransf);

    result.clear();
    if (manifold.pointCount == 0) {
        // nothing to do
        return false;
    }
    // transform to global coordinate
    b2WorldManifold wm;
    wm.Initialize(&manifold,
                  transform, poly1->m_radius,
                  identityTransf, b2BoundingBox.m_radius);
    result.resize(manifold.pointCount);
    for (unsigned int i = 0; i < manifold.pointCount; ++i) {
        const b2Vec2& wp = wm.points[i];
        result[i].x = wp.x; result[i].y = wp.y;
    }
    return true;
}

inline bool
CollPreciseInfo::testPoint(const core::Vector2& point) const
{
    ASSERT(shape);
    return shape->TestPoint(transform, b2Vec2(point.x, point.y));
}

inline void
CollPreciseInfo::closestPoint(const core::Vector2& center,
                              const float radius,
                              core::Vector2& closestA,
                              core::Vector2& closestB,
                              float& distance) const
{
    ASSERT(shape);
    b2CircleShape circle;
    circle.m_radius = radius;
    circle.m_p = b2Vec2(center.x, center.y);

    b2DistanceOutput output;
    b2DistanceInput input;
    b2SimplexCache cache;
    cache.count = 0;
    input.proxyA.Set(shape, 0);
    input.transformA = transform;
    input.proxyB.Set(&circle, 0);
    input.transformB = identityTransf;

    b2Distance(&output, &cache, &input);
    closestA.x = output.pointA.x;
    closestA.y = output.pointA.y;
    closestB.x = output.pointB.x;
    closestB.y = output.pointB.y;
    distance = output.distance;

}

} /* namespace coll */
#endif /* COLLPRECISEINFO_H_ */
