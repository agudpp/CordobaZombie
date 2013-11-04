/*
 * ZombieUnit.cpp
 *
 *  Created on: Sep 9, 2013
 *      Author: agustin
 */

#include "ZombieUnit.h"

#include <cmath>

#include <physics/BulletObject.h>
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
        ASSERT(entity->getParentNode() == node);
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
bool
ZombieUnit::processImpactInfo(const HitInfo& hitInfo)
{
    ASSERT(false && "TODO");

    ASSERT(hitInfo.hasImpact);
    ASSERT(hitInfo.power > 0.f);

    // else, we had intersected the zombie body, now we have to check what
    // we have to do here depending which body part was intersected.
    // The common things are:
    //  - Subtract life
    //  - Play sound
    //  - Blood effect
    //
    ASSERT(sEffectHandler);

    mLife -= static_cast<short>(hitInfo.power);
    debugERROR("Play sound\n");

    // reproduce the blood effect here
    ASSERT(sBloodQueue);
    debugWARNING("check that blood effect is correct\n");
    BloodParticles* bloodParticles = sBloodQueue->getAvailable();
    if (bloodParticles) {
        bloodParticles->configure(hitInfo.intersectionPoint,
                                  hitInfo.ray.getDirection(),
                                  15.f);
        sEffectHandler->add(bloodParticles);
    }

    // we can check 2 differents groups here, the bodyparts that can be extirpated
    // and the bodyparts that cannot.
    //
    if (!mBody.canExtirpate(hitInfo.bodyPart)) {
        // this body part cannot be extirpated, check if we are still alive
        if (!hasLife()) {
            // we die, change to the die state
            mFSM.newEvent(ZombieEvent::ZE_DIE);
        } else {
            // else we didn't die, we have to perform the being hit state
            //
            mFSM.newEvent(ZombieEvent::ZE_BEING_HIT);
        }

        // nothing else to do here,
        return true;
    }

    // else, some of the parts can be extirpated. We will extirpate the part
    // and reproduce the "bodyPartEffect"
    //
    if (hitInfo.power >= EXTIRPATION_POWER_THRESHOLD) {
        // we need to extirpate the part
        BodyPartElement* bpelement = mBody.extirpate(hitInfo.bodyPart);

        // configure the force and direction associated to the bodyPart
        physics::BulletObject* bo = bpelement->bulletObject;
        ASSERT(bo);
        ASSERT(bo->rigidBody);
        Ogre::Vector3 force = hitInfo.ray.getDirection();
        force.normalise();
        force *= (hitInfo.power * 1.f / bo->rigidBody->getInvMass());
        bo->applyCentralForce(force);

        // configure the effect here and reproduce it
        mBodyPartEffect.setElement(bpelement);
        mBodyPartEffect.setQueue(mBody.bodyPartQueue());
        sEffectHandler->add(&mBodyPartEffect);

        debugERROR("Here we need to activate the bpelement and reproduce the "
            "effect setting the force of the element in the same direction than"
            " the bullet\n");
    }

    // if we hit the head or the zombie has die!!
    //
    if (hitInfo.bodyPart == ZombieBody::BodyPart::BP_HEAD || !hasLife()) {
        setLife(-1); // just in case we hit the head and still have life

        // configure the movement force of the ragdoll
        //
        mBody.setRagdollEnable(true);
        configureRagdollVelocity();

        // apply the force to the head
        Ogre::Vector3 force = hitInfo.ray.getDirection();
        force.normalise();
        ASSERT(mBody.currentRagDollInstance());
        // TODO: we are using bullet code directly here! WRONG, check issue 212
        //
        btRigidBody* rigidBody =
            mBody.currentRagDollInstance()->getRigidBody(hitInfo.bodyPart);
        ASSERT(rigidBody);

        force *= (velocity() + hitInfo.power * (1.f / rigidBody->getInvMass()));
        mBody.applyForce(force, hitInfo.bodyPart);

        // advise the FSM that we are dying
        mFSM.newEvent(ZombieEvent::ZE_DIE);
        return true;
    }

    // if not, we are not dying, we have nothing to do?
    debugWARNING("What whe have to do here? advise that we are being hit?\n")

    switch (hitInfo.bodyPart) {
    ////////////////////////////////////////////////////////////////////
    case ZombieBody::BodyPart::BP_HEAD:
        // the zombie should die right now!
        break;
    ////////////////////////////////////////////////////////////////////
    case ZombieBody::BodyPart::BP_UPPER_ARM_L:

        break;
    ////////////////////////////////////////////////////////////////////
    case ZombieBody::BodyPart::BP_FORE_ARM_L:

        break;
    ////////////////////////////////////////////////////////////////////
    case ZombieBody::BodyPart::BP_UPPER_ARM_R:

        break;
    ////////////////////////////////////////////////////////////////////
    case ZombieBody::BodyPart::BP_FORE_ARM_R:

        break;
    ////////////////////////////////////////////////////////////////////
    case ZombieBody::BodyPart::BP_SPINE:

        break;
    ////////////////////////////////////////////////////////////////////
    case ZombieBody::BodyPart::BP_PELVIS:

        break;
    ////////////////////////////////////////////////////////////////////
    case ZombieBody::BodyPart::BP_THIGH_L:

        break;
    ////////////////////////////////////////////////////////////////////
    case ZombieBody::BodyPart::BP_LEG_L:

        break;
    ////////////////////////////////////////////////////////////////////
    case ZombieBody::BodyPart::BP_THIGH_R:

        break;
    ////////////////////////////////////////////////////////////////////
    case ZombieBody::BodyPart::BP_LEG_R:

        break;
    }

    return true;
}

} /* namespace cz */
