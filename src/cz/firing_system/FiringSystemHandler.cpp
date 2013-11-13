/*
 * FiringSystemHandler.cpp
 *
 *  Created on: Oct 21, 2013
 *      Author: agustin
 */

#include "FiringSystemHandler.h"

#include <OgreSceneManager.h>
#include <OgreRay.h>

#include <physics/DynamicWorld.h>
#include <debug/PrimitiveDrawer.h>
#include <global_data/GlobalData.h>
#include <zombie_unit/ZombieUnit.h>
#include <zombie_unit/HitInfo.h>

namespace cz {


////////////////////////////////////////////////////////////////////////////////
FiringSystemHandler::FiringSystemHandler() :
    mDynamicWorld(0)
,   mRayQuery(0)
{
}
////////////////////////////////////////////////////////////////////////////////
FiringSystemHandler::~FiringSystemHandler()
{
    // destroy the query
    Ogre::SceneManager* sceneMngr = GlobalData::sceneMngr;
    ASSERT(sceneMngr);
    sceneMngr->destroyQuery(mRayQuery);
}

////////////////////////////////////////////////////////////////////////////////
void
FiringSystemHandler::update(void)
{
    ASSERT(mDynamicWorld);

    // TODO: we need to update each of the bullets and check if we collide
    // against something: World object (floor / wall / whatever) or an zombie.
    //

    Ogre::SceneManager* sceneMngr = GlobalData::sceneMngr;
    ASSERT(sceneMngr);
    ASSERT(mRayQuery);
    Ogre::Ray ray;
    HitInfo hitInfo;

    unsigned int index = 0;
    for (auto it = mBullets.begin(), end = mBullets.end(); it != end; ++it, ++index) {
        Bullet* bullet = *it;
        const Ogre::Vector3 start = bullet->currentPosition();
        bullet->update(GlobalData::lastTimeFrame);
        const Ogre::Vector3 endPos = bullet->currentPosition();
//        ASSERT(start != endPos);

        // check if the bullet is still inside the world limits
        if (!mWorldLimits.contains(endPos)) {
            // we need to remove this one
            remove(bullet);
            --it;
            end = mBullets.end();
        } else {
            // now we have to perform the raycast here from start to end and check
            // for collisions.

            // we will perform only ogre scene raycast for now, change this when
            // finish issue #215
            ray.setDirection(bullet->direction());
            ray.setOrigin(start);
            mRayQuery->setRay(ray);
            Ogre::RaySceneQueryResult& result = mRayQuery->execute();
            if (!result.empty()) {
                // we have a zombie?
                Ogre::RaySceneQueryResultEntry& closest = result.front();
                if (closest.movable) {
                    const Ogre::Any& any =
                        closest.movable->getUserObjectBindings().getUserAny();
                    if (!any.isEmpty()) {
                        ZombieUnit* zombie = Ogre::any_cast<ZombieUnit*>(any);
                        if (zombie) {
                            // we have a zombie, check if we hit him
                            hitInfo.reset(*bullet);
                            if (zombie->checkImpact(hitInfo)) {
                                // draw impact point
                                core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
                                pd.createSphere(hitInfo.intersectionPoint, 1, pd.getFreshColour());
                                zombie->processImpactInfo(hitInfo);
                            }
                        }
                    }
                }

                // note that if the bullet hits something means that we have to
                // remove that bullet from the handler
                //
                remove(bullet);
                --it;
                end = mBullets.end();
            }


//            ASSERT(false && "TODO: complete this");
        }
    }
}

} /* namespace cz */
