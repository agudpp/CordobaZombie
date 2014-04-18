/*
 * Coll2DRepresentation.cpp
 *
 *  Created on: Jan 8, 2014
 *      Author: agustin
 */

#include "Coll2DRepresentation.h"

namespace w_o {

coll::CollisionHandler* Coll2DRepresentation::sCollHandler = 0;

Coll2DRepresentation::Coll2DRepresentation() :
    mCollType(Coll2DType::Coll2D_Dynamic)
,   mCollObj(0)
,   mSqrRadius(-1.f)
{
    ASSERT(sCollHandler);

    // construct the coll object
    mCollObj = sCollHandler->getNewCollObject();

}

Coll2DRepresentation::~Coll2DRepresentation()
{
    // destroy the coll object
    sCollHandler->deleteCollObject(mCollObj);
}

void
Coll2DRepresentation::setCollisionInformation(coll::CollPreciseInfo* pi,
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
}

} /* namespace w_o */
