/*
 * RigidBody.h
 *
 *  Created on: Jan 7, 2014
 *      Author: agustin
 */

#ifndef RIGIDBODY_H_
#define RIGIDBODY_H_


#include <bullet/btBulletDynamicsCommon.h>

#include <debug/DebugUtil.h>

#include "CollisionObject.h"
#include "MotionState.h"



// some helpful defines
//
#define RIGIDBODY_PRECONDITION  ASSERT(mCollObject && "We need to set the RigidBody"\
                                       " before be able to use this class!")
#define RIGIDBODY_CAST(x)   static_cast<btRigidBody*>(x)


namespace physics {


// forward
//
class PhysicsBuilder;


class RigidBody : public CollisionObject
{
public:
    inline
    RigidBody() {};
    ~RigidBody(){};

    inline btRigidBody*
    bulletRigidBody(void);

    // @brief Helper method used to apply a force to the rigid body
    // @param force     The force we want to apply
    //
    inline void
    applyCentralForce(const Ogre::Vector3& force);

    inline void
    applyCentralImpulse(const Ogre::Vector3& impulse);
    inline void
    applyImpulse(const Ogre::Vector3& impulse, const Ogre::Vector3& rel_pos);
    inline void
    applyCentralImpulse(const btVector3& impulse);
    inline void
    applyImpulse(const btVector3& impulse, const btVector3& rel_pos);

    // @brief Set a position and orientation for this object
    //
    inline void
    setTransform(const Ogre::Vector3& pos, const Ogre::Quaternion& rot);

    // @brief Reset all the forces associated to the rigid Body (linear velocity,
    //        angular velocity, all the other forces too).
    //        this method should be called when the rigid body is not in the
    //        physics world.
    //
    inline void
    clearForces(void);

    // @brief Activate / deactivate the rigid body
    // @brief activate      Activate?
    //
    inline void
    activate(bool activate);

private:


    // @brief We will let only to the PhysicsBuilder to construct this objects
    //        with bullet information.
    //
    friend class PhysicsBuilder;

    // @brief Set the rigid body now
    //
    inline void
    setBulletRigidBody(btRigidBody* rb);
};










////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

////////////////////////////////////////////////////////////////////////////////
inline void
RigidBody::setBulletRigidBody(btRigidBody* rb)
{
    mCollObject = rb;
}

////////////////////////////////////////////////////////////////////////////////
inline btRigidBody*
RigidBody::bulletRigidBody(void)
{
    return RIGIDBODY_CAST(bulletCollObj());
}

inline void
RigidBody::applyCentralForce(const Ogre::Vector3& force)
{
    RIGIDBODY_PRECONDITION;
    RIGIDBODY_CAST(bulletCollObj())->applyCentralForce(BulletUtils::ogreToBullet(force));
}

inline void
RigidBody::applyCentralImpulse(const Ogre::Vector3& impulse)
{
    RIGIDBODY_PRECONDITION;
    RIGIDBODY_CAST(bulletCollObj())->applyCentralImpulse(BulletUtils::ogreToBullet(impulse));
}
inline void
RigidBody::applyImpulse(const Ogre::Vector3& impulse, const Ogre::Vector3& rel_pos)
{
    RIGIDBODY_PRECONDITION;
    RIGIDBODY_CAST(bulletCollObj())->applyImpulse(BulletUtils::ogreToBullet(impulse),
                                                  BulletUtils::ogreToBullet(rel_pos));
}
inline void
RigidBody::applyCentralImpulse(const btVector3& impulse)
{
    RIGIDBODY_PRECONDITION;
    RIGIDBODY_CAST(bulletCollObj())->applyCentralImpulse(impulse);
}
inline void
RigidBody::applyImpulse(const btVector3& impulse, const btVector3& rel_pos)
{
    RIGIDBODY_PRECONDITION;
    RIGIDBODY_CAST(bulletCollObj())->applyImpulse(impulse, rel_pos);
}

inline void
RigidBody::setTransform(const Ogre::Vector3& pos, const Ogre::Quaternion& rot)
{
    RIGIDBODY_PRECONDITION;
    btTransform &trans = RIGIDBODY_CAST(bulletCollObj())->getWorldTransform();
    trans.setIdentity();
    trans.setOrigin(BulletUtils::ogreToBullet(pos));
    trans.setRotation(BulletUtils::ogreToBullet(rot));

    // TODO: verify if this is really needed...
    btMotionState* ms = RIGIDBODY_CAST(bulletCollObj())->getMotionState();
    if (ms) {
        ms->setWorldTransform(trans);
    }
}

inline void
RigidBody::clearForces(void)
{
    RIGIDBODY_PRECONDITION;
    btRigidBody* rb = RIGIDBODY_CAST(bulletCollObj());
    rb->clearForces();
    rb->setAngularVelocity(btVector3(0,0,0));
    rb->setLinearVelocity(btVector3(0,0,0));
}

inline void
RigidBody::activate(bool activate)
{
    RIGIDBODY_PRECONDITION;
    RIGIDBODY_CAST(bulletCollObj())->activate(activate);
}


} /* namespace physics */
#endif /* RIGIDBODY_H_ */
