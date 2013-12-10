/*
 * WorldObject.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#include "WorldObject.h"

#include <debug/DebugUtil.h>
#include <physics/BulletObject.h>

namespace demo_app {

GameLogicData* WorldObject::sGameLogicData = 0;


WorldObject::WorldObject(float maxSide, bool isGood) :
    mMaxSize(maxSide)
,   mIsGood(isGood)
{
    ASSERT(sGameLogicData);
}

WorldObject::~WorldObject()
{
}

bool
WorldObject::update(float timeFrame)
{
    // check if we are in the floor
    physics::BulletObject& bo = bulletObject();
    const float z = bo.motionState.node()->getPosition().z;
    if (z != 0.f && z < mMaxSize) {
        // we finish here, increment the counter
        if (mIsGood) {
            sGameLogicData->goodBoxes += 1;
        } else {
            sGameLogicData->badBoxes += 1;
        }

        return false;
    }

    return true;
}

} /* namespace demo_app */
