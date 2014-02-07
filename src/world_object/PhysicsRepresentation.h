/*
 * PhysicsRepresentation.h
 *
 *  Created on: Jan 8, 2014
 *      Author: agustin
 */

#ifndef PHYSICSREPRESENTATION_H_
#define PHYSICSREPRESENTATION_H_


#include <physics/CollisionObject.h>
#include <physics/RigidBody.h>
#include <physics/DynamicWorld.h>
#include <debug/DebugUtil.h>

#include "MaskData.h"


namespace w_o {

class PhysicsRepresentation
{
public:

    // public enum to distinguish between collision objects and rigid bodies
    //
    enum PhysicsType {
        Physics_None = 0,
        Physics_RigidBody,
        Physics_CollObject,
    };

    // @brief Set the DynamicWorld instance to be used by all the representations
    // @param dw        The DynamicWorld instance.
    //
    static inline void
    setDynamicWorld(physics::DynamicWorld* dw);

public:
    PhysicsRepresentation();
    ~PhysicsRepresentation();

    // avoid copy constructor
    //
    PhysicsRepresentation(const PhysicsRepresentation&) = delete;
    PhysicsRepresentation& operator=(const PhysicsRepresentation&) = delete;

    // The Specific interface methods
    //

    // @brief Set the collision data to this class. We can choose to be a collision
    //        object or a rigid body depending which of this methods we call.
    //        Note if we already have set something before we will don't delete
    //        any memory nor we will destroy the associated collision object
    //        set. Basically we will not handle any memory.
    // @param collElement       The collision element we will use
    // @note That the element shouldn't be already in the dynamic world. If that
    //       happens then we will get a crash inside of the bulletDynamicWorld
    //
    void
    setCollisionObjectElement(physics::CollisionObject* collElement);
    void
    setRigidBodyElement(physics::RigidBody* collElement);

    // @brief Get the collision data
    //
    inline physics::CollisionObject*
    collisionObject(void);
    inline const physics::CollisionObject*
    collisionObject(void) const;
    inline physics::RigidBody*
    rigidBody(void);
    inline const physics::RigidBody*
    rigidBody(void) const;

    // @brief Return the type of physics element we have
    //
    inline PhysicsType
    physicsType(void) const;

    // @brief Check if the physic element is already in the World.
    //
    inline bool
    isInDynamicWorld(void) const;

    // @brief Add the collision element to the dynamic world (if not already)
    //
    void
    addToDynamicWorld(void);
    void
    removeFromDynamicWorld(void);


    // The common Interface for all the representations
    //

    // @brief Set this representation enabled / disabled
    //
    inline void
    setEnabled(bool enabled);
    inline bool
    isEnabled(void) const;

    // @brief Set the position for this representation in the world.
    // @param pos       The position where we should put it.
    //
    inline void
    setPosition(const Ogre::Vector3& pos);

    // @brief Set the Orientation for this representation in the world.
    // @param orientation       The orientation.
    //
    inline void
    setOrientation(const Ogre::Quaternion& orientation);

    // @brief Set the scale associated to the world object for this representation.
    // @param scale         The new scale
    //
    inline void
    setScale(const Ogre::Vector3& scale);

    // @brief Set the Mask data to this representation associated to the WorldObject.
    // @param masks     The masks information
    //
    inline void
    setMasks(const MaskData& masks);

    // @brief Set the user defined pointer used for this world object for this
    //        representation.
    // @param userDef       The user def pointer.
    //
    inline void
    setUserDef(void* userDef);

private:
    // shared dynamic world used...
    //
    static physics::DynamicWorld* sDynamicWorld;

    PhysicsType mType;
    union CollData {
        physics::CollisionObject* co;
        physics::RigidBody* rb;
    } mCollData;

    MaskData mMasks;
    bool mMasksSet;
    bool mInWorld;
};









////////////////////////////////////////////////////////////////////////////////
//                              INLINE STUFF                                  //
////////////////////////////////////////////////////////////////////////////////

inline void
PhysicsRepresentation::setDynamicWorld(physics::DynamicWorld* dw)
{
    ASSERT(dw);
    sDynamicWorld = dw;
}

inline physics::CollisionObject*
PhysicsRepresentation::collisionObject(void)
{
#ifdef DEBUG
    if (physicsType() != PhysicsRepresentation::PhysicsType::Physics_CollObject) {
        debugWARNING("We are trying to get a collision object but we have a rigid\n");
    }
#endif
    return mCollData.co;
}
inline const physics::CollisionObject*
PhysicsRepresentation::collisionObject(void) const
{
#ifdef DEBUG
    if (physicsType() != PhysicsRepresentation::PhysicsType::Physics_CollObject) {
        debugWARNING("We are trying to get a collision object but we have a rigid\n");
    }
#endif
    return mCollData.co;
}
inline physics::RigidBody*
PhysicsRepresentation::rigidBody(void)
{
#ifdef DEBUG
    if (physicsType() != PhysicsRepresentation::PhysicsType::Physics_RigidBody) {
        debugWARNING("We are trying to get a rigid body object but we have a "
            "collision one\n");
    }
#endif
    return mCollData.rb;
}
inline const physics::RigidBody*
PhysicsRepresentation::rigidBody(void) const
{
#ifdef DEBUG
    if (physicsType() != PhysicsRepresentation::PhysicsType::Physics_RigidBody) {
        debugWARNING("We are trying to get a rigid body object but we have a "
            "collision one\n");
    }
#endif
    return mCollData.rb;
}

inline PhysicsRepresentation::PhysicsType
PhysicsRepresentation::physicsType(void) const
{
    return mType;
}

inline bool
PhysicsRepresentation::isInDynamicWorld(void) const
{
    return mInWorld;
}

// The common Interface for all the representations
//

inline void
PhysicsRepresentation::setEnabled(bool enabled)
{
    if (enabled) {
        addToDynamicWorld();
    } else {
        removeFromDynamicWorld();
    }
}
inline bool
PhysicsRepresentation::isEnabled(void) const
{
    return isInDynamicWorld();
}

inline void
PhysicsRepresentation::setPosition(const Ogre::Vector3& pos)
{
    ASSERT(mCollData.co);
    mCollData.co->setPosition(pos);
}

inline void
PhysicsRepresentation::setOrientation(const Ogre::Quaternion& orientation)
{
    ASSERT(mCollData.co);
    mCollData.co->setRotation(orientation);
}

inline void
PhysicsRepresentation::setScale(const Ogre::Vector3& scale)
{
    ASSERT(false && "We are not supporing this right now for dynamic scaling, "
        "probably different scales are supported when building the object but not"
        " dynamically!"
        "TODO!");
}

inline void
PhysicsRepresentation::setMasks(const MaskData& masks)
{
    mMasks = masks;
    mMasksSet = true;
}

inline void
PhysicsRepresentation::setUserDef(void* userDef)
{
    ASSERT(mCollData.co);
    mCollData.co->setUserDef(userDef);
}


} /* namespace w_o */
#endif /* PHYSICSREPRESENTATION_H_ */
