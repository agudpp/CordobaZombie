/*
 * StaticWorldObject.cpp
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#include "StaticWorldObject.h"

namespace cz {

////////////////////////////////////////////////////////////////////////////////
StaticWorldObject::StaticWorldObject()
{
    // we will set the current bullet user pointer to point this instance
    mPhysicsRep.collObject.setUserPointer(this);

}

////////////////////////////////////////////////////////////////////////////////
StaticWorldObject::~StaticWorldObject()
{
    // delete the associated shape if we have one
    if (mPhysicsRep.shape()) {
        ASSERT(sDynamicWorld);
        sDynamicWorld->removeObject(mPhysicsRep);
        delete mPhysicsRep.shape();
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
StaticWorldObject::checkImpact(HitInfo& hitInfo) const
{
    ASSERT(false && "Implement this, probably return true always\n");
}

////////////////////////////////////////////////////////////////////////////////
void
StaticWorldObject::processImpactInfo(const HitInfo& hitInfo)
{
    ASSERT(false && "Implement this, play a sound, show some particles...\n");
}

} /* namespace cz */
