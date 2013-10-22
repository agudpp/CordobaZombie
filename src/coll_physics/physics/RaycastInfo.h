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

#include "BulletUtils.h"


namespace physics {

class BulletObject;


// we will define the raycast information here
struct RaycastInfo {
    btVector3 from;
    btVector3 to;
    short int filterMask;

    RaycastInfo(const btVector3& aFrom,
                const btVector3& aTo,
                short int mask = btBroadphaseProxy::AllFilter) :
        from(aFrom), to(aTo), filterMask()
    {}
    RaycastInfo(const Ogre::Vector3& aFrom,
                const Ogre::Vector3& aTo,
                short int mask = btBroadphaseProxy::AllFilter) :
        from(physics::BulletUtils::ogreToBullet(aFrom)),
        to(physics::BulletUtils::ogreToBullet(aTo)),
        filterMask(mask)
    {}
    RaycastInfo(const Ogre::Ray& ray, short int mask = btBroadphaseProxy::AllFilter) :
        from(physics::BulletUtils::ogreToBullet(ray.getOrigin())),
        to(physics::BulletUtils::ogreToBullet(ray.getOrigin() +
            ray.getDirection() * 99999.f)),
        filterMask(mask)
    {}
};


// The result of the raycast structure
struct RaycastResult {
    BulletObject* object;
    btVector3 worldNormal;      // hit normal vector
    btVector3 worldPosition;    // hit position
};

}



#endif /* RAYCASTINFO_H_ */
