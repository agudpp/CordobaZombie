/*
 * CollObject.cpp
 *
 *  Created on: Aug 27, 2013
 *      Author: agustin
 */
#include "CollObject.h"

#include "CollisionHandler.h"

namespace coll {

CollisionHandler* CollObject::sCollHandler = 0;


void
CollObject::setBoundingBox(const core::AABB& bb)
{
    ASSERT(sCollHandler);
    sCollHandler->coUpdateBB(this, bb);
}
void
CollObject::setPreciseInfo(PreciseInfo* pi)
{
    ASSERT(sCollHandler);
    // check if we need to remove the info
    if (pi == 0 && mPinfo != 0) {
        delete mPinfo;
    }
    mPinfo = pi;
    sCollHandler->coUpdatePreciseInfo(this, mPinfo);
}


////////////////////////////////////////////////////////////////////////////
// Movement functions
//

void
CollObject::setPosition(const core::Vector2& pos)
{
    ASSERT(sCollHandler);
    sCollHandler->coSetPosition(this, pos);
}

void
CollObject::translate(const core::Vector2& tvec)
{
    ASSERT(sCollHandler);
    sCollHandler->coTranslate(this, tvec);
}

void
CollObject::setCollisionsEnabled(bool enable)
{
    ASSERT(sCollHandler);
    sCollHandler->coEnableCollisions(this, enable);
}

}


