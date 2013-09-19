/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef TestingBullet_H_
#define TestingBullet_H_

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>
#include <OgreColourValue.h>

#include <vector>
#include <string>

#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <debug/OgreText.h>
#include <debug/OgreTextTable.h>
#include <types/basics.h>
#include <utils/OrbitCamera.h>
#include <input/InputHelper.h>
#include <types/StackVector.h>
#include <debug/PrimitiveDrawer.h>

#include <bullet/btBulletDynamicsCommon.h>
#include "BulletObject.h"
#include "RagDoll.h"


// The xml file for this tool
// <TestingBullet mesh="file.mesh" />
//
#define ANIM_PLAYER_XML_FILE    "TestingBullet.xml"

namespace tests {

class TestingBullet : public core::AppTester
{
public:
    TestingBullet();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~TestingBullet();

private:

    // @brief Load the floor
    //
    void
    loadFloor(void);

    // @brief Configure bullet dynamic world and bullet scene
    //
    void
    configureBullet(void);

    // @brief Create a box from 2 positions. It will create the box with
    //        the length (in the Z value of the BB) and the dimensions dim
    //        for the x and y values.
    //
    Ogre::AxisAlignedBox
    bbFrom2Pos(const Ogre::Vector3& p1, const Ogre::Vector3& p2, float dim) const;

    // @brief Create a primitive from 2 bones and the parent node
    //
    core::Primitive*
    primitiveFromBones(const Ogre::Bone* b1,
                       const Ogre::Bone* b2,
                       const Ogre::SceneNode* node) const;

    // @brief Create an entity and show all its bones
    //
    void
    createEntityAndShowBones(void);

    // @brief Construct ragdoll
    //
    void
    createRagdoll(Ogre::SkeletonInstance* s, Ogre::SceneNode* sn);

    // @brief create sample fisics
    //
    void
    createSampleScene(void);

    // @brief create ragdolls and entities tables
    //
    void
    createRagdollInstances(void);


    void
    handleRagdollInput(void);

    void
    handleCameraInput(void);

private:
    Ogre::SceneNode *mNode;
    Ogre::Entity *mEntity;
    float mTimeFrame;
    tool::OrbitCamera mOrbitCamera;
    core::OgreTextTable mTextTable;
    input::InputHelper mInputHelper;

    // bullet
    btDefaultCollisionConfiguration mDefConf;
    btCollisionDispatcher mDispatcher;
    btDbvtBroadphase mBroadPhase;
    btSequentialImpulseConstraintSolver mSeqSolver;
    btDiscreteDynamicsWorld mDynamicWorld;
    btAlignedObjectArray<btCollisionShape*> mCollisionShapes;
    BulletObject mOgreBulletObject;

    // RagDoll tests parts
    BulletObject mHand;
    BulletObject mArm;
    BulletObject mArm2;
    btConeTwistConstraint* mConstraint;

    // Skeleton
    struct BoxBoneInfo {
        core::Primitive *p;
        Ogre::SceneNode* boneNode;
        Ogre::Bone* bone;

        BoxBoneInfo(core::Primitive* prim = 0, Ogre::SceneNode* n = 0, Ogre::Bone* b = 0) :
            p(prim), boneNode(n), bone(b)
        {}

        // return the world position of the bone
        inline Ogre::Vector3
        getWorldPosition()
        {
            return boneNode->getPosition() +
                boneNode->getOrientation() * bone->_getDerivedPosition();
        }

        // update the position of the primitive
        inline void
        updatePrimitivePosition(void)
        {
            p->node->setPosition(getWorldPosition());
            p->node->setOrientation(boneNode->_getDerivedOrientation() *
                bone->_getDerivedOrientation());
        }
    };


    core::StackVector<BoxBoneInfo, 15> mBoxBoneInfo;
    Ogre::SceneNode* mModelNode;
    Ogre::SceneNode* mSkeletonNode;
    Ogre::SceneNode* mArmNode;
    Ogre::SkeletonInstance* mSkeletonInstance;
    Ogre::Bone* mArmBone;
    Ogre::Skeleton* mSkeleton;
    core::StackVector<core::Primitive*, 45> mBones;
    Ogre::AnimationState* mAnimState;
    physics::RagDoll mRagdoll;

    struct RagdollInfo {
        physics::RagDoll ragdoll;
        physics::BoneTable table;
        Ogre::SceneNode* node;
        Ogre::Entity* ent;
        bool needToUpdate;
    };
    core::StackVector<RagdollInfo, 40> mRagdolls;
    bool mRagdollNeedToUpdate;
    physics::BoneTable table;
    btRigidBody* mHeadNode;
};

}

#endif /* TestingBullet_H_ */
