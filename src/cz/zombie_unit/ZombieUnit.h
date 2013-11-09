/*
 * ZombieUnit.h
 *
 *  Created on: Sep 9, 2013
 *      Author: agustin
 */

#ifndef ZOMBIEUNIT_H_
#define ZOMBIEUNIT_H_

#include <OgreEntity.h>
#include <OgreAnimationState.h>
#include <OgreSceneNode.h>

#include <debug/DebugUtil.h>
#include <fx/effects/blood/BloodParticles.h>
#include <fx/effects/body_part/BodyPartEffect.h>
#include <fx/effects/EffectQueueDefs.h>
#include <global_data/GlobalData.h>
#include <helpers/AnimTable.h>
#include <ia/fsm/FSMTransitionTable.h>
#include <ia/path_handler/PathHandler.h>
#include <physics/BulletCollisionObject.h>
#include <world_object/WorldObject.h>

#include "states/ZombieFSMDefs.h"
#include "ZombieBody.h"
#include "RagDollQueue.h"
#include "HitInfo.h"



// forward
//
namespace effect {
class EffectHandler;
}

namespace physics {
class DynamicWorld;
}

namespace cz {

// forward
//
class BodyPartQueue;


// Some typedefs used by this class
//


class ZombieUnit : public WorldObject
{
    // @brief Here we will define some private defines to be used by this
    //        particular zombies.
    //
    static const unsigned int NUM_ANIMATIONS = 10;

    // Some defines
    //

public:

    // @brief Public information and defines
    //

    enum Anim {
        ZA_RUN1 = 0,
        ZA_RUN2,
        ZA_HIT_SHOULDER_R,
        ZA_HIT_SHOULDER_L,
        ZA_FALL,
        ZA_DRAG,
        ZA_DRAG_DIE,
        ZA_HEAD_DIE,
        ZA_NORMAL_DIE1,
        ZA_NORMAL_DIE2,

        ZA_ANIM_COUNT,
    };

public:
    ZombieUnit();
    ~ZombieUnit();

    // @brief Set the Dynamic world instance.
    // @param dwi       The dynamic world instance
    //
    static inline void
    setDynamicWorld(physics::DynamicWorld* dwi);
    static inline physics::DynamicWorld*
    dynamicWorld(void);

    // @brief We need to set the global EffectHandler to be used for all the
    //        effects of the zombies.
    // @param eh        The global effect handler
    //
    static inline void
    setEffectHandler(effect::EffectHandler* eh);
    static inline effect::EffectHandler*
    effectHandler(void);

    // @brief Set the Blood particle effect queue to be shared for all the
    //        zombies
    // @param bpq       Blood particle queue
    //
    static inline void
    setBloodParticlesQueue(BloodParticlesQueue* bpq);
    static inline BloodParticlesQueue*
    bloodParticlesQueue(void);

    // @brief Set the queues used by this zombie unit.
    // @param rq        The RagDollQueue instance
    // @param bpq       The BodyPartQueue instance
    //
    inline void
    setQueues(RagDollQueue<>* rq, BodyPartQueue* bpq);
    inline RagDollQueue<>*
    ragDollQueue(void);
    inline const RagDollQueue<>*
    ragDollQueue(void) const;
    inline BodyPartQueue*
    bodyPartQueue(void);
    inline const BodyPartQueue*
    bodyPartQueue(void) const;

    // @brief Return the BodyPartEffectQueue reference associated to this zombie
    // @return BodyPartsEffectQueue
    //
    inline BodyPartsEffectQueue&
    bodyPartsEffectQueue(void);
    inline const BodyPartsEffectQueue&
    bodyPartsEffectQueue(void) const;

    // @brief Set the associated body part ID to be used for this zombie.
    //
    inline void
    setBodyPartID(unsigned short id);
    inline unsigned short
    bodyPartID(void) const;

    // @brief Configure the Zombie unit from a given entity and a scene node.
    //        This method will configure the radius of the zombie, the collision
    //        object, the masks for collisions and raycasts, the animation state
    //        table, sound tables and everything else.
    // @param node      The scene node used for this zombie.
    // @param entity    The associated entity (already attached to the node, if
    //                  not we will automatically attach it).
    //
    void
    configure(Ogre::SceneNode* node, Ogre::Entity* entity);

