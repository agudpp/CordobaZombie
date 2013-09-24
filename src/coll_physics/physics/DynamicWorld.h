/*
 * DynamicWorld.h
 *
 *  Created on: Sep 19, 2013
 *      Author: agustin
 */

#ifndef DYNAMICWORLD_H_
#define DYNAMICWORLD_H_

#include <bullet/btBulletDynamicsCommon.h>

#include "BulletObject.h"

namespace physics {

// @brief For now we will use the default values for the broadphase algorithm
//        and the dispatcher, we can change or configure this using other
//        interface probably. Not for now
//


class DynamicWorld
{
public:
    // Default constructor with the default gravity vector
    DynamicWorld(const btVector3& gravity = btVector3(0,0,-10));
    ~DynamicWorld();

    // @brief Add a bulletObject to the world. Note that we will not check
    //        anything, only add the object in the world directly.
    // @param bo        The bullet object to add
    //
    inline void
    addObject(BulletObject& bo);

    // @brief Remove a dynamic object from the world. We will not check for
    //        anything, just call bullet->removeDynamic.
    // @param bo        The bullet object to remove
    //
    inline void
    removeObject(BulletObject& bo);

    // @brief Track a bullet object to be removed later and free its memory
    //        when we call freeAll() or destroy this instance). We will
    //        only remove the shape and rigid body if contains
    // @param bo        The bullet object to track.
    // @note that we will not check if you add twice the same object.
    //
    inline void
    trackObjectToRemove(BulletObject& bo);

    // @brief Stop tracking (to remove) an object.
    // @param bo        The bullet object we want to stop tracking.
    //
    inline void
    untrackObjectToRemove(BulletObject& bo);

private:
    btDefaultCollisionConfiguration mDefConf;
    btCollisionDispatcher mDispatcher;
    btDbvtBroadphase mBroadPhase;
    btSequentialImpulseConstraintSolver mSeqSolver;
    btDiscreteDynamicsWorld mDynamicWorld;
};

} /* namespace physics */
#endif /* DYNAMICWORLD_H_ */
