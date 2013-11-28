/*
 * RaycastInfo.h
 *
 *  Created on: Oct 21, 2013
 *      Author: agustin
 */

#ifndef RAYCASTINFO_H_
#define RAYCASTINFO_H_

#include <OgreRay.h>

#include <bullet/btBulletDynamicsCommon.h>

#include <types/StackVector.h>
#include <debug/DebugUtil.h>

#include "BulletUtils.h"


namespace physics {

class BulletObject;
class DynamicWorld;


// we will define the raycast information here
struct RaycastInfo {
    btVector3 from;
    btVector3 to;
    short int filterMask;
    short int filterGroup;

    RaycastInfo(const btVector3& aFrom = btVector3(0,0,0),
                const btVector3& aTo = btVector3(0,0,0),
                short int mask = btBroadphaseProxy::AllFilter,
                short int group = btBroadphaseProxy::DefaultFilter) :
        from(aFrom), to(aTo), filterMask(mask), filterGroup(group)
    {}
    RaycastInfo(const Ogre::Vector3& aFrom,
                const Ogre::Vector3& aTo,
                short int mask = btBroadphaseProxy::AllFilter,
                short int group = btBroadphaseProxy::DefaultFilter) :
        from(physics::BulletUtils::ogreToBullet(aFrom)),
        to(physics::BulletUtils::ogreToBullet(aTo)),
        filterMask(mask),
        filterGroup(group)
    {}
    RaycastInfo(const Ogre::Ray& ray,
                short int mask = btBroadphaseProxy::AllFilter,
                short int group = btBroadphaseProxy::DefaultFilter) :
        from(physics::BulletUtils::ogreToBullet(ray.getOrigin())),
        to(physics::BulletUtils::ogreToBullet(ray.getOrigin() +
            ray.getDirection() * 99999.f)),
        filterMask(mask),
        filterGroup(group)
    {}
};


// The result of the raycast structure
struct RaycastResult {
    const btCollisionObject* object;
    btVector3 worldNormal;      // hit normal vector
    btVector3 worldPosition;    // hit position
};

// The result for a raycast with multiple possible objects
//
struct RaycastMultiResult {
    static const unsigned int MAX_ELEMENTS = 15;

    inline RaycastMultiResult() : btResult(btVector3(0,0,0), btVector3(0,0,0)) {}
    inline ~RaycastMultiResult(){}

    // @brief Return the number of elements we had intersected
    //
    inline unsigned int
    size(void) const;

    // @brief Return the i-th Bullet object intersected (already sorted) where
    //        the first one (first = 0-th element) is the closest one).
    // @param i         The index position
    // @return the associated BulletObject | 0 on error
    //
    inline const btCollisionObject*
    btCollObject(unsigned int i);

    // @brief Return the world normal associated to the ith element
    // @param i         The index position (i should be < than size())
    // @return the world normal vector reference
    //
    inline const btVector3&
    worldNormal(unsigned int i) const;

    // @brief Return the hit position associated to the ith element
    // @param i         The index position (i should be < than size())
    // @return the hit world position vector reference
    //
    inline const btVector3&
    worldPosition(unsigned int i) const;

    // @brief Clear all the current information
    //
    inline void
    clear(void);

private:
    // private data
    friend class DynamicWorld;
    core::StackVector<unsigned short, MAX_ELEMENTS> sortedIndices;
    btCollisionWorld::AllHitsRayResultCallback btResult;

};














////////////////////////////////////////////////////////////////////////////////
// Inline implementation
//


inline unsigned int
RaycastMultiResult::size(void) const
{
    return sortedIndices.size();
}

inline const btCollisionObject*
RaycastMultiResult::btCollObject(unsigned int i)
{
    ASSERT(i < size());
    ASSERT(btResult.m_collisionObjects[sortedIndices[i]]);

    const btCollisionObject* btCollObj = btResult.m_collisionObjects[sortedIndices[i]];
    return btCollObj;

}

inline const btVector3&
RaycastMultiResult::worldNormal(unsigned int i) const
{
    ASSERT(i < size());
    return btResult.m_hitNormalWorld[sortedIndices[i]];
}

inline const btVector3&
RaycastMultiResult::worldPosition(unsigned int i) const
{
    ASSERT(i < size());
    return btResult.m_hitPointWorld[sortedIndices[i]];
}

inline void
RaycastMultiResult::clear(void)
{
    btResult.m_closestHitFraction = btScalar(1.);
    btResult.m_collisionObjects.resize(0);
    btResult.m_collisionObject = 0;
    btResult.m_flags = 0;
    sortedIndices.clear();
}

}



#endif /* RAYCASTINFO_H_ */
