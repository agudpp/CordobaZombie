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

BulletObject*
DynamicWorld::performClosestRay(const Ogre::Vector3& aFrom,
                                const Ogre::Vector3& aTo,
                                short int filterMask) const
{
    const btVector3 from = physics::BulletUtils::ogreToBullet(aFrom);
    const btVector3 to = physics::BulletUtils::ogreToBullet(aTo);

    btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
    rayCallback.m_collisionFilterMask = filterMask;

    mDynamicWorld.rayTest(from, to, rayCallback);
    if (!rayCallback.hasHit()) {
        return 0;
    }
    void* ptr = rayCallback.m_collisionObject->getUserPointer();
    if (ptr == 0) {
        return 0;
    }

    // this is dangerous!
    physics::BulletObject* bo = static_cast<physics::BulletObject*>(ptr);
    return bo;
}

} /* namespace physics */
