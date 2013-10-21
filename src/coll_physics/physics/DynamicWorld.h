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

#include "BulletObject.h"
#include "RaycastInfo.h"

namespace physics {

// @brief For now we will use the default values for the broadphase algorithm
//        and the dispatcher, we can change or configure this using other
//        interface probably. Not for now
//

// Forward
//
class DynamicWorld;
typedef std::function<void (DynamicWorld*, BulletObject*)> MovementObjCb;

class DynamicWorld
{
public:
    // Default constructor with the default gravity vector
    DynamicWorld(const btVector3& gravity = btVector3(0,0,-10));
    ~DynamicWorld();

    // @brief Set the gravity vector for this world.
    // @param gravity   The gravity vector
    //
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

    // @brief Add a bulletObject to the world. Note that we will not check
    //        anything, only add the object in the world directly.
    // @param bo        The bullet object to add
    // @param groupMask The group mask associated to this bo [optional]
    // @param boMask    The object mask [optional]
    //
    inline void
    addObject(BulletObject& bo, short int groupMask = ~0, short int boMask = ~0);

    // @brief Remove a dynamic object from the world. We will not check for
    //        anything, just call bullet->removeDynamic.
    // @param bo        The bullet object to remove
    //
    inline void
    removeObject(BulletObject& bo);

    ////////////////////////////////////////////////////////////////////////////

    // @brief Add an BulletObject to be tracked until it stops moving. Note that
    //        we must call the update() method each frame if we want to know
    //        about this.
    //        You need to call first the addObject to be updated by the physics
    //        world.
    // @param bo        The bullet object to check
    // @param cb        The callback we will call once the object stops moving.
    //
    inline void
    checkMovement(BulletObject* bo, MovementObjCb& cb);

    // @brief Remove the object we are checking its movement.
    // @param bo        The object we are checking its movement.
    // @note This method will not call the callback associated.
    //
    inline void
    stopCheckMovement(BulletObject* bo);

    ////////////////////////////////////////////////////////////////////////////

    // @brief Perform raycast and returns the first BulletObject we intersect
    //        for the given ray and masks
    // @param ri          The raycast information
    // @param result      The raycast result
    // @return true if some object was intersected | false otherwise
    //
    bool
    performClosestRay(const RaycastInfo& ri, RaycastResult& result) const;

    ////////////////////////////////////////////////////////////////////////////

    // @brief Update the objects we are tracking
    //
    inline void
    update(void);

private:

    // Internal classes
    //
    struct MovementInfo {
        BulletObject* object;
        MovementObjCb callback;

        MovementInfo(BulletObject* bo, MovementObjCb& cb) :
            object(bo), callback(cb)
        {}
        MovementInfo(){}

    };

private:
    btDefaultCollisionConfiguration mDefConf;
    btCollisionDispatcher mDispatcher;
    btDbvtBroadphase mBroadPhase;
    btSequentialImpulseConstraintSolver mSeqSolver;
    btDiscreteDynamicsWorld mDynamicWorld;
    std::vector<MovementInfo> mMovableObjects;
};



////////////////////////////////////////////////////////////////////////////////
// Inline
//

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
DynamicWorld::addObject(BulletObject& bo, short int groupMask, short int boMask)
{
    ASSERT(bo.rigidBody);
    if (groupMask == ~0 && boMask == ~0) {
        mDynamicWorld.addRigidBody(bo.rigidBody);
    } else {
        mDynamicWorld.addRigidBody(bo.rigidBody, groupMask, boMask);
    }
}

inline void
DynamicWorld::removeObject(BulletObject& bo)
{
    mDynamicWorld.removeRigidBody(bo.rigidBody);
}

inline void
DynamicWorld::checkMovement(BulletObject* bo, MovementObjCb& cb)
{
    ASSERT(bo);
    mMovableObjects.push_back(MovementInfo(bo, cb));
    bo->motionState.setDirty(true);
}

inline void
DynamicWorld::stopCheckMovement(BulletObject* bo)
{
    ASSERT(bo);
    unsigned int i = 0, size = mMovableObjects.size();
    while (i < size && mMovableObjects[i].object != bo) ++i;
    if (i < size) {
        // remove the element
        mMovableObjects[i] = mMovableObjects.back();
        mMovableObjects.pop_back();
    }
}

inline void
DynamicWorld::update(void)
{
    for (MovementInfo& mi : mMovableObjects) {
        if (!mi.object->motionState.isDirty()) {
            // we need to advise that this object is not being moved anymore
            ASSERT(false && "TODO!");
        }
    }
}



} /* namespace physics */
#endif /* DYNAMICWORLD_H_ */