    // @brief Set / get the velocity of the zombie to move.
    // @param vel       The movement velocity
    //
    inline void
    setVelocity(float vel);
    inline float
    velocity(void) const;

    // @brief Get / set the life of the zombie
    // @param life      The life of the zombie
    //
    inline void
    setLife(short life);
    inline short
    life(void) const;
    inline bool
    hasLife(void) const;

    // @brief Set the initial life to be used every time the zombie is reset
    // @param initialLife   The initial life to be used
    //
    inline void
    setInitialLife(short initialLife);
    inline short
    initialLife(void) const;

    ////////////////////////////////////////////////////////////////////////////

    // @brief This method will be called to initialize the zombie to let them as
    //        if it was just loaded from disk. This method should reinit all
    //        the animation states, show the zombie fully (for example if we
    //        hide some part of its body), reset the texture (if we changed it),
    //        etc. But it will do not show it in the world.
    //
    void
    reset(void);

    // @brief This method will be called when the zombie will appear in the world
    //        to start the logic and IA.
    //
    void
    born(void);

    // @brief This method will be called when the zombie dies, after his dead we
    //        need to move the zombie to a ZombieFactory and hide them from the
    //        scene, disable/remove from the collisions world, etc.
    //
    void
    dead(void);

    // @brief Check if this zombie is alive or not.
    // @return true if the zombie is alive | false otherwise
    //
    inline bool
    isAlive(void) const;

    ////////////////////////////////////////////////////////////////////////////

    // @brief Emit a new event (external event).
    // @param event     The external event
    //
    inline void
    newEvent(ZombieEvent event);

    ////////////////////////////////////////////////////////////////////////////

    // @brief Make the zombie move to a certain position (calculate the new path
    //        from the current position to the destination position).
    // @param dest      The destination position where we want to go.
    // @return true if there exists a path or false otherwise
    //
    inline bool
    moveTo(const core::Vector2& dest);

    // @brief Get the current PathHandler associated to this zombie instance.
    //
    inline const PathHandler&
    pathHandler(void) const;
    inline PathHandler&
    pathHandler(void);

    // @brief This method will translate the zombie and change its direction to
    //        where it is walking. Note that move vector is relative to the position
    //        of the unit.
    //        We will move the unit proportionally to the last time frame passed.
    // @param move          The move vector to be used.
    //
    inline void
    moveAndLook(const core::Vector2& move);

    ////////////////////////////////////////////////////////////////////////////

    // @brief Reproduce a new animation from the list of animations. This will
    //        start the animation from 0.
    // @param animID        The animation index (ID).
    //
    inline void
    startAnim(unsigned int animID);

    // @brief Check if an animation is already active.
    // @param animID        The animation index (ID).
    //
    inline bool
    isAnimActive(unsigned int animID);

    // @brief Stop a particular animation.
    // @param animID        The animation index (ID).
    //
    inline void
    stopAnim(unsigned int animID);

    // @brief Get a particular animation to be able to configure it
    // @param animID        The animation index (ID).
    // @return the associated animation state (ogre)
    //
    inline Ogre::AnimationState*
    animState(unsigned int animID);

    // @brief Stop all the animations.
    //
    inline void
    stopAllAnimations(void);

    ////////////////////////////////////////////////////////////////////////////

    // @brief Check for a possible impact. This method will only check if we
    //        impact (the zombie) against any bullet... We will not perform
    //        any other operation.
    // @param hitInfo       The hit information structure used to check the
    //                      impact and retrieve the information also. This
    //                      information will be used later if necessary to process
    //                      the ImpactInfo.
    // @return true if we had impact the zombie | false otherwise
    //
    inline bool
    checkImpact(HitInfo& hitInfo) const;

    // @brief The zombie was hit by a bullet or something (indicated in the
    //        HitInfo structure). This method will animate the zombie / hide
    //        the parts that were extirpated and create the body part to be
    //        animated in the physics world (we need to use the BodyPartsContainer
    //        and PhysicsEffectSystem).
    // @param hitInfo       The hit information structure. This structure should
    //                      be filled in the checkImpact() method.
    // @note that we will perform the effects here (blood + bodyPart).
    //
    void
    processImpactInfo(const HitInfo& hitInfo);

    // @brief Return the last hit information associated to this unit.
    // @return the reference to the HitInfo structure holding the last hitting
    //         information against this zombie
    //
    inline const HitInfo&
    lastHitInfo(void) const;

