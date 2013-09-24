/*
 * RagDoll.h
 *
 *  Created on: Sep 12, 2013
 *      Author: agustin
 */

#ifndef RAGDOLL_H_
#define RAGDOLL_H_


#include <OgreSceneNode.h>
#include <OgreSkeletonInstance.h>
#include <OgreBone.h>

#include <bullet/btBulletDynamicsCommon.h>

#include <types/StackVector.h>
#include <debug/DebugUtil.h>


#ifdef DEBUG
#include <debug/PrimitiveDrawer.h>
#endif


namespace physics {


// Define the body parts ID (indices)
//
enum BodyPartID {
    BP_HEAD = 0,
    BP_UPPER_ARM_L,
    BP_FORE_ARM_L,
    BP_UPPER_ARM_R,
    BP_FORE_ARM_R,
    BP_SPINE,
    BP_PELVIS,
    BP_THIGH_L,
    BP_LEG_L,
    BP_THIGH_R,
    BP_LEG_R,

    BP_MAX
};

// The bones needed to construct the skeleton.
//
enum BonesID {
    B_HEAD = 0,
    B_NECK,
    B_CLAVICLE_R,
    B_CLAVICLE_L,
    B_UPPER_ARM_R,
    B_UPPER_ARM_L,
    B_FORE_ARM_R,
    B_FORE_ARM_L,
    B_HAND_R,
    B_HAND_L,
    B_SPINE,
    B_PELVIS,
    B_THIGH_R,
    B_THIGH_L,
    B_CALF_R,
    B_CALF_L,
    B_FOOT_R,
    B_FOOT_L,

    // this will be the completely unused bones
//    B_SPINE1,
//    B_HEAD_NUB,
//    B_BIP01_ROOT,
//    B_TOE0_L,
//    B_TOE0_NUB_L,
//    B_TOE0_R,
//    B_TOE0_NUB_R,

    B_COUNT
};

typedef core::StackVector<Ogre::Bone*, BonesID::B_COUNT> BoneTable;


// @TODO we can remove this class update each bone later in some "update" method
//       to only calculate once the inverse position and translation of the
//       skeleton as well as the transform.
//
class RagDollUpdater : public btMotionState {
public:
    // Global variables for this class
    //
    btTransform transform;
    bool dirty;
#ifdef DEBUG
    core::Primitive* primitive;
#endif



    RagDollUpdater() :
        dirty(false)
#ifdef DEBUG
    ,   primitive(0)
#endif
    {}

    // @brief Used by bullet
    //
    virtual void
    getWorldTransform(btTransform &worldTrans) const
    {
        worldTrans = transform;
    }

    // @brief set the world transform
    //
    virtual void
    setWorldTransform(const btTransform &worldTrans)
    {
        dirty = true;
        transform = worldTrans;

#ifdef DEBUG
        if (!primitive) {
            return;
        }
        const btQuaternion& rot = worldTrans.getRotation();
        primitive->node->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
        const btVector3& pos = worldTrans.getOrigin();
        primitive->node->setPosition(pos.x(), pos.y(), pos.z());
#endif
    }
};


class RagDoll
{
    // Define the joints
    //
    enum BodyConstraints {
        BC_SPINE_HEAD = 0,
        BC_SPINE_PELVIS,
        BC_LEFT_SHOULDER,
        BC_LEFT_ELBOW,
        BC_RIGHT_SHOULDER,
        BC_RIGHT_ELBOW,
        BC_LEFT_HIP,
        BC_LEFT_KNEE,
        BC_RIGHT_HIP,
        BC_RIGHT_KNEE,

        BC_COUNT,
    };

public:

    // Define the list of OBB information
    //
    struct OBBInfo {
        btTransform worldTransform;
        btVector3 extents;
    };

    typedef core::StackVector<OBBInfo, BodyPartID::BP_MAX> OBBInfoVec;

public:
    // set the world?
    RagDoll(btDynamicsWorld* bdw = 0);
    ~RagDoll();

