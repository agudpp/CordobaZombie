/*
 * ZSBeingHit.cpp
 *
 *  Created on: Nov 5, 2013
 *      Author: agustin
 */

#include "ZSBeingHit.h"

#include <zombie_unit/HitInfo.h>
#include <zombie_unit/ZombieBody.h>
#include <debug/DebugUtil.h>
#include <effect_handler/EffectHandler.h>
#include <physics/BulletObject.h>
#include <fx/effects/body_part/BodyPartEffect.h>
#include <fx/effects/blood/BloodParticles.h>
#include <zombie_unit/ZombieUnit.h>



namespace cz {

////////////////////////////////////////////////////////////////////////////////
ZSBeingHit::ZSBeingHit()
{

}

////////////////////////////////////////////////////////////////////////////////
ZSBeingHit::~ZSBeingHit()
{

}

////////////////////////////////////////////////////////////////////////////////
void
ZSBeingHit::enter(ZombieUnit* ref)
{
    ASSERT(ref);
    // analyze where we had being hit
    //
    const HitInfo& hitInfo = ref->lastHitInfo();

    // else, we had intersected the zombie body, now we have to check what
    // we have to do here depending which body part was intersected.
    // The common things are:
    //  - Subtract life
    //  - Play sound
    //  - Blood effect
    //
    effect::EffectHandler* effectHandler = ref->effectHandler();
    ASSERT(effectHandler);

    ref->setLife(ref->life() - static_cast<short>(hitInfo.power));
    debugERROR("Play sound\n");

    // reproduce the blood effect here
    BloodParticlesQueue* bloodQueue = ref->bloodParticlesQueue();
    ASSERT(bloodQueue);
    debugWARNING("check that blood effect is correct\n");
    BloodParticles* bloodParticles = bloodQueue->getAvailable();
    if (bloodParticles) {
        bloodParticles->configure(hitInfo.intersectionPoint,
                                  hitInfo.ray.getDirection(),
                                  15.f);
        effectHandler->add(bloodParticles);
    }

    ZombieBody& body = ref->zombieBody();

    // we can check 2 different groups here, the bodyparts that can be extirpated
    // and the bodyparts that cannot.
    //
//    if (!body.canExtirpate(ZombieBody::BodyPart(hitInfo.bodyPart))) {
//        // this body part cannot be extirpated, check if we are still alive
//        if (!ref->hasLife()) {
//            // we die, change to the die state
//            ref->newEvent(ZombieEvent::ZE_DIE);
//            ref->setRagDollEnabled(true);
//            ref->configureRagdollVelocity();
//            return;
//        }
//    }

    // else, some of the parts can be extirpated. We will extirpate the part
    // and reproduce the "bodyPartEffect"
    //
    if ((hitInfo.power >= EXTIRPATION_POWER_THRESHOLD) &&
            body.canExtirpate(ZombieBody::BodyPart(hitInfo.bodyPart))) {
        // we need to extirpate the part
        BodyPartElement* bpelement = body.extirpate(ZombieBody::BodyPart(hitInfo.bodyPart));

        if (bpelement) {
            // configure the force and direction associated to the bodyPart
            physics::BulletObject* bo = bpelement->bulletObject;
            ASSERT(bo);
            ASSERT(bo->rigidBody);
            Ogre::Vector3 force = hitInfo.ray.getDirection();
            force.normalise();
            force *= (hitInfo.power * 1.f / bo->rigidBody->getInvMass());
            bo->applyCentralForce(force);

            // configure the effect here and reproduce it
            BodyPartEffect& bodyPartEffect = ref->bodyPartEffect();
            bodyPartEffect.setElement(bpelement);
            bodyPartEffect.setQueue(body.bodyPartQueue());
            effectHandler->add(&bodyPartEffect);
        }
    }

    // if we hit the head or the zombie has die!!
    //
    if (hitInfo.bodyPart == ZombieBody::BodyPart::BP_HEAD || !ref->hasLife()) {
        ref->setLife(-1); // just in case we hit the head and still have life

        // configure the movement force of the ragdoll
        //
        ref->setRagDollEnabled(true);
        ref->configureRagdollVelocity();

        // apply the force to the head
        Ogre::Vector3 force = hitInfo.ray.getDirection();
        force.normalise();
        ASSERT(body.currentRagDollInstance());
        // TODO: we are using bullet code directly here! WRONG, check issue 212
        //
        btRigidBody* rigidBody =
            body.currentRagDollInstance()->getRigidBody(
                ZombieBody::BodyPart(hitInfo.bodyPart));
        ASSERT(rigidBody);

        force *= (ref->velocity() + hitInfo.power * (1.f / rigidBody->getInvMass()));
        body.applyForce(force, ZombieBody::BodyPart(hitInfo.bodyPart));

        // advise the FSM that we are dying
        ref->newEvent(ZombieEvent::ZE_DIE);
        return;
    }

    // if not, we are not dying, we have nothing to do?
    debugWARNING("Here we will just play an animation randomly (hitting animation)"
        " but probably this is not what we want (we want to be able to continue"
        " moving the zombie forward)\n");

    mRunningAnim = ZombieUnit::Anim::ZA_ANIM_COUNT;


    switch (hitInfo.bodyPart) {
    ////////////////////////////////////////////////////////////////////
    case ZombieBody::BodyPart::BP_HEAD:
    case ZombieBody::BodyPart::BP_UPPER_ARM_L:
    case ZombieBody::BodyPart::BP_FORE_ARM_L:
    case ZombieBody::BodyPart::BP_THIGH_L:
    case ZombieBody::BodyPart::BP_LEG_L:
        mRunningAnim = ZombieUnit::Anim::ZA_HIT_SHOULDER_L;

        break;
    ////////////////////////////////////////////////////////////////////
    case ZombieBody::BodyPart::BP_UPPER_ARM_R:
    case ZombieBody::BodyPart::BP_FORE_ARM_R:
    case ZombieBody::BodyPart::BP_SPINE:
    case ZombieBody::BodyPart::BP_PELVIS:
    case ZombieBody::BodyPart::BP_THIGH_R:
    case ZombieBody::BodyPart::BP_LEG_R:
        mRunningAnim = ZombieUnit::Anim::ZA_HIT_SHOULDER_R;
        break;
    default:
        ASSERT(false && "Invalid bodypart!");
    }
    ASSERT(mRunningAnim != ZombieUnit::Anim::ZA_ANIM_COUNT);
    ref->startAnim(mRunningAnim);
}

////////////////////////////////////////////////////////////////////////////////
void
ZSBeingHit::update(ZombieUnit* ref)
{
    ASSERT(ref);

    // wait until the animation ends...
    ASSERT(mRunningAnim != ZombieUnit::Anim::ZA_ANIM_COUNT);
    if (!ref->isAnimActive(mRunningAnim)) {
        ref->newEvent(ZombieEvent::ZE_DONE);
    }

    // we need to wait
}

////////////////////////////////////////////////////////////////////////////////
void
ZSBeingHit::exit(ZombieUnit* ref)
{
    ASSERT(ref);

    // stop the animation
    if (mRunningAnim != ZombieUnit::Anim::ZA_ANIM_COUNT) {
        ref->stopAnim(mRunningAnim);
    }
}

} /* namespace cz */
