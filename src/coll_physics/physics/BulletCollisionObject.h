/*
 * BulletCollisionObject.h
 *
 *  Created on: Nov 6, 2013
 *      Author: agustin
 */

#ifndef BULLETCOLLISIONOBJECT_H_
#define BULLETCOLLISIONOBJECT_H_


#include <bullet/btBulletCollisionCommon.h>

#include <debug/DebugUtil.h>

#include "BulletShape.h"
#include "BulletUtils.h"



// @brief This will represent a collision object only (collision shape and
//        collision object only).
// @note THIS CLASS WILL NOT HANDLE ANY MEMORY, IT WILL ONLY HOLD THE INFORMATION
//

namespace physics {

struct BulletCollisionObject {
    btCollisionObject collObject;

    BulletCollisionObject(BulletShape* shape = 0)
    {
        if (shape) {
            collObject.setCollisionShape(shape);
        }
    }

    // @brief Return the associated shape
    //
    inline BulletShape*
    shape(void);
    inline const BulletShape*
    shape(void) const;

    // @brief Set the shape to be used
    // @param shape     The bullet shape
    //
    inline void
    setShape(BulletShape* shape);

    // @brief Set the position and orientation of the collision object
    // @param pos       The position
    // @param rot       The rotation / orientation
    //
    inline void
    setTransform(const Ogre::Vector3& pos, const Ogre::Quaternion& rot);
    inline void
    setRotation(const Ogre::Quaternion& rot);
    inline void
    setPosition(const Ogre::Vector3& pos);

    // @brief Set / get user defined object
    //
    inline void
    setUserDef(void* data);
    inline void*
    userDef(void);

private:
    // avoid copying
    BulletCollisionObject(const BulletCollisionObject&);
    BulletCollisionObject& operator=(const BulletCollisionObject&);
};



////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

// @brief Return the associated shape
//
inline BulletShape*
BulletCollisionObject::shape(void)
{
    return collObject.getCollisionShape();
}
inline const BulletShape*
BulletCollisionObject::shape(void) const
{
    return collObject.getCollisionShape();
}

inline void
BulletCollisionObject::setShape(BulletShape* shape)
{
    ASSERT(shape);
    collObject.setCollisionShape(shape);
}

inline void
BulletCollisionObject::setTransform(const Ogre::Vector3& pos, const Ogre::Quaternion& rot)
{
    btTransform &trans = collObject.getWorldTransform();;
    trans.setOrigin(BulletUtils::ogreToBullet(pos));
    trans.setRotation(BulletUtils::ogreToBullet(rot));
}
inline void
BulletCollisionObject::setRotation(const Ogre::Quaternion& rot)
{
    btTransform &trans = collObject.getWorldTransform();;
    trans.setRotation(BulletUtils::ogreToBullet(rot));
}
inline void
BulletCollisionObject::setPosition(const Ogre::Vector3& pos)
{
    btTransform &trans = collObject.getWorldTransform();;
    trans.setOrigin(BulletUtils::ogreToBullet(pos));
}

inline void
BulletCollisionObject::setUserDef(void* data)
{
    collObject.setUserPointer(data);
}
inline void*
BulletCollisionObject::userDef(void)
{
    return collObject.getUserPointer();
}

} /* namespace physics */
#endif /* BULLETCOLLISIONOBJECT_H_ */
