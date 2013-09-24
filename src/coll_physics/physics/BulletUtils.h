/*
 * BulletUtils.h
 *
 *  Created on: Sep 19, 2013
 *      Author: agustin
 */

#ifndef BULLETUTILS_H_
#define BULLETUTILS_H_

#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreRay.h>
#include <OgreMath.h>

#include <bullet/btBulletCollisionCommon.h>

namespace physics {

namespace BulletUtils {

// @brief Functions to convert from ogre to bullet and bullet to ogre the
//        rotation and orientation
//
inline Ogre::Vector3
bulletToOgre(const btVector3& vec);
inline Ogre::Quaternion
bulletToOgre(const btQuaternion& rot);
inline btVector3
ogreToBullet(const Ogre::Vector3& vec);
inline btQuaternion
ogreToBullet(const Ogre::Quaternion& rot);


// @brief Check if a btBoxShape and a ogre ray intersects or not.
// @param worldTrans    The world transform matrix of the shape
// @param extents       The extents of the box
// @param ray           The ogre ray
// @return std::pair<intersects?, distance>
//
inline std::pair<bool, float>
intersects(const btTransform& worldTrans,
           const btVector3& extents,
           const Ogre::Ray& ray);

}






////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

namespace BulletUtils {


inline Ogre::Quaternion
bulletToOgre(const btQuaternion& rot)
{
    return Ogre::Quaternion(rot.w(), rot.x(), rot.y(), rot.z());
}
inline Ogre::Vector3
bulletToOgre(const btVector3& v)
{
    return Ogre::Vector3(v.x(), v.y(), v.z());
}
inline btQuaternion
ogreToBullet(const Ogre::Quaternion& rot)
{
    return btQuaternion(rot.x, rot.y, rot.z, rot.w);
}
inline btVector3
ogreToBullet(const Ogre::Vector3& v)
{
    return btVector3(v.x, v.y, v.z);
}


inline std::pair<bool, float>
intersects(const btTransform& worldTrans,
           const btVector3& extents,
           const Ogre::Ray& ray)
{
    btTransform invWorlObj = worldTrans.inverse();
    Ogre::Ray localRay(bulletToOgre(invWorlObj * ogreToBullet(ray.getOrigin())),
                       bulletToOgre(invWorlObj.getRotation()) * ray.getDirection());
    Ogre::AxisAlignedBox bb(bulletToOgre(-extents),
                            bulletToOgre(extents));
    return Ogre::Math::intersects(localRay, bb);
}


}

} /* namespace physics */
#endif /* BULLETUTILS_H_ */
