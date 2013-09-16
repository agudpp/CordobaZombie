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

    B_COUNT
};

// The structure used to map bones -> BodyPart
struct BodyPartInfo {
    std::string rootBoneName;     // The root bone that represent the bodypart
    std::string childBoneName;    // The associated child bone
    float height;                 // The height of the box representing the bp
    float width;                  // The width of the box representing the bp
    float mass;                   // The associated mass to this bodypart

    BodyPartInfo(const std::string& rootB,
                 const std::string& childB,
                 float h,
                 float w,
                 float m = 2.f) :
        rootBoneName(rootB)
    ,   childBoneName(childB)
    ,   height(h)
    ,   width(w)
    ,   mass(m)
    {}

    BodyPartInfo()
    {}
};

typedef core::StackVector<BodyPartInfo, BP_MAX> BodyPartInfoVec;
typedef core::StackVector<Ogre::Bone*, BonesID::B_COUNT> BoneTable;


// @TODO we can remove this class update each bone later in some "update" method
//       to only calculate once the inverse position and translation of the
//       skeleton as well as the transform.
//
class RagDollUpdater : public btMotionState {
public:

    // Global variables for this class
    //
    Ogre::Bone *bone;
    Ogre::SceneNode* parentNode;
    btTransform transform;
    btVector3 offset;


    RagDollUpdater() : bone(0), parentNode(0) {}

    // @brief reset the values
    //
    inline void
    reset(void)
    {
        bone = 0; parentNode = 0;
    }

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
        /*ASSERT(bone);
        ASSERT(parentNode);
        // TODO: here we need to transform from world to local.
        //
        const btQuaternion& rot = worldTrans.getRotation();
        bone->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
        const btVector3& pos = worldTrans.getOrigin();
        bone->setPosition(pos.x(), pos.y(), pos.z());
        transform = worldTrans;*/

#ifdef DEBUG
        const btQuaternion& rot = worldTrans.getRotation();
        primitive->node->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
        const btVector3& pos = worldTrans.getOrigin();
        primitive->node->setPosition(pos.x(), pos.y(), pos.z());
#endif
    }

    // @brief get the global position/orientation for this particular object
    //
    inline Ogre::Vector3
    worldPosition(void)
    {
        return globalOgrePosition(bone, parentNode);
    }
    inline Ogre::Quaternion
    worldOrientation(void)
    {
        return globalOgreRotation(bone, parentNode);
    }

    // Helper methods
    //
    static inline Ogre::Quaternion
    globalOgreRotation(const Ogre::Bone *bone, const Ogre::SceneNode* parentNode)
    {
        return parentNode->_getDerivedOrientation() * bone->_getDerivedOrientation();
    }
    static inline Ogre::Vector3
    globalOgrePosition(const Ogre::Bone *bone, const Ogre::SceneNode* parentNode)
    {
        return parentNode->getPosition() + parentNode->getOrientation() *
            bone->_getDerivedPosition();
    }

#ifdef DEBUG
public:
    core::Primitive* primitive;
#endif

};


class RagDoll
{
    // Define the joints
    //
    enum BodyConstraints {
        BC_SPINE_HEAD = 0,
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
    configureRagdoll(const core::StackVector<Ogre::Bone*, BP_MAX>& bones,
                     Ogre::SceneNode* parentNode);

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

    // @brief Remove all the data from this class and free the memory.
    //
    void
    clear(void);

    // TODO: remove this
    btRigidBody *head;

private:

    // Helper internal classes
    //
    struct RagdollBoneInfo {
        btBoxShape* shape;
        btRigidBody* rigidBody;
        Ogre::Bone* bone;
        btVector3 offset;
        RagDollUpdater motionState;
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
    core::StackVector<RagdollBoneInfo, BP_MAX> mRagdollBones;
    core::StackVector<btTypedConstraint*, BC_COUNT> mConstraints;

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

} /* namespace physics */
#endif /* RAGDOLL_H_ */
