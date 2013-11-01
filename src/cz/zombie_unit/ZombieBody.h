/*
 * ZombieBody.h
 *
 *  Created on: Oct 22, 2013
 *      Author: agustin
 */

#ifndef ZOMBIEBODY_H_
#define ZOMBIEBODY_H_

#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreSkeletonInstance.h>
#include <OgreSubEntity.h>

#include <debug/DebugUtil.h>
#include <physics/BulletObject.h>
#include <physics/RagDoll.h>

#include "RagDollQueue.h"
#include "BodyPartQueue.h"

namespace cz {

// @brief This class will be used to handle the skeleton behavior and body parts
//        of the zombies.
//        This include:
//          - Have an instance of the associated RagDoll.
//          - Check what is the part that a shoot is being hit.
//          - Hide / show different subentities whenever its needed.
//

class ZombieBody
{
public:
    // Define the number of body parts we have
    static const unsigned int BP_COUNT = physics::BodyPartID::BP_MAX;

    typedef physics::BodyPartID     BodyPart;

public:
    ZombieBody();
    ~ZombieBody();

    // @brief Set the RagdollQueue to be used for this instance.
    // @param rq        The ragdoll queue.
    //
    inline void
    setRagDollQueue(RagDollQueue<>* rq);
    inline RagDollQueue<>*
    ragDollQueue(void);

    // @brief Set the BodyPart queue used by this body.
    // @param bpq       The body part queue
    //
    inline void
    setBodyPartQueue(BodyPartQueue* bpq);
    inline BodyPartQueue*
    bodyPartQueue(void);

    // @brief Set the skeleton / entity instance associated to this Body.
    // @param skeleton      The skeleton instance.
    //
    inline void
    setSkeleton(Ogre::Skeleton* skeleton);
    inline Ogre::Skeleton*
    skeleton(void);
    inline void
    setEntity(Ogre::Entity* entity);
    inline Ogre::Entity*
    entity(void);

    // @brief Set the scene node associated to the zombie and where the skeleton
    //        is attached to.
    // @param node      The scene node.
    //
    inline void
    setSceneNode(Ogre::SceneNode* node);
    inline Ogre::SceneNode*
    sceneNode(void);

    ////////////////////////////////////////////////////////////////////////////

    // @brief This method will build the current body. You should call this method
    //        before use the class.
    //
    void
    build(void);

    // @brief Reset the current body to the initial state (show all the parts,
    //        reset the position of the bones, etc).
    //
    void
    resetBody(void);

    ////////////////////////////////////////////////////////////////////////////

    // @brief Show / Hide a particular body part and its children if necessary,
    //        For example if we want to hide the UPPER_ARM then we need to hide
    //        also the LOWER_ARM as well as we need to show some of the internal
    //        subentities.
    // @param bodyPart      The body part we want to "extirpate" from the zombie.
    // @return the associated BulletObject already rotated and in the same position
    //         than the bodyPart we are extirpating. This represent the graphic
    //         BodyPart we need to animate.
    //         0 on error or if not possible
    // @note YOU SHOULDN'T REMOVE NOR FREE ANY MEMORY.
    //
    physics::BulletObject*
    extirpate(BodyPart bodyPart);

    // @brief Check if a ray intersects some of the body parts of the current
    //        zombie.
    // @param ray       The ray we will use, in this case, we will use a OgreRay.
    // @param bp        The body part we intersect (in case of intersection).
    // @param intPoint  The intersection point in world space coordinates (this
    //                  is the point where the ray intersects the associated
    //                  bodyPart).
    // @return true if there is some intersection | false otherwise
    //
    bool
    checkIntersection(const Ogre::Ray& ray,
                      Ogre::Vector3& intPoint,
                      BodyPart& bp) const;

    ////////////////////////////////////////////////////////////////////////////

    // @brief Start / Stop using the Ragdoll for this skeleton. Before calling this
    //        method (Start) be sure that all the animations of the entity are inactive.
    // @param enable        Set the ragdoll enable?
    // @note that after calling this method you have to call the update method
    //       each frame
    //
    inline void
    setRagdollEnable(bool enable);

    // @brief Check if the ragdoll is enable or not.
    //
    inline bool
    isRagdollEnabled(void) const;

    // @brief Method used to update the ragdoll once it is active, this method
    //        will return true if we need to continue update the ragdoll or false
    //        it is not necessary anymore.
    // @returns true if we have to countinue updating the ragdoll | false otherwise
    //          (i.e the ragdoll stop moving).
    //
    inline bool
    update(void);

private:
    // Helper class used to handle the extirpation table, this is what we should
    // show / hide whenever we extirpate a body part.
    //
    struct ExtirpationNode {
        // In this particular case
        Ogre::SubEntity* hidedPart;
        Ogre::SubEntity* normalPart;

