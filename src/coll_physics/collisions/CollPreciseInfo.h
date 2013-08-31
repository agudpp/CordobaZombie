/*
 * CollPreciseInfo.h
 *
 *  Created on: Aug 30, 2013
 *      Author: agustin
 */

#ifndef COLLPRECISEINFO_H_
#define COLLPRECISEINFO_H_

#include <Box2D/Collision/Shapes/b2Shape.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Common/b2Math.h>

#include <math/Vec2.h>
#include <math/AABB.h>
#include <debug/DebugUtil.h>

namespace coll {

// Forward
//
class CollObject;
class CollisionHandler;


// @brief We will use this class to add more precise information but we don't
//        will let the suer to modify this directly since it could create problems.
//        This class will be handled by the CollObject and the Handler and we
//        will add some utils methods here to be more easy to use it.
//        This class will be the "bridge" between our system and the box2d system
//        so we will implement a lot of methods here.
//

struct CollPreciseInfo
{
private:
    friend class CollObject;
    friend class CollisionHandler;

    // the transformation matrix for this object
    b2Transform transform;
    // The box2D shape
    b2Shape* shape;


    CollPreciseInfo(b2Shape* s = 0) :
        shape(s)
    {
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
    // @param
    // TODO:

private:
    // avoid copying
    CollPreciseInfo(const CollPreciseInfo&);
    CollPreciseInfo& operator=(const CollPreciseInfo&);
};






////////////////////////////////////////////////////////////////////////////////
// inline stuff
//


inline void
CollPreciseInfo::setPosition(const core::Vector2& pos)
{
    transform.p.x = pos.x;
    transform.p.y = pos.y;
}
inline void
CollPreciseInfo::setAngle(float angle)
{
    transform.q.Set(angle);
}


////////////////////////////////////////////////////////////////////////////////
// Box2D methods

// @brief Check if this shapes overlaps with another (they are intersecting)
// @param other     The other shape we want to check against.
// @return true if they overlap, false otherwise
//
inline bool
CollPreciseInfo::checkOverlap(const CollPreciseInfo& other) const
{
    ASSERT(shape);
    return b2TestOverlap(shape, 0, other.shape, 0, transform, other.transform);
}

// @brief Check if this shape overlaps with a bounding box or not.
// @param bb        The bounding box we want to check against
// @return true if overlaps the bb | false otherwise
//
inline bool
CollPreciseInfo::checkOverlap(const core::AABB& bb) const
{
    ASSERT(shape);
    // TODO: probably this could be improved
    b2PolygonShape b2BoundingBox;
    b2Transform t;
    t.q.SetIdentity();
    b2BoundingBox.SetAsBox(bb.getWidth() * 0.5, bb.getHeight() * 0.5);
    const core::Vector2 center = bb.center();
    t.p.x = center.x;
    t.p.y = center.y;
    return b2TestOverlap(shape, 0, &b2BoundingBox, 0, transform, t);
}

} /* namespace coll */
#endif /* COLLPRECISEINFO_H_ */
