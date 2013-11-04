/*
 * FiringSystemHandler.cpp
 *
 *  Created on: Oct 21, 2013
 *      Author: agustin
 */

#include "FiringSystemHandler.h"

#include <physics/DynamicWorld.h>
#include <global_data/GlobalData.h>

namespace cz {


////////////////////////////////////////////////////////////////////////////////
FiringSystemHandler::FiringSystemHandler() :
    mDynamicWorld(0)
{
}
////////////////////////////////////////////////////////////////////////////////
FiringSystemHandler::~FiringSystemHandler()
{
    // nothing to do
}

////////////////////////////////////////////////////////////////////////////////
void
FiringSystemHandler::update(void)
{
    ASSERT(mDynamicWorld);

    // TODO: we need to update each of the bullets and check if we collide
    // against something: World object (floor / wall / whatever) or an zombie.
    //
    ASSERT(false && "TODO: Implement this when we have finished issue #130");

    for (auto it = mBullets.begin(), end = mBullets.end(); it != end; ++it) {
        Bullet* bullet = *it;
        const Ogre::Vector3 start = bullet->currentPosition();
        bullet->update(GlobalData::lastTimeFrame);
        const Ogre::Vector3 end = bullet->currentPosition();
        ASSERT(start != end);

        // now we have to perform the raycast here from start to end and check
        // for collisions.
    }
}

} /* namespace cz */
