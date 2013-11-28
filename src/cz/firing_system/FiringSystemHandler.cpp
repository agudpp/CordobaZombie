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
#include <physics/BulletUtils.h>
#include <debug/PrimitiveDrawer.h>
#include <global_data/GlobalData.h>
#include <physic_game_object/PhysicGameObject.h>
#include <CZMasksDefines.h>

namespace cz {


////////////////////////////////////////////////////////////////////////////////
FiringSystemHandler::FiringSystemHandler() :
    mDynamicWorld(0)
{
    // we only want to detect zombies and world objects
    mPhysicsRaycastInfo.filterGroup = CZRM_RAYCASTABLE;
}
////////////////////////////////////////////////////////////////////////////////
FiringSystemHandler::~FiringSystemHandler()
{
}

////////////////////////////////////////////////////////////////////////////////
void
FiringSystemHandler::update(void)
{
    ASSERT(mDynamicWorld);

    // TODO: we need to update each of the bullets and check if we collide
    // against something: World object (floor / wall / whatever) or an zombie.
    //
    HitInfo hitInfo;


    unsigned int index = 0;
    for (auto it = mBullets.begin(), end = mBullets.end(); it != end; ++it, ++index) {
        Bullet* bullet = *it;
        const Ogre::Vector3 start = bullet->currentPosition();
        bullet->update(GlobalData::lastTimeFrame);
        const Ogre::Vector3 endPos = bullet->currentPosition();

        // check if the bullet is still inside the world limits
        if (!mWorldLimits.contains(endPos)) {
            // we need to remove this one
            remove(bullet);
            --it;
            end = mBullets.end();
        } else if (start != endPos){
            // now we have to perform the raycast here from start to end and check
            // for collisions using bullet.
            mPhysicsRaycastInfo.from = physics::BulletUtils::ogreToBullet(start);
            mPhysicsRaycastInfo.to = physics::BulletUtils::ogreToBullet(endPos);
            physics::RaycastMultiResult raycastResult;
            if (mDynamicWorld->performRaycast(mPhysicsRaycastInfo,
                                               raycastResult)) {

                // configure the hit info just in case using the bullet
                // information
                hitInfo.reset(*bullet);

                // we get results, check what kind of bullet object is using
                // its flags.
                // Iterate over all the objects (already sorted by distance) and
                // perform whatever we need.
                //
                bool impactProcessed = false;
                const unsigned int numObjects = raycastResult.size();
                for (unsigned int i = 0; i < numObjects && (!impactProcessed); ++i) {
                    const btCollisionObject* btCollObj = raycastResult.btCollObject(i);
                    ASSERT(btCollObj && "We should have a bullet collision object "
                        "assoaciated always");

                    // get the associated user def pointer..
                    void* userDef = btCollObj->getUserPointer();
#ifdef DEBUG
                    if (userDef == 0) {
                        debugWARNING("We hit an element that is not being tracked!\n");
                        continue;
                    }
#else
                    ASSERT(userDef && "We should always have some element associated "
                        "to each bulletObject since we are performing the raycast"
                        " using the flags to detect zombies and world objects and"
                        " anything that could be raycasted using bullet!");
#endif

                    // get the PhysicGameObject associated to this bullet object
                    PhysicGameObject* physicsGameObj= static_cast<PhysicGameObject*>(userDef);

                    // check if this class will handle the impact or not, if not
                    // we will skip it and continue with the next one

                    if (physicsGameObj->checkImpact(hitInfo)) {
                        // we will set the needed information
                        hitInfo.intersectionPoint =
                            physics::BulletUtils::bulletToOgre(
                                raycastResult.worldPosition(i));
                        hitInfo.normalIntersection =
                            physics::BulletUtils::bulletToOgre(
                                raycastResult.worldNormal(i));

                        // process the intersection and hit information
                        physicsGameObj->processImpactInfo(hitInfo);

                        impactProcessed = true;
                    }
                }

                // check if we impact something we should remove the bullet
                if (impactProcessed) {
                    remove(bullet);
                    --it;
                    end = mBullets.end();
                }
            }
        }
    }
}

} /* namespace cz */