    // @brief Set / get the Dynamic world used to construct the ragdoll
    // @param bdw       The bullet dynamic world
    //
    inline void
    setDynamicWorld(btDynamicsWorld* bdw);
    inline btDynamicsWorld*
    dynamicWorld() const;

    // @brief Construct the rigid body from an ogre bone table.
    //        The idea is to share the RagdollInstance for different skeletons
    //        and link the ragdoll with an skeleton in runtime.
    //        This method will create the ragdoll structure for this skeleton also
    //        but will not be enabled until you want to.
    // @param ogreBones         The ogre bone table
    // @param parentNode        The parent scene node.
    // @return true on success | false otherwise
    //
    bool
    buildFromSkeleton(const BoneTable& ogreBones,
                      Ogre::SceneNode* parentNode);

    // @brief Configure this ragdoll to be used with an specific list of bones.
    //        This bones should be sorted in the same order than the above enum.
    // @param bones         The bones to be linked against this ragdoll
    // @param parentNode    The parence scene node of the skeleton
    //
    void
    configureRagdoll(const BoneTable& bones,
                     Ogre::SceneNode* parentNode);

    // @brief Reset the original position and orientation of the bones of an
    //        skeleton to the main state.
    //        Note that the BoneTable should belong to an entity with the same
    //        skeleton than this.
    // @param boneTable     The bone table to reset its position.
    //
    void
    resetBones(BoneTable& bones);

    // @brief Enable / disable this ragdoll to start modifying the current
    //        linked bones.
    //        This will mark all the bones as manuallyModificable and insert
    //        all the rigid bodies into the dynamic world.
    //        If we are disabling this then we mark the bones as not manually
    //        modifiable and remove all the rigid body from the collisions world.
    // @param enable        We should mark as enabled? disabled?
    //
    void
    setEnable(bool enable);

    // @brief Check if this ragdoll is enabled or not
    //
    inline bool
    isEnabled(void) const;

    // @brief Remove all the data from this class and free the memory.
    //
    void
    clear(void);

    // @brief Get an specific body part information.
    // @param bpIndex       The body part index.
    // @return the associated rigid body part.
    //
    inline btRigidBody*
    getRigidBody(BodyPartID bpIndex);
    inline const btRigidBody*
    getRigidBody(BodyPartID bpIndex) const;

    // @brief Return the OBB for each body part in form of boxes. Each entry
    //        will contain the world btTransform of the box, and the extents
    //        of the OBB from a given BoneTable.
    // @param bones         The bones associated to the skeleton instance
    // @param parentNode    The parent scene node.
    // @param obbs          The OBBInfoVec for each body part.
    //
    void
    getBodyPartsOBB(const BoneTable& bones,
                    const Ogre::SceneNode* parentNode,
                    OBBInfoVec& obbs) const;

    // @brief Check the closest body part that intersects against a ray.
    //        Probably this should be removed from this class since is not
    //        responsibility of the ragdoll to check raycasting intersections
    //        (return the list of OBB of each body part and test outside,
    //         issue #0123)
    // @param rOrigin       The ray origin
    // @param rDir          The ray direction (not need to be normalized).
    // @param bpIntersected The closest BodyPartID that intersects against the ray.
    // @return true if there are intersections | false otherwise
    //
    bool
    getClosestIntersection(const Ogre::Vector3& rOrigin,
                           const Ogre::Vector3& rDir,
                           BodyPartID& bpIntersected) const;

    // @brief Update the ragdoll. This method will update the associated skeleton
    //        using the current position of the ragdoll.
    // @return true if the ragdoll should need to be updated anymore | false if not.
    //
    bool
    update(void);


private:

    typedef core::StackVector<unsigned short, 3> AdditionalOffsets;

    // Helper internal classes
    //
    struct RagdollBoneInfo {
        btBoxShape* shape;
        btRigidBody* rigidBody;
        Ogre::Bone* bone;
        btVector3 offset;
        RagDollUpdater motionState;
        AdditionalOffsets childJoints;
        BonesID boneID; // for the bone table
    };

    struct BoneChildOffset {
        Ogre::Bone* bone;
        btVector3 offset;