    // @brief This method will configure the ragdoll velocity depending on the
    //        current state of the zombie.
    //
    void
    configureRagdollVelocity(void);

    // @brief Set enable the ragdoll. This will stop all the animations and
    //        turn on the ragdoll.
    //
    inline void
    setRagDollEnabled(bool enabled);


    ////////////////////////////////////////////////////////////////////////////
    // Reproduce an specific sound


    ////////////////////////////////////////////////////////////////////////////
    // General "getters" methods
    //

    // @brief Return the ZombieBody instance of this class
    //
    inline ZombieBody&
    zombieBody(void);
    inline const ZombieBody&
    zombieBody(void) const;

    // @brief Return the FSM associated to this instance
    //
    inline ZombieFSM&
    FSM(void);
    inline const ZombieFSM&
    FSM(void) const;

    ////////////////////////////////////////////////////////////////////////////

    // @brief Update all the logic of the zombie. We will use the
    //        GlobalData::lastTimeFrame to calculate everything
    //
    inline void
    update(void);

protected:

protected:
    // static members
    // the table used for all the zombies of this type
    static ZombieTTable sTTable;
    // global blood queue
    static BloodParticlesQueue* sBloodQueue;
    // global body part effect to be used, this will be shared for all the zombies
    static BodyPartsEffectQueue sBodyPartEffectQueue;
    // global effect handler
    static effect::EffectHandler* sEffectHandler;
    // Last hit information, since we are not running in parallel we can do this
    // safely.
    static HitInfo sLastHitInfo;
    // Dynamic world instance
    static physics::DynamicWorld* sDynamicWorld;

protected:
    // General members
    AnimTable<NUM_ANIMATIONS> mAnimTable;
    ZombieFSM mFSM;
    PathHandler mPathHandler;
    float mVelocity;
    short mInitialLife;
    short mLife;
    // The zombie body instance associated to this zombie
    ZombieBody mBody;
    // The physic bounding box representation of the zombie to be able to receive
    // raycast and probably intersect against body parts
    physics::BulletCollisionObject mPhysicBB;

