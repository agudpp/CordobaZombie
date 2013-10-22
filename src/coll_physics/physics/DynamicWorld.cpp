/*
 * DynamicWorld.cpp
 *
 *  Created on: Sep 19, 2013
 *      Author: agustin
 */

#include "DynamicWorld.h"


#include "BulletUtils.h"

namespace physics {

DynamicWorld::DynamicWorld(const btVector3& gravity) :
    mDispatcher(&mDefConf)
,   mDynamicWorld(&mDispatcher, &mBroadPhase, &mSeqSolver, &mDefConf)
{
    mDynamicWorld.setGravity(gravity);
}

DynamicWorld::~DynamicWorld()
{

}

bool
DynamicWorld::performClosestRay(const RaycastInfo& ri, RaycastResult& result) const
{
    btCollisionWorld::ClosestRayResultCallback rayCallback(ri.from, ri.to);
    rayCallback.m_collisionFilterMask = ri.filterMask;

    mDynamicWorld.rayTest(ri.from, ri.to, rayCallback);
    if (!rayCallback.hasHit()) {
        return false;
    }
    void* ptr = rayCallback.m_collisionObject->getUserPointer();
    if (ptr == 0) {
        return false;
    }

    result.worldNormal = rayCallback.m_hitNormalWorld;
    result.worldPosition = rayCallback.m_hitPointWorld;

    // this is dangerous!
    result.object = static_cast<physics::BulletObject*>(ptr);

    return true;
}

} /* namespace physics */
