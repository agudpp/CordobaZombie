/*
 * CollPreciseInfoBuilder.h
 *
 *  Created on: Aug 31, 2013
 *      Author: agustin
 */

#ifndef COLLPRECISEINFOBUILDER_H_
#define COLLPRECISEINFOBUILDER_H_

#include <cmath>

#include <math/Vec2.h>
#include <math/AABB.h>

#ifdef DEBUG
#include <debug/PrimitiveDrawer.h>
#endif

#include "CollPreciseInfo.h"
#include "CollisionHandler.h"




namespace coll {

// @brief This class will be in charge of building the ugly named PreciseInfo
//        classes for collision objects.
//        We will use some kind o heuristic to determine which is the best
//        type of PreciseInfo for a given list of points that conforms the object
//
//        We will also give some helper functions to check if the object could
//        be mapped into a BB without losing too much precision.
//


// Forward
//
class CollObject;

class CollPreciseInfoBuilder
{
public:

    // List all the possible types we consider possible for collision objects.
    //
    enum Type {
        PIT_SIMPLE_AABB = 0,            // this the best possible case
        PIT_CIRCLE,
        PIT_POLYGON,
    };


public:
    // Construct the builder with the current handler to be used to build the
    // collision objects
    //
    CollPreciseInfoBuilder(CollisionHandler* ch = 0);
    ~CollPreciseInfoBuilder();

    // @brief Set a list of vertices to analyze that conform the collision object.
    // @param vertices      The list of vertices representing the collision object
    //                      in counter clockwise order.
    // @param count         The number of vertices.
    // @param errFactor     This value will be used as the error acceptance factor.
    //                      For example if we want to know if the polygon is
    //                      very similar to an AABB, then we will calculate the
    //                      area of the polygon and get the difference between
    //                      the AABB area and the polygon area (this will give
    //                      us the area that is not equal to the polygon). If
    //                      the factor area:
    //                      (AABB area - polygon area) / total area) < errFactor
    //                      then we can assume that the polygon is an AABB.
    //                      In the case that we will check if it is a circle,
    //                      then we will use the errFactor as the max difference
    //                      value that can be between the different points and
    //                      the maximum distance between these points.
    // @note YOU MUST KEEP the vertices during the life of the builder, if not
    //       undefined behavior is ensure :p.
    //
    void
    setInfo(const core::Vector2* vertices,
            unsigned int count,
            float errFactor = 0.1f);

    // @brief Get which is the best type of box2D shape used for this or if we
    //        just can use a simple AABB or other thing.
    // @returns the best type we consider for the last collision object (info)
    //          set.
    //
    inline Type
    getBestType(void);

    // @brief Return the associated bounding box for the object analyzed.
    // @returns the associated BB
    //
    inline const core::AABB&
    getBoundingBox(void) const;

    // @brief Get the radius of the object (the radius of the object that
    //        holds all its points inside).
    // @returns the minimum radius holding all the vertices
    //
    inline float
    getRadius(void) const;

    // @brief Construct a PreciseInfo object using the current data. Note that
    //        if the current best type is an AABB this class will return 0.
    // @returns the PreciseInfo object using the best shape possible.
    //          If some error occurr 0 is returned.
    // @note The allocated memory should be free by the caller
    //
    CollPreciseInfo*
    constructPreciseInfo(void) const;

    // @brief Construct a collision object using the associated collision handler
    //        and return the correct object.
    //        This means if the object is not only an AABB we will construct its
    //        PreciseInfo and associate it to the resulting CollObject.
    // @returns the collObject associated to the CollisionHandler on success
    //          | 0 otherwise
    // @note that the mask will be set to ~0 and user define = 0
    // @note The allocated memory should be free by the caller
    //
    CollObject*
    constructCollObject(void) const;


    // DEBUG
#ifdef DEBUG
    // @brief Build the debug primitive information for the current data set
    // @return Primitive pointer or 0 on error.
    //
    core::Primitive*
    buildDebugInfo(void);
#endif


private:

    // @brief Calculate the bounding box for a list of vertices
    // @param vertices      The vertices
    // @param count         The number of vertices
    // @returns the associated bounding box that contains all the vertices
    //
    core::AABB
    calculateBB(const core::Vector2* vertices, unsigned int count) const;

    // @brief Calculate the squared distances for a list a vertices and a given point.
    //        It will also get the maximum distance and minimum squared distance.
    // @param vertices      The list of vertices
    // @param count         The number of vertices
    // @param point         The point to calculate all the distances.
    // @param distances     The resulting distances (size(distances) >= count).
    // @param max           The maximum distance calculated
    // @param min           The minimum distance calculated
    //
    void
    calculateDistances(const core::Vector2* vertices,
                       unsigned int count,
                       const core::Vector2& point,
                       float* distances,
                       float& max,
                       float& min) const;

    // @brief Calculate the distances between the different points in a list of
    //        points. We will calculate the minimum / max{min} distance between
    //        all the points.
    // @param vertices      The list of points
    // @param count         The number of points
    // @param max           The maximum min distance between all the points
    // @param min           The minimum distance between all the points
    //
    void
    getMinDistancesBetweenPoints(const core::Vector2* vertices,
                                 unsigned int count,
                                 float& max,
                                 float& min) const;


    // @brief Calculate the minimum distance between a point and a bounding box.
    // @param point         The point
    // @param bb            The bounding box
    // @returns the minimum distance between the point and the bb.
    //
    inline float
    getMinDistance(const core::Vector2& point, const core::AABB& bb) const;

    // @brief Calculate the minimum squared distance between a point and the closest
    //        vertex of the bounding box.
    // @param point         The point
    // @param bb            The bounding box
    // @returns the minimum squared distance between the point and the closest vertex of
    //          the bb.
    //
    inline float
    getMinDistanceToVertex(const core::Vector2& point, const core::AABB& bb) const;

    // @brief Check if the current information is a circle, and if it is, then
    //        set the information needed.
    // @param vertices      The vertices to analyze
    // @param count         The number of vertices
    // @param sqredErr      The squared error to be considered.
    // @return true if it could be considered a circle | false otherwise
    //
    bool
    isCircle(const core::Vector2* vertices, unsigned int count, float sqredErr);

    // @brief Check if the current information is a AABB, and if it is, then
    //        set the information needed.
    // @param vertices      The vertices to analyze
    // @param count         The number of vertices
    // @param sqredErr      The squared error to be considered.
    // @return true if it could be considered an AABB | false otherwise
    //
    bool
    isAABB(const core::Vector2* vertices, unsigned int count, float sqredErr);

private:
    core::AABB mBB;
    Type mType;
    float mRadius;
    const core::Vector2* mVertices;
    unsigned int mCount;
    CollisionHandler* mCollHandler;
};








////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline float
CollPreciseInfoBuilder::getMinDistance(const core::Vector2& point,
                                       const core::AABB& bb) const
{
    return std::min(std::min(std::abs(point.x - bb.tl.x),
                             std::abs(point.x - bb.br.x)),
                    std::min(std::abs(point.y - bb.tl.y),
                             std::abs(point.y - bb.br.y)));
}

inline float
CollPreciseInfoBuilder::getMinDistanceToVertex(const core::Vector2& point,
                                               const core::AABB& bb) const
{
    return std::min(std::min(point.squaredDistance(bb.tl),
                             point.squaredDistance(core::Vector2(bb.br.x, bb.tl.y))),
                    std::min(point.squaredDistance(bb.br),
                             point.squaredDistance(core::Vector2(bb.tl.x, bb.br.y))));
}

inline CollPreciseInfoBuilder::Type
CollPreciseInfoBuilder::getBestType(void)
{
    return mType;
}


inline const core::AABB&
CollPreciseInfoBuilder::getBoundingBox(void) const
{
    return mBB;
}


inline float
CollPreciseInfoBuilder::getRadius(void) const
{
    return mRadius;
}

} /* namespace coll */
#endif /* COLLPRECISEINFOBUILDER_H_ */