        ExtirpationNode() : hidedPart(0), normalPart(0) {}

        inline void extirpate(void)
        {
            ASSERT(hidedPart); ASSERT(normalPart);
            hidedPart->setVisible(true);
            normalPart->setVisible(false);
        }
        inline void fullExtirtpation(void)
        {
            ASSERT(hidedPart); ASSERT(normalPart);
            hidedPart->setVisible(false);
            normalPart->setVisible(false);
        }
        inline void reset(void)
        {
            ASSERT(hidedPart); ASSERT(normalPart);
            hidedPart->setVisible(false);
            normalPart->setVisible(true);
        }
    };
    struct ExtirpationInfo {
        // Each Extirpation info will be associated to a list of subentities that
        // have to hide / show. In this particular case, the UpperArms are the
        // ones that have more extirpation nodes associated = 1 {UpperArm, LowerArm};
        BodyPart bp;
        ExtirpationNode node;
        core::StackVector<ExtirpationInfo*, 1> childs;
    };

    struct ExtirpationTable {
        // We have only a few extirpation parts
        // {UPPER_ARM_R, FORE_ARM_R, UPPER_ARM_L, FORE_ARM_L, HEAD}
        //
        static const unsigned int EXTIRPATION_COUNT = 5;

        ExtirpationInfo mTable[EXTIRPATION_COUNT];

        // @brief Build the table from an entity
        //
        void
        build(Ogre::Entity* entity);

        // @brief Reset the table
        //
        void
        reset(void);

        // @brief Extirpate a body part and return the bodyParts we will
        //        remove
        // @return true if some body part was extirpated | false otherwise
        //
        bool
        extirpate(BodyPart bp, physics::BodyPartMask& mask);
    };

private:
    // this could be static queues to be used for all the instances
    //
    RagDollQueue<>* mRagdollQueue;
    BodyPartQueue* mBodyPartQueue;

    Ogre::Skeleton* mSkeleton;
    Ogre::SceneNode* mNode;
    Ogre::Entity* mEntity;
    physics::BodyPartMask mBodyMask;
    physics::BoneTable mBoneTable;
    physics::RagDoll* mCurrentRagdoll;

    ExtirpationTable mExtirpationTable;
};




////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//


inline void
ZombieBody::setRagDollQueue(RagDollQueue<>* rq)
{
    ASSERT(rq);
    mRagdollQueue = rq;
}
inline RagDollQueue<>*
ZombieBody::ragDollQueue(void)
{
    return mRagdollQueue;
}

inline void
ZombieBody::setBodyPartQueue(BodyPartQueue* bpq)
{
    ASSERT(bpq);
    mBodyPartQueue = bpq;
}
inline BodyPartQueue*
ZombieBody::bodyPartQueue(void)
{
    return mBodyPartQueue;
}

inline void
ZombieBody::setSkeleton(Ogre::Skeleton* skeleton)
{
    ASSERT(skeleton);
    mSkeleton = skeleton;
}
inline Ogre::Skeleton*
ZombieBody::skeleton(void)
{
    return mSkeleton;
}
inline void
ZombieBody::setEntity(Ogre::Entity* entity)
{
    ASSERT(entity);
    mEntity = entity;
}
inline Ogre::Entity*
ZombieBody::entity(void)
{
    return mEntity;
}


inline void
ZombieBody::setSceneNode(Ogre::SceneNode* node)
{
    ASSERT(node);
    mNode = node;
}
inline Ogre::SceneNode*
ZombieBody::sceneNode(void)
{
    return mNode;
}

inline void
ZombieBody::setRagdollEnable(bool enable)
{
    if (enable) {
        if (!mCurrentRagdoll) {
            mCurrentRagdoll = mRagdollQueue->getNewOne();
            ASSERT(mCurrentRagdoll);
            ASSERT(mNode);
            mCurrentRagdoll->configureRagdoll(mBoneTable, mNode);
            mCurrentRagdoll->setEnable(true);
        }
    } else {
        if (mCurrentRagdoll) {
            mCurrentRagdoll->setEnable(false);
            mRagdollQueue->letAvailable(mCurrentRagdoll);
            mCurrentRagdoll = 0;
        }
    }
}

inline bool
ZombieBody::isRagdollEnabled(void) const
{
    return mCurrentRagdoll && mCurrentRagdoll->isEnabled();
}

inline bool
ZombieBody::ZombieBody::update(void)
{
    ASSERT(mCurrentRagdoll);
    return mCurrentRagdoll->update();
}

} /* namespace cz */
#endif /* ZOMBIEBODY_H_ */