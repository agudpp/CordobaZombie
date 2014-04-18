/*
 * DynamicWorld.cpp
 *
 *  Created on: Sep 19, 2013
 *      Author: agustin
 */

#include "DynamicWorld.h"

#include <algorithm>


#include "BulletUtils.h"

namespace physics {

DynamicWorld::DynamicWorld(const Ogre::Vector3& gravity) :
    mDispatcher(&mDefConf)
,   mDynamicWorld(&mDispatcher, &mBroadPhase, &mSeqSolver, &mDefConf)
{
    setGravity(gravity);
}

DynamicWorld::~DynamicWorld()
{

}

bool
DynamicWorld::performClosestRay(const RaycastInfo& ri, RaycastResult& result) const
{
    btCollisionWorld::ClosestRayResultCallback rayCallback(ri.from, ri.to);
    rayCallback.m_collisionFilterMask = ri.filterMask;
    rayCallback.m_collisionFilterGroup = ri.filterGroup;

    mDynamicWorld.rayTest(ri.from, ri.to, rayCallback);
    if (!rayCallback.hasHit()) {
        return false;
    }

    result.worldNormal = rayCallback.m_hitNormalWorld;
    result.worldPosition = rayCallback.m_hitPointWorld;

    result.object = rayCallback.m_collisionObject;

    return true;
}

bool
DynamicWorld::performRaycast(const RaycastInfo& ri, RaycastMultiResult& result) const
{
    // clear the old data, just in case
    btCollisionWorld::AllHitsRayResultCallback& callback = result.btResult;

    // perform the raycast using the btResult in result, reset all its values
    result.clear();
    callback.m_rayFromWorld = ri.from;
    callback.m_rayToWorld = ri.to;
    callback.m_collisionFilterMask = ri.filterMask;
    callback.m_collisionFilterGroup = ri.filterGroup;

    mDynamicWorld.rayTest(ri.from, ri.to, callback);
    if (!callback.hasHit()) {
        return false;
    }

    // we hit some objects, so we need to sort it, only if there are more than one
    if (callback.m_collisionObjects.size() == 1) {
        // nothing to sort
        result.sortedIndices.push_back(0);
        return true;
    }

    // now sort the values using the distance
    struct PointDistance {
        float sqrDistance;
        unsigned int index;

        inline bool operator<(const PointDistance& o) const
        {return sqrDistance < o.sqrDistance;}
    };
    core::StackVector<PointDistance, RaycastMultiResult::MAX_ELEMENTS> aux;
    aux.resize(callback.m_collisionObjects.size());
    for (unsigned int i = 0; i < aux.size(); ++i) {
        PointDistance& pd = aux[i];
        pd.sqrDistance = callback.m_rayFromWorld.distance2(callback.m_hitPointWorld[i]);
        pd.index = i;
    }

    // sort the values now
    std::sort(aux.begin(), aux.end());

    // now fill the sorted indices vector of the result
    for (PointDistance& pd : aux) {
        result.sortedIndices.push_back(pd.index);
    }

    return true;
}

} /* namespace physics */
