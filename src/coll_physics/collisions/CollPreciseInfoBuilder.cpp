/*
 * CollPreciseInfoBuilder.cpp
 *
 *  Created on: Aug 31, 2013
 *      Author: agustin
 */

#include "CollPreciseInfoBuilder.h"

#include <limits>
#ifdef DEBUG
#include <vector>
#endif

#include <debug/DebugUtil.h>

#include "CollObject.h"

namespace coll {

////////////////////////////////////////////////////////////////////////////////
core::AABB
CollPreciseInfoBuilder::calculateBB(const core::Vector2* vertices,
                                    unsigned int count) const
{
    ASSERT(vertices);
    ASSERT(count > 0);

    core::AABB result;
    result.tl = vertices[0];
    result.br = vertices[0];
    for (unsigned int i = 1; i < count; ++i) {
        result.increaseToContain(vertices[i]);
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////
void
CollPreciseInfoBuilder::calculateDistances(const core::Vector2* vertices,
                                           unsigned int count,
                                           const core::Vector2& point,
                                           float* distances,
                                           float& max,
                                           float& min) const
{
    ASSERT(vertices);
    ASSERT(distances);
    ASSERT(count > 0);

    max = min = point.squaredDistance(vertices[0]);
    distances[0] = point.squaredDistance(vertices[0]);

    for (unsigned int i = 1; i < count; ++i) {
        const float distance = point.squaredDistance(vertices[i]);
        distances[i] = distance;
        if (distance > max) {
            max = distance;
        }
        if (distance < min) {
            min = distance;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void
CollPreciseInfoBuilder::getMinDistancesBetweenPoints(const core::Vector2* vertices,
                                                     unsigned int count,
                                                     float& max,
                                                     float& min) const
{
    ASSERT(vertices);
    ASSERT(count > 1);
    min = vertices[0].squaredDistance(vertices[1]);
    max = -1.f;
    for (unsigned int i = 0; i < count; ++i) {
        float localMin = std::numeric_limits<float>::max();
        for (unsigned int j = i+1; j < count; ++j) {
            const float distance = vertices[i].squaredDistance(vertices[j]);
            if (distance < min) {
                min = distance;
            }
            if (distance < localMin) {
                localMin = distance;
            }
        }
        if (localMin != std::numeric_limits<float>::max() &&
            localMin > max) {
            max = localMin;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
CollPreciseInfoBuilder::isCircle(const core::Vector2* vertices,
                                 unsigned int count,
                                 float sqredErr)
{
    // check for
    // a circle. We will get all the distances from each vertex and the center
    // of the BB, this will give us the "distribution" of the points from the
    // center of the object, and if all of these distances are some kind equal,
    // then we can represent the object as if it were a circle...
    //
    const core::Vector2 center = mBB.center();

    ASSERT(count < 512 && "We are trying to build a VERY complex object!!!\n");
    float distances[512]; // should be enough
    float maxDist, minDist;

    calculateDistances(vertices, count, center, distances, maxDist, minDist);
    ASSERT(maxDist >= minDist);
    mRadius = std::sqrt(maxDist);  // the radius is the maximum distance

    // we now check for the error
    float maxError = maxDist - minDist;
    if (maxError <= sqredErr) {
        // now we have to check if the distance between different points is almost
        // the same... if not, we cannot ensure that this is a circle...
        getMinDistancesBetweenPoints(vertices, count, maxDist, minDist);
        maxError = maxDist - minDist;
        if (maxError <= sqredErr) {
            // we can consider this object as if it were a circle
            mType = Type::PIT_CIRCLE;
            return true;
        }
    }

    // this is not a circle
    return false;
}

////////////////////////////////////////////////////////////////////////////////
bool
CollPreciseInfoBuilder::isAABB(const core::Vector2* vertices,
                               unsigned int count,
                               float sqredErr)
{
    // We will get the polygon area and the AABB area as well.
    // We then will get the the % of incorrect area:
    // factor = (AABB area - polygon area) / AABB area.
    // So we will calculate the area:
    // To do this, we need to ensure that the vertices are in CCW order!!!
    // http://www.wikihow.com/Calculate-the-Area-of-a-Polygon
    //

    float xMult = 0, yMult = 0;
    for (unsigned int i = 0, size = count - 1; i < size; ++i) {
        xMult += vertices[i].x * vertices[i+1].y;
        yMult += vertices[i].y * vertices[i+1].x;
    }

    xMult += vertices[count-1].x * vertices[0].y;
    yMult += vertices[count-1].y * vertices[0].x;

    const float polyArea = (xMult - yMult) * 0.5f;
    const float bbArea = mBB.getHeight() * mBB.getWidth();
    ASSERT(bbArea > 0);
    ASSERT(polyArea > 0);
    const float factor = (bbArea - polyArea) / bbArea;

    if ((factor*factor) <= sqredErr) {
        // we can ensure that the object will be well fitted into an AABB.
        mType = Type::PIT_SIMPLE_AABB;
        return true;
    }

    // this is not an AABB
    return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CollPreciseInfoBuilder::CollPreciseInfoBuilder(CollisionHandler* ch) :
    mVertices(0)
,   mCount(0)
,   mCollHandler(ch)
{

}
////////////////////////////////////////////////////////////////////////////////
CollPreciseInfoBuilder::~CollPreciseInfoBuilder()
{

}

////////////////////////////////////////////////////////////////////////////////
void
CollPreciseInfoBuilder::setInfo(const core::Vector2* vertices,
                                unsigned int count,
                                float errFactor)
{
    ASSERT(errFactor >= 0);

    if (vertices == 0 || count < 0) {
        debugERROR("No vertices where given\n");
        return;
    }

    mVertices = vertices;
    mCount = count;

    // we will use the squared error factor...
    errFactor = errFactor * errFactor;

    // we will calculate the BB and the distances for the vertices
    mBB = calculateBB(vertices, count);

    // check if it is a AABB
    if (isAABB(vertices, count, errFactor)) {
        // yes it is.
        return;
    } else if (isCircle(vertices, count, errFactor)) {
        // yes it is
        return;
    }

    // else... it is a polygon :(
    mType = Type::PIT_POLYGON;
}

////////////////////////////////////////////////////////////////////////////////
CollPreciseInfo*
CollPreciseInfoBuilder::constructPreciseInfo(void) const
{
    if (mVertices == 0 || mCount == 0) {
        debugERROR("No vertices where set to build this object\n");
        return 0;
    }

    if (mType == Type::PIT_SIMPLE_AABB) {
        debugWARNING("Trying to construct a CollPreciseInfo unnecessarily\n");
        return 0;
    }

    // try to build the object then
    if (mType == Type::PIT_CIRCLE) {
        debugGREEN("Constructing a Precise Info circle object!\n");
        return CollPreciseInfo::createCirclePrecise(mRadius, mBB.center());
    } else if (mType == Type::PIT_POLYGON) {
        debugGREEN("Constructing a Precise Info polygon object!\n");
        return CollPreciseInfo::createPolygonPrecise(mVertices, mCount);
    }

}

////////////////////////////////////////////////////////////////////////////////
CollObject*
CollPreciseInfoBuilder::constructCollObject(void) const
{
    if (mVertices == 0 || mCount == 0) {
        debugERROR("No vertices where set to build this object\n");
        return 0;
    }

    if (mCollHandler == 0) {
        debugERROR("No collisionHandler set to construct objects!\n");
        return 0;
    }

    // check if we have to build only with and AABB or with precise info
    const bool constructPrecise = mType != Type::PIT_SIMPLE_AABB;
    CollPreciseInfo* pi = 0;
    if (constructPrecise) {
        pi = constructPreciseInfo();
        if (pi == 0) {
            return 0;
        }
    } else {
        debugGREEN("Constructing a simple AABB object!\n");
    }

    CollObject* result = mCollHandler->getNewCollObject(~0, mBB, 0, pi);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
core::Primitive*
CollPreciseInfoBuilder::buildDebugInfo(void)
{
    if (mVertices == 0 || mCount == 0) {
        debugERROR("No vertices set to build anything\n");
        return 0;
    }

    // Transform into ogre vertices
    std::vector<Ogre::Vector3> ogreVec;
    ogreVec.resize(mCount+1);
    for (unsigned int i = 0; i < mCount; ++i) {
        ogreVec[i] = Ogre::Vector3(mVertices[i].x, mVertices[i].y, 0);
    }
    ogreVec[mCount] = Ogre::Vector3(mVertices[0].x, mVertices[0].y, 0);

    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
    return pd.createMultiline(ogreVec, pd.getFreshColour());
}
#endif

} /* namespace coll */
