/*
 * DynamicWorld.h
 *
 *  Created on: Sep 19, 2013
 *      Author: agustin
 */

#ifndef DYNAMICWORLD_H_
#define DYNAMICWORLD_H_

#include <vector>
#include <functional>

#include <OgreRay.h>

#include <bullet/btBulletDynamicsCommon.h>

#include <debug/DebugUtil.h>

#include "RigidBody.h"
#include "CollisionObject.h"
#include "RaycastInfo.h"
#include "BulletDebugDrawer.h"

namespace physics {

// @brief For now we will use the default values for the broadphase algorithm
//        and the dispatcher, we can change or configure this using other
//        interface probably. Not for now
//

// Forward
//

class DynamicWorld
{
public:
    // Default constructor with the default gravity vector
    DynamicWorld(const Ogre::Vector3& gravity = Ogre::Vector3(0,0,-10));
    ~DynamicWorld();

    // @brief Set the gravity vector for this world.
    // @param gravity   The gravity vector
    //
    inline void
    setGravity(const Ogre::Vector3& gravity);
    inline void
    setGravity(const btVector3& gravity);

    // @brief Return the instance of the btDynamicWorld
    //
    inline btDynamicsWorld&
    bulletDynamicWorld(void);
    inline const btDynamicsWorld&
    bulletDynamicWorld(void) const;

    // @brief Simulate the physics world with an given amount of sub simulation
    //        steps.
    // @param fTime     The frame time
    // @param steps     The number of simulation sub steps
    //
    inline void
    simulate(float fTime, unsigned int steps = 10);

    // @brief Add a CollisionObject to the world. Note that we will not check
    //        anything, only add the object in the world directly.
    // @param co            The Collision object to add
    // @param groupMask     The group mask associated to this co
    // @param filterMask    The object filter mask
    //
    inline void
    addCollisionObject(CollisionObject& co);
    inline void
    addCollisionObject(CollisionObject& co, short int groupMask, short int filterMask);
    inline void
    addRigidBody(RigidBody& rb);
    inline void
    addRigidBody(RigidBody& rb, short int groupMask, short int filterMask);

    // @brief Remove a collision object / rigid body from the world.
    //        We will not check for anything, just call bullet->removeDynamic.
    // @param bo        The bullet object to remove
    //
    inline void
    removeCollisionObject(CollisionObject& co);
    inline void
    removeRigidBody(RigidBody& rb);

    ////////////////////////////////////////////////////////////////////////////

    // @brief Perform raycast and returns the first BulletObject we intersect
    //        for the given ray and masks
    // @param ri          The raycast information
    // @param result      The raycast result
    // @return true if some object was intersected | false otherwise
    //
    bool
    performClosestRay(const RaycastInfo& ri, RaycastResult& result) const;

    // @brief Perform a raycast and return all the raycasted world objects
    //        already sorted by distance.
    // @param ri        The raycast information
    // @param result    The raycast result
    // @return true if some object was intersected | false otherwise
    //
    bool
    performRaycast(const RaycastInfo& ri, RaycastMultiResult& result) const;


private:
    btDefaultCollisionConfiguration mDefConf;
    btCollisionDispatcher mDispatcher;
    btDbvtBroadphase mBroadPhase;
    btSequentialImpulseConstraintSolver mSeqSolver;
    btDiscreteDynamicsWorld mDynamicWorld;
};



////////////////////////////////////////////////////////////////////////////////
// Inline
//

inline void
DynamicWorld::setGravity(const Ogre::Vector3& gravity)
{
    mDynamicWorld.setGravity(BulletUtils::ogreToBullet(gravity));
}
inline void
DynamicWorld::setGravity(const btVector3& gravity)
{
    mDynamicWorld.setGravity(gravity);
}

inline btDynamicsWorld&
DynamicWorld::bulletDynamicWorld(void)
{
    return mDynamicWorld;
}
inline const btDynamicsWorld&
DynamicWorld::bulletDynamicWorld(void) const
{
    return mDynamicWorld;
}

inline void
DynamicWorld::simulate(float fTime, unsigned int steps)
{
    mDynamicWorld.stepSimulation(fTime, 10);
}


inline void
DynamicWorld::addCollisionObject(CollisionObject& co)
{
    ASSERT(co.bulletCollObj());
    mDynamicWorld.addCollisionObject(co.bulletCollObj());
}

inline void
DynamicWorld::addCollisionObject(CollisionObject& co, short int groupMask, short int filterMask)
{
    ASSERT(co.bulletCollObj());
    mDynamicWorld.addCollisionObject(co.bulletCollObj(), groupMask, filterMask);
}
inline void
DynamicWorld::addRigidBody(RigidBody& rb)
{
    ASSERT(rb.bulletRigidBody());
    mDynamicWorld.addRigidBody(rb.bulletRigidBody());
}
inline void
DynamicWorld::addRigidBody(RigidBody& rb, short int groupMask, short int filterMask)
{
    ASSERT(rb.bulletRigidBody());
    mDynamicWorld.addRigidBody(rb.bulletRigidBody(), groupMask, filterMask);

}

inline void
DynamicWorld::removeCollisionObject(CollisionObject& co)
{
    ASSERT(co.bulletCollObj());
    mDynamicWorld.removeCollisionObject(co.bulletCollObj());
}
inline void
DynamicWorld::removeRigidBody(RigidBody& rb)
{
    ASSERT(rb.bulletRigidBody());
    mDynamicWorld.removeRigidBody(rb.bulletRigidBody());
}



} /* namespace physics */
#endif /* DYNAMICWORLD_H_ */
