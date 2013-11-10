/*
 * BodyPartEffect.cpp
 *
 *  Created on: Nov 4, 2013
 *      Author: agustin
 */

#include "BodyPartEffect.h"

#include <OgreSceneManager.h>

#include <physics/DynamicWorld.h>
#include <physics/BulletUtils.h>
#include <global_data/GlobalData.h>
#include <CZMasksDefines.h>



namespace cz {

physics::DynamicWorld* BodyPartEffect::sDynamicWorld = 0;
const float BodyPartEffect::MIN_TIME_IN_SCENE_SECS = 4.f;

////////////////////////////////////////////////////////////////////////////////
BodyPartEffect::BodyPartEffect() :
    mElement(0)
,   mElementQueue(0)
{

}
////////////////////////////////////////////////////////////////////////////////
BodyPartEffect::~BodyPartEffect()
{
}

////////////////////////////////////////////////////////////////////////////////
void
BodyPartEffect::configureForce(const Ogre::Vector3& impactPosition,
                               const Ogre::Vector3& direction,
                               float impactForce)
{
    ASSERT(mElement);

    // we will configure the force here
    // configure the force and direction associated to the bodyPart
    physics::BulletObject* bo = mElement->bulletObject;
    ASSERT(bo);
    ASSERT(bo->rigidBody);

    // clear the forces first
    bo->clearForces();

    // TODO: we are using directly bullet values here, ITS BAD, we need to
    // create a wrapper here.
    btVector3 force = physics::BulletUtils::ogreToBullet(direction);
    force.normalize();
    force *= (impactForce * 1.f);

    // We will get the position where we have to apply the force
    btVector3 relPos = physics::BulletUtils::ogreToBullet(impactPosition) -
        bo->rigidBody->getWorldTransform().getOrigin();
    bo->rigidBody->applyImpulse(force, relPos);
}

////////////////////////////////////////////////////////////////////////////////
void
BodyPartEffect::beforeStart(void)
{
    ASSERT(sDynamicWorld);
    ASSERT(mElement);
    ASSERT(mElement->bulletObject);

    // reset the bullet object
    mElement->bulletObject->motionState.setDirty(true);

    // we need to add the bullet element in the bullet world
    sDynamicWorld->addObject(*(mElement->bulletObject),
                             CZRayMask::CZRM_ZOMBIE_BODY_PART,
                             CZRayMask::CZRM_WORLD |
                             CZRayMask::CZRM_ZOMBIE_BODY_PART |
                             CZRayMask::CZRM_RAYCASTABLE);
    mElement->bulletObject->activate(true);

    // make the graphic representation visible
    Ogre::SceneManager* sceneMngr = GlobalData::sceneMngr;
    ASSERT(sceneMngr);
    Ogre::SceneNode* node = mElement->bulletObject->motionState.node();
    ASSERT(node);
    sceneMngr->getRootSceneNode()->addChild(node);
    mAccumTime = 0.f;
}

////////////////////////////////////////////////////////////////////////////////
bool
BodyPartEffect::update(float timeFrame)
{
    ASSERT(mElement);
    ASSERT(mElement->bulletObject);

    mAccumTime += timeFrame;

    // check if we have to wait
    if (mAccumTime < MIN_TIME_IN_SCENE_SECS) {
        return true;
    }

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
    ASSERT(mElementQueue);

    sDynamicWorld->removeObject(*(mElement->bulletObject));
    mElementQueue->letAvailable(mElement);

    // remove from the scene
    Ogre::SceneManager* sceneMngr = GlobalData::sceneMngr;
    ASSERT(sceneMngr);
    Ogre::SceneNode* node = mElement->bulletObject->motionState.node();
    ASSERT(node);
    sceneMngr->getRootSceneNode()->removeChild(node);

    mElement = 0;

    // we will put this bodypartEffect again in the queue
    letThisFree();
}

} /* namespace cz */
