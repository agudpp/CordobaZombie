/*
 * DynamicWorld.cpp
 *
 *  Created on: Sep 19, 2013
 *      Author: agustin
 */

#include "DynamicWorld.h"

namespace physics {

DynamicWorld::DynamicWorld(const btVector3& gravity) :
    mDispatcher(&mDefConf)
,   mDynamicWorld(&mDispatcher, &mBroadPhase, &mSeqSolver, &mDefConf)
{
    mDynamicWorld.setGravity(gravity);
}

DynamicWorld::~DynamicWorld()
{

}

} /* namespace physics */
