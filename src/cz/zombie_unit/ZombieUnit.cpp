/*
 * ZombieUnit.cpp
 *
 *  Created on: Sep 9, 2013
 *      Author: agustin
 */

#include "ZombieUnit.h"

#include <cmath>

#include <physics/BulletObject.h>
#include <physics/BulletImporter.h>
#include <physics/DynamicWorld.h>
#include <effect_handler/EffectHandler.h>

#include "BodyPartElement.h"
#include "CZMasksDefines.h"

// helper stuff
//
namespace {

//      ZA_RUN1,
//      ZA_RUN2,
//      ZA_HIT_SHOULDER_R,
//      ZA_HIT_SHOULDER_L,
//      ZA_FALL,
//      ZA_DRAG,
//      ZA_DRAG_DIE,
//      ZA_HEAD_DIE,
//      ZA_NORMAL_DIE1,
//      ZA_NORMAL_DIE2,
static const char* ZA_ANIMS[] = {
    "corre",
    "corre2",
    "golpe_hombrod",
    "golpe_hombroi",
    "cae",
    "arrastra",
    "muere_arrastra",
    "muere_cabeza",
    "muere_normal1",
    "muere_normal2",
    0
};

}

namespace cz {

ZombieTTable ZombieUnit::sTTable;
effect::EffectHandler* ZombieUnit::sEffectHandler = 0;
BloodParticlesQueue* ZombieUnit::sBloodQueue = 0;
HitInfo ZombieUnit::sLastHitInfo;
physics::DynamicWorld* ZombieUnit::sDynamicWorld = 0;


////////////////////////////////////////////////////////////////////////////////
ZombieUnit::ZombieUnit() :
    WorldObject()
,   mFSM(sTTable)
,   mVelocity(15.f)
,   mInitialLife(100)
,   mLife(100)
{
    mFSM.setRef(this);
}

////////////////////////////////////////////////////////////////////////////////
ZombieUnit::~ZombieUnit()
{
    // delete the associated shape if we have one
    if (mPhysicBB.shape()) {
        ASSERT(sDynamicWorld);
        sDynamicWorld->removeObject(mPhysicBB);
        delete mPhysicBB.shape();
    }
    debugWARNING("we should remove the scenenode and entity here?\n");
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieUnit::configure(Ogre::SceneNode* node, Ogre::Entity* entity)
{
    ASSERT(node && "Invalid node");
    ASSERT(entity && "Invalid entity");

    // check if the entity is already attached
    if (entity->getParentNode()) {
//        ASSERT(entity->getParentNode() == node);
    } else {
        // attach it
        node->attachObject(entity);
    }

    setOgreStuff(node, entity);

    // set the pointers for the collision object and the OgreAny
    mCollObj->setUserDef(this);
    Ogre::UserObjectBindings& binding = entity->getUserObjectBindings();
    binding.setUserAny(Ogre::Any(this));

    // configure the animation table
    mAnimTable.loadAnims(ZA_ANIMS, entity);

    // configure the masks for raytracing and collisions
    entity->setQueryFlags(CZRayMask::CZRM_ZOMBIE);
    setCollMask(~0); // we want to collide with everything

    // configure the size of the coll object
    const Ogre::AxisAlignedBox& ogreBB = entity->getBoundingBox();
    const Ogre::Vector3 ogreBBSize = ogreBB.getHalfSize();
    const core::AABB bb(ogreBBSize.y, -ogreBBSize.x, -ogreBBSize.y, ogreBBSize.x);
    setCollisionInformation(0, bb);
    setCollisionType(false); // wow, this is rare XD, probably using an enum will be
                             // more easy to understand
    enableCollisions(false); // disable the collisions for now

    // create the physic representation of the zombie, for now we will use a capsule
    ASSERT(sDynamicWorld);
    if (mPhysicBB.shape()) {
        // destroy the current one
        sDynamicWorld->removeObject(mPhysicBB);
        delete mPhysicBB.shape();
    }

    mPhysicBB.setShape(physics::BulletImporter::createBoxShape(ogreBB));
    ASSERT(mPhysicBB.shape() && "Error creating the shape?");
    sDynamicWorld->addObject(mPhysicBB, CZRayMask::CZRM_ZOMBIE, 0);
//    prim = core::PrimitiveDrawer::instance().createBox(Ogre::Vector3::ZERO, ogreBB.getSize());


    // set the radius of the pathHandler
    mPathHandler.setRadius(std::sqrt(sqrRadius()));

    // configure the body here
    mBody.setEntity(entity);
    mBody.setSceneNode(node);
    mBody.setSkeleton(entity->getSkeleton());
    mBody.build();

    // TODO: load sounds here
    debugRED("TODO: \n\t* load sounds here... \n");

}

////////////////////////////////////////////////////////////////////////////////
void
ZombieUnit::reset(void)
{
    mLife = mInitialLife;
    mBody.resetBody();
    debugERROR("TODO\n");
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieUnit::born(void)
{
    debugERROR("TODO\n");

    // enable the collision
    enableCollisions(true);

    // reset the fsm
    mFSM.reset();
    mFSM.start();
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieUnit::dead(void)
{
    debugERROR("TODO\n");
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieUnit::processImpactInfo(const HitInfo& hitInfo)
{
    ASSERT(hitInfo.hasImpact);
    ASSERT(hitInfo.power > 0.f);

    // we will just save the hit information and analyze everything in the
    // BeingHit state
    sLastHitInfo = hitInfo;

    mFSM.newEvent(ZombieEvent::ZE_BEING_HIT);
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieUnit::configureRagdollVelocity(void)
{
    // we will apply the same force to all the body parts of the ragdoll
    //

    // calculate the force
    Ogre::Vector3 force(getNormalizedDir().x, getNormalizedDir().y, 0);
    force *= velocity();
    for (unsigned int i = 0; i < ZombieBody::BodyPart::BP_MAX; ++i) {
        mBody.applyForce(force, ZombieBody::BodyPart(i));
    }
}


} /* namespace cz */
