/*
 * WorldObject.cpp
 *
 *  Created on: Sep 7, 2013
 *      Author: agustin
 */

#include "WorldObject.h"

#include <debug/PrimitiveDrawer.h>

namespace cz {

coll::CollisionHandler* WorldObject::sCollHandler = 0;

WorldObject::WorldObject() :
    mEntity(0)
,   mNode(0)
,   mSqrRadius(0)
,   mHeight(0)
,   mCollObj(0)
{
    ASSERT(sCollHandler);

    // construct the coll object
    mCollObj = sCollHandler->getNewCollObject();
}

WorldObject::~WorldObject()
{
    // destroy the coll object
    sCollHandler->deleteCollObject(mCollObj);
}


////////////////////////////////////////////////////////////////////////////////
// Collision stuff methods
//


void
WorldObject::setCollisionInformation(coll::CollPreciseInfo* pi,
                                     const core::AABB& bb)
{
    if (pi != 0) {
        // we need to set the information to the collObject
        mCollObj->setPreciseInfo(pi);
    } else {
        mCollObj->destroyPreciseInfo();
        mCollObj->setBoundingBox(bb);
    }

    // now we will calculate the radius of the object
    mSqrRadius = mCollObj->boundingBox().calculateSquaredRadius();
    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
    core::Primitive* p = pd.createBox(mCollObj->boundingBox(), pd.getFreshColour());
    p->node->getParentSceneNode()->removeChild(p->node);
    mNode->addChild(p->node);
}

////////////////////////////////////////////////////////////////////////////////
void
WorldObject::WorldObject::setCollisionType(bool isStatic)
{
    ASSERT(sCollHandler);
    ASSERT(mCollObj);
    ASSERT(!mCollObj->isInCollisionWorld());

    if (isStatic) {
        sCollHandler->coAddStatic(mCollObj);
    } else {
        sCollHandler->coAddDynamic(mCollObj);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
WorldObject::removeFromCollWorld(void)
{
    ASSERT(sCollHandler);

    if (mCollObj->isStatic()) {
        sCollHandler->coRemoveStatic(mCollObj);
    } else {
        sCollHandler->coRemoveDynamic(mCollObj);
    }
}


} /* namespace cz */