    core::Primitive* prim;


private:
    // Avoid copying this class
    ZombieUnit(const ZombieUnit&);
    ZombieUnit& operator=(const ZombieUnit&);

};






////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
ZombieUnit::setDynamicWorld(physics::DynamicWorld* dwi)
{
    ASSERT(dwi);
    sDynamicWorld = dwi;
}
inline physics::DynamicWorld*
ZombieUnit::dynamicWorld(void)
{
    return sDynamicWorld;
}

inline void
ZombieUnit::setEffectHandler(effect::EffectHandler* eh)
{
    ASSERT(eh);
    sEffectHandler = eh;
}
inline effect::EffectHandler*
ZombieUnit::effectHandler(void)
{
    return sEffectHandler;
}

inline void
ZombieUnit::setBloodParticlesQueue(BloodParticlesQueue* bpq)
{
    ASSERT(bpq);
    sBloodQueue = bpq;
}
inline BloodParticlesQueue*
ZombieUnit::bloodParticlesQueue(void)
{
    return sBloodQueue;
}


inline void
ZombieUnit::setQueues(RagDollQueue<>* rq, BodyPartQueue* bpq)
{
    mBody.setRagDollQueue(rq);
    mBody.setBodyPartQueue(bpq);
}
inline RagDollQueue<>*
ZombieUnit::ragDollQueue(void)
{
    return mBody.ragDollQueue();
}
inline const RagDollQueue<>*
ZombieUnit::ragDollQueue(void) const
{
    return mBody.ragDollQueue();
}
inline BodyPartQueue*
ZombieUnit::bodyPartQueue(void)
{
    return mBody.bodyPartQueue();
}
inline const BodyPartQueue*
ZombieUnit::bodyPartQueue(void) const
{
    return mBody.bodyPartQueue();
}

inline BodyPartsEffectQueue&
ZombieUnit::bodyPartsEffectQueue(void)
{
    return sBodyPartEffectQueue;
}
inline const BodyPartsEffectQueue&
ZombieUnit::bodyPartsEffectQueue(void) const
{
    return sBodyPartEffectQueue;
}

inline void
ZombieUnit::setBodyPartID(unsigned short id)
{
    mBody.setBodyPartID(id);
}
inline unsigned short
ZombieUnit::bodyPartID(void) const
{
    return mBody.bodyPartID();
}

inline void
ZombieUnit::setVelocity(float vel)
{
    ASSERT(vel > 0);
    mVelocity = vel;
}
inline float
ZombieUnit::velocity(void) const
{
    return mVelocity;
}

////////////////////////////////////////////////////////////////////////////
inline void
ZombieUnit::setLife(short life)
{
    mLife = life;
}
inline short
ZombieUnit::life(void) const
{
    return mLife;
}
inline bool
ZombieUnit::hasLife(void) const
{
    return mLife > 0;
}

////////////////////////////////////////////////////////////////////////////
inline void
ZombieUnit::setInitialLife(short initialLife)
{
    mInitialLife = initialLife;
}
inline short
ZombieUnit::initialLife(void) const
{
    return mInitialLife;
}

////////////////////////////////////////////////////////////////////////////
inline bool
ZombieUnit::isAlive(void) const
{
    return isInCollisionWorld() && hasCollisionsEnabled();
}

////////////////////////////////////////////////////////////////////////////
inline void
ZombieUnit::newEvent(ZombieEvent event)
{
    mFSM.newEvent(event);
}

////////////////////////////////////////////////////////////////////////////

inline bool
ZombieUnit::moveTo(const core::Vector2& dest)
{
    return mPathHandler.calculatePath(position(), dest);
}

inline const PathHandler&
ZombieUnit::pathHandler(void) const
{
    return mPathHandler;
}
inline PathHandler&
ZombieUnit::pathHandler(void)
{
    return mPathHandler;
}

inline void
ZombieUnit::moveAndLook(const core::Vector2& move)
{
    core::Vector2 tvec(move);
    tvec *= GlobalData::lastTimeFrame;
    translate(tvec * velocity());
    lookAt(position() + tvec);
}

////////////////////////////////////////////////////////////////////////////

inline void
ZombieUnit::startAnim(unsigned int animID)
{
    mAnimTable.startAnim(animID);
}

inline bool
ZombieUnit::isAnimActive(unsigned int animID)
{
    return mAnimTable.isAnimStarted(animID);
}

inline void
ZombieUnit::stopAnim(unsigned int animID)
{
    mAnimTable.stopAnim(animID);
}

inline Ogre::AnimationState*
ZombieUnit::animState(unsigned int animID)
{
    return mAnimTable[animID];
}

inline void
ZombieUnit::stopAllAnimations(void)
{
    mAnimTable.stopAll();
}

////////////////////////////////////////////////////////////////////////////

inline bool
ZombieUnit::checkImpact(HitInfo& hitInfo) const
{
    ZombieBody::BodyPart bp;
    hitInfo.hasImpact = mBody.checkIntersection(hitInfo.ray,
                                                hitInfo.intersectionPoint,
                                                bp);
    hitInfo.bodyPart = bp;
    return hitInfo.hasImpact;
}

////////////////////////////////////////////////////////////////////////////

inline const HitInfo&
ZombieUnit::lastHitInfo(void) const
{
    return sLastHitInfo;
}

inline void
ZombieUnit::setRagDollEnabled(bool enabled)
{
    if (enabled) {
        mAnimTable.stopAll();
    }
    mBody.setRagdollEnable(enabled);
}

////////////////////////////////////////////////////////////////////////////

inline ZombieBody&
ZombieUnit::zombieBody(void)
{
    return mBody;
}
inline const ZombieBody&
ZombieUnit::zombieBody(void) const
{
    return mBody;
}
inline ZombieFSM&
ZombieUnit::FSM(void)
{
    return mFSM;
}
inline const ZombieFSM&
ZombieUnit::FSM(void) const
{
    return mFSM;
}

////////////////////////////////////////////////////////////////////////////

inline void
ZombieUnit::update(void)
{
    // first check if we have to update the physics bounding box
    if (hasMovedOrRotated()) {
//        Ogre::Vector3 position(position3D());
//        position.z += objectHeight();
//        mPhysicBB.setTransform(position, rotation());
//        prim->node->setPosition(position);
//        prim->node->setOrientation(rotation());
        resetMovedOrRotatedFlag();
    }

    // update everything
    mFSM.update();
    mAnimTable.update(GlobalData::lastTimeFrame);
}

} /* namespace cz */
#endif /* ZOMBIEUNIT_H_ */
