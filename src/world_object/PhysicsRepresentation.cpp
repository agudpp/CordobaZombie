/*
 * PhysicsRepresentation.cpp
 *
 *  Created on: Jan 8, 2014
 *      Author: agustin
 */

#include "PhysicsRepresentation.h"

namespace w_o {

physics::DynamicWorld* PhysicsRepresentation::sDynamicWorld = 0;


PhysicsRepresentation::PhysicsRepresentation() :
    mType(PhysicsType::Physics_None)
,   mMasksSet(false)
,   mInWorld(false)
{
    // ensure we already have the dynamic world
    ASSERT(sDynamicWorld);

    mCollData.co = 0;
}

PhysicsRepresentation::~PhysicsRepresentation()
{
    setEnabled(false);
}

////////////////////////////////////////////////////////////////////////////////
void
PhysicsRepresentation::setCollisionObjectElement(physics::CollisionObject* collElement)
{
    ASSERT(collElement);
    ASSERT(!isEnabled() && "We cannot set the coll element when the object is enabled");

    mType = PhysicsType::Physics_CollObject;
    mCollData.co = collElement;
}

void
PhysicsRepresentation::setRigidBodyElement(physics::RigidBody* collElement)
{
    ASSERT(collElement);
    ASSERT(!isEnabled() && "We cannot set the coll element when the object is enabled");

    mType = PhysicsType::Physics_RigidBody;
    mCollData.rb = collElement;
}

////////////////////////////////////////////////////////////////////////////////
void
PhysicsRepresentation::addToDynamicWorld(void)
{
    if (mInWorld) {
        return;
    }
    ASSERT(mType != PhysicsType::Physics_None);
    ASSERT(mCollData.co != 0);

    if (mType == PhysicsType::Physics_RigidBody) {
        if (mMasksSet) {
            sDynamicWorld->addRigidBody(*mCollData.rb,
                                        mMasks.groupMask,
                                        mMasks.filterMask);
        } else {
            sDynamicWorld->addRigidBody(*mCollData.rb);
        }
    } else {
        if (mMasksSet) {
            sDynamicWorld->addCollisionObject(*mCollData.co,
                                              mMasks.groupMask,
                                              mMasks.filterMask);
        } else {
            sDynamicWorld->addCollisionObject(*mCollData.co);
        }
    }
    mInWorld = true;
}

////////////////////////////////////////////////////////////////////////////////
void
PhysicsRepresentation::removeFromDynamicWorld(void)
{
    if (!mInWorld) {
        return;
    }
    ASSERT(mType != PhysicsType::Physics_None);
    ASSERT(mCollData.co != 0);
    if (mType == PhysicsType::Physics_RigidBody) {
        sDynamicWorld->removeRigidBody(*mCollData.rb);
    } else {
        sDynamicWorld->removeCollisionObject(*mCollData.co);
    }
    mInWorld = false;
}

} /* namespace w_o */
