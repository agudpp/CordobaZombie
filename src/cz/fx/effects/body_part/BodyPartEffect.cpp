/*
 * BodyPartEffect.cpp
 *
 *  Created on: Nov 4, 2013
 *      Author: agustin
 */

#include "BodyPartEffect.h"

#include <physics/DynamicWorld.h>
#include <CZMasksDefines.h>


namespace cz {

physics::DynamicWorld* BodyPartEffect::sDynamicWorld = 0;

////////////////////////////////////////////////////////////////////////////////
BodyPartEffect::BodyPartEffect() :
    mElement(0)
,   mQueue(0)
{
    // TODO Auto-generated constructor stub

}
////////////////////////////////////////////////////////////////////////////////
BodyPartEffect::~BodyPartEffect()
{
    // TODO Auto-generated destructor stub
}

////////////////////////////////////////////////////////////////////////////////
void
BodyPartEffect::beforeStart(void)
{
    ASSERT(sDynamicWorld);
    ASSERT(mElement);
    ASSERT(mElement->bulletObject);
    // we need to add the bullet element in the bullet world
    sDynamicWorld->addObject(*(mElement->bulletObject), CZRM_ALL, CZRM_ALL);
}

////////////////////////////////////////////////////////////////////////////////
bool
BodyPartEffect::update(float timeFrame)
{
    ASSERT(mElement);
    ASSERT(mElement->bulletObject);

    // we need to check if the body part stops.
    if (mElement->bulletObject->motionState.isDirty()) {
        // reset the dirty flag
        mElement->bulletObject->motionState.setDirty(false);
        return true;
    }

    // the body part stops moving we don't need to reproduce this effect anymore
    //
    return false;
}

////////////////////////////////////////////////////////////////////////////////
void
BodyPartEffect::afterFinish(void)
{
    // remove the bullet object from the world and return it to the associated
    // queue
    ASSERT(sDynamicWorld);
    ASSERT(mElement);
    ASSERT(mElement->bulletObject);
    ASSERT(mQueue);

    sDynamicWorld->removeObject(*(mElement->bulletObject));
    mQueue->letAvailable(mElement);
    mElement = 0;
}

} /* namespace cz */