        BoneChildOffset(Ogre::Bone* b, const btVector3& o) : bone(b), offset(o){}
        BoneChildOffset() : bone(0) {};
    };

    struct TempBoneInfo {
        btVector3 worldPos;
        btQuaternion worldRot;
        float mass;
        Ogre::Bone* bone;

        // @brief helper function to transform a world position into a local
        //        position of this node
        btVector3
        toLocal(const btVector3& aWorldPos) const
        {
            btTransform t;
            t.setIdentity();
            t.setOrigin(worldPos);
            t.setRotation(worldRot);
            return t.inverse() * aWorldPos;
        }
    };

    typedef core::StackVector<TempBoneInfo, B_COUNT> TempBoneInfoVec;

    // Structure used to contain the initial state (orientation / rotation) of
    // the bones of the skeleton
    //
    struct InitialStateBoneInfo {
        Ogre::Vector3 position;
        Ogre::Quaternion rotation;

        InitialStateBoneInfo(const Ogre::Vector3& p, const Ogre::Quaternion& q) :
            position(p), rotation(q)
        {}
        InitialStateBoneInfo() {};
    };

    typedef core::StackVector<InitialStateBoneInfo, B_COUNT> InitialStateBoneInfoVec;

    // @brief Construct a RagdollBoneInfo from a position, orientation and box
    //        half sizes. We will also address of the boneInfo.motionState
    //        as the motion state pointer to be used.
    // @param position      The position of the shape.
    // @param orientation   The rotation / orientation of the shape.
    // @param halfSize      The half size of the box to be used.
    // @param mass          The mass of the rigid body.
    // @param boneInfo      The bone info structure to be used.
    //
    inline void
    constructBodyPart(const btVector3& position,
                      const btQuaternion& orientation,
                      const btVector3& halfSize,
                      float mass,
                      RagdollBoneInfo& boneInfo);

    // @brief Create the RagdollBoneInfo strcutcure with the temporary information
    //        and save it in the vector.
    // @param boneInfo        The bone information
    // @return true on succes | false otherwise
    //
    bool
    buildRagdollBoneInfo(const TempBoneInfoVec& boneInfo);

    // @brief Helper method used to configure the constraints in the already
    //        set shapes and rigid bodies.
    //        We will use the bones information to configure this also
    //
    bool
    setConstraints(const TempBoneInfoVec& boneInfo);

#ifdef DEBUG
    // @brief Create the bb for the boxes created for the ragdoll.
    //
    void
    createPrimitives(void);
#endif


private:
    btDynamicsWorld* mDynamicWorld;
    core::StackVector<RagdollBoneInfo, BodyPartID::BP_MAX> mRagdollBones;
    core::StackVector<btTypedConstraint*, BC_COUNT> mConstraints;
    InitialStateBoneInfoVec mInitialInfo;
    // in the mAdditionalOffsets vector we will put the child offset for some
    // of the bones that are not being tracked by the physics engine but are
    // child of some other bone that it is tracked.
    // This offsets will be relative to the rigid body position and not to the
    // bone position.
    core::StackVector<BoneChildOffset, BonesID::B_COUNT - BP_MAX> mAdditionalOffsets;
    Ogre::SceneNode* mParentNode;
    btVector3 mParentLastPos;
    bool mEnabled;

};





////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//


inline void
RagDoll::setDynamicWorld(btDynamicsWorld* bdw)
{
    mDynamicWorld = bdw;
}
inline btDynamicsWorld*
RagDoll::dynamicWorld() const
{
    return mDynamicWorld;
}

inline bool
RagDoll::isEnabled(void) const
{
    return mEnabled;
}

inline btRigidBody*
RagDoll::getRigidBody(BodyPartID bpIndex)
{
    return mRagdollBones[bpIndex].rigidBody;
}
inline const btRigidBody*
RagDoll::getRigidBody(BodyPartID bpIndex) const
{
    return mRagdollBones[bpIndex].rigidBody;
}

} /* namespace physics */
#endif /* RAGDOLL_H_ */
