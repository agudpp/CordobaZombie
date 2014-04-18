/*
 * CollisionObject.h
 *
 *  Created on: Jan 7, 2014
 *      Author: agustin
 */

#ifndef COLLISIONOBJECT_H_
#define COLLISIONOBJECT_H_


#include <bullet/btBulletDynamicsCommon.h>

#include <debug/DebugUtil.h>

#include "Shape.h"
#include "BulletUtils.h"

#define COLLOBJ_PRECONDITION     ASSERT(mCollObject && "You must set a collision "\
                                        "object before being able to use this" \
                                        " class instance!")

namespace physics {

// forward
//
class PhysicsBuilder;


class CollisionObject
{
public:
    inline
    CollisionObject() : mCollObject(0) {};

    // Note since we will do nothing with this wrapper (free any memory) we will
    // do nothing in the destructor.. This is dangerous
    //
    //virtual
    ~CollisionObject(){};

    // @brief BECAREFUL USING THE COPY CONSTRUCTOR, we will not reallocate
    //        another shape for this, we will just use the same pointer!
    //
    CollisionObject(const CollisionObject&) = default;
    CollisionObject& operator=(const CollisionObject&) = default;

    inline btCollisionObject*
    bulletCollObj(void);

    // @brief Shape stuff
    //
    inline void
    setShape(Shape& shape);
    inline Shape&
    shape(void);
    inline const Shape&
    shape(void) const;

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

    // @brief Shortcuts for bullet stuff
    //
    inline btTransform&
    getWorldTransform(void);
    inline const btTransform&
    getWorldTransform(void) const;

private:

    // @brief We will let only to the PhysicsBuilder to construct this objects
    //        with bullet information.
    //
    friend class PhysicsBuilder;

    inline
    CollisionObject(btCollisionObject* co);

    // @brief Set the collision object pointer to be used
    //
    inline void
    setBullettCollObject(btCollisionObject* co);

protected:
    btCollisionObject* mCollObject;
    Shape mShape;
};







////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//
inline
CollisionObject::CollisionObject(btCollisionObject* co) :
    mCollObject(co)
{}

inline void
CollisionObject::setBullettCollObject(btCollisionObject* co)
{
    mCollObject = co;
}

inline btCollisionObject*
CollisionObject::bulletCollObj(void)
{
    return mCollObject;
}

// @brief Shape stuff
//
inline void
CollisionObject::setShape(Shape& shape)
{
    mShape = shape;
}

inline Shape&
CollisionObject::shape(void)
{
    return mShape;
}

inline const Shape&
CollisionObject::shape(void) const
{
    return mShape;
}

inline void
CollisionObject::setTransform(const Ogre::Vector3& pos, const Ogre::Quaternion& rot)
{
    COLLOBJ_PRECONDITION;
    btTransform &trans = mCollObject->getWorldTransform();;
    trans.setOrigin(BulletUtils::ogreToBullet(pos));
    trans.setRotation(BulletUtils::ogreToBullet(rot));
}
inline void
CollisionObject::setRotation(const Ogre::Quaternion& rot)
{
    COLLOBJ_PRECONDITION;
    btTransform &trans = mCollObject->getWorldTransform();;
    trans.setRotation(BulletUtils::ogreToBullet(rot));
}
inline void
CollisionObject::setPosition(const Ogre::Vector3& pos)
{
    COLLOBJ_PRECONDITION;
    btTransform &trans = mCollObject->getWorldTransform();;
    trans.setOrigin(BulletUtils::ogreToBullet(pos));
}

inline void
CollisionObject::setUserDef(void* data)
{
    COLLOBJ_PRECONDITION;
    mCollObject->setUserPointer(data);
}
inline void*
CollisionObject::userDef(void)
{
    COLLOBJ_PRECONDITION;
    return mCollObject->getUserPointer();
}


// @brief Shortcuts for bullet stuff
//
inline btTransform&
CollisionObject::getWorldTransform(void)
{
    COLLOBJ_PRECONDITION;
    return mCollObject->getWorldTransform();
}
inline const btTransform&
CollisionObject::getWorldTransform(void) const
{
    COLLOBJ_PRECONDITION;
    return mCollObject->getWorldTransform();
}


} /* namespace physics */
#endif /* COLLISIONOBJECT_H_ */
