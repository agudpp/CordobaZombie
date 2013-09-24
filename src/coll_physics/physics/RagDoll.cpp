/*
 * RagDoll.cpp
 *
 *  Created on: Sep 12, 2013
 *      Author: agustin
 */

#include "RagDoll.h"

#include <limits>

#include <OgreAxisAlignedBox.h>
#include <OgreMath.h>

#include "BulletObject.h"
#include "BulletUtils.h"



// Helper methods
//
namespace {


// @brief Construct an axis aligned bounding box from 2 points and a given
//        width and height. This will put the bb in the first position
//        with a len of dist(p1,p2) in the x axis (that it seems that how are
//        the bones are aligned).
//
inline Ogre::AxisAlignedBox
bbFrom2Pos(const Ogre::Vector3& p1,
           const Ogre::Vector3& p2,
           float height,
           float width)
{
    ASSERT(height > 0);
    ASSERT(width > 0);

    const Ogre::Vector3 distVec = p1 - p2;
    const float len = distVec.length();
    return Ogre::AxisAlignedBox(Ogre::Vector3(0, -height, -width),
                                Ogre::Vector3(len, height, width));
}

// @brief Global position orientation methods
//
inline Ogre::Quaternion
globalOgreRotation(const Ogre::Bone *bone, const Ogre::SceneNode* parentNode)
{
    return parentNode->_getDerivedOrientation() *
        (bone->getParent() ? bone->_getDerivedOrientation() : bone->getOrientation());
}
inline Ogre::Vector3
globalOgrePosition(const Ogre::Bone *bone, const Ogre::SceneNode* parentNode)
{
    return parentNode->getPosition() + parentNode->getOrientation() *
        (bone->getParent() ? bone->_getDerivedPosition() : bone->getPosition());
}

// @brief Transform ogre quaternion into bullet quaternion
//
inline Ogre::Quaternion
bulletToOgre(const btQuaternion& rot)
{
    return Ogre::Quaternion(rot.w(), rot.x(), rot.y(), rot.z());
}
// @brief Transform ogre vec into bullet vec
//
inline Ogre::Vector3
bulletToOgre(const btVector3& v)
{
    return Ogre::Vector3(v.x(), v.y(), v.z());
}
// @brief Transform bullet quaternion into ogre quaternion
//
inline btQuaternion
ogreToBullet(const Ogre::Quaternion& rot)
{
    return btQuaternion(rot.x, rot.y, rot.z, rot.w);
}
// @brief Transform bullet vec into ogre vec
//
inline btVector3
ogreToBullet(const Ogre::Vector3& v)
{
    return btVector3(v.x, v.y, v.z);
}

// @brief Obtain the orientation and position from a OgreBone.
// @param bone      The bone we want to extract the info
// @param node      The parent node of the bone, if it has... 0 if not
// @param pos       The position of the bone
// @param rot       The rotation of the bone
//
inline void
obtainGlobalInfo(const Ogre::Bone *bone,
                 const Ogre::SceneNode* node,
                 btVector3& pos,
                 btQuaternion& rot)
{
    ASSERT(bone);
    if (node) {
        pos = ogreToBullet(globalOgrePosition(bone, node));
        rot = ogreToBullet(globalOgreRotation(bone, node));
    } else {
        pos = ogreToBullet(bone->_getDerivedPosition());
        rot = ogreToBullet(bone->_getDerivedOrientation());
    }
}

// @brief Configure a rigid body from a bone
// @param rb            The rigid body to configure
// @param bone          The bone we will extract the information
// @param parentNode    The parent scene node
// @param localOffset   The local offset of the rigid body
//
inline void
configureRigidBody(btRigidBody& rb,
                   const Ogre::Bone* bone,
                   const Ogre::SceneNode* parentNode,
                   const btVector3& localOffset)
{
    // calculate the rotation and global position for this bone
    btTransform& transform = rb.getWorldTransform();
    transform.setIdentity();
    transform.setOrigin(ogreToBullet(globalOgrePosition(bone, parentNode)));
    transform.setRotation(ogreToBullet(globalOgreRotation(bone, parentNode)));
    // move the offset using the current transformation
    transform.setOrigin(transform * localOffset);
}

// @brief Configure an OBBInfo from a bone, parentNode and rigid body
// @param obInfo    The OBBInfo object to configure
// @param bone      The bone
// @param pNode     The parent node
// @param rb        The rigid body
// @param offset    The local rigid body offset.
// @param shape     The box shape associated
//
inline void
configureOBB(physics::RagDoll::OBBInfo& obInfo,
             const Ogre::Bone* bone,
             const Ogre::SceneNode* pNode,
             const btRigidBody& rb,
             const btVector3& offset,
             const btBoxShape* shape)
{
    obInfo.worldTransform.setIdentity();
    obInfo.worldTransform.setOrigin(ogreToBullet(globalOgrePosition(bone, pNode)));
    obInfo.worldTransform.setRotation(ogreToBullet(globalOgreRotation(bone, pNode)));
    obInfo.worldTransform.setOrigin(obInfo.worldTransform * offset);
    obInfo.extents = shape->getHalfExtentsWithoutMargin();
}


}



namespace physics {

////////////////////////////////////////////////////////////////////////////////
inline void
RagDoll::constructBodyPart(const btVector3& position,
                           const btQuaternion& orientation,
                           const btVector3& halfSize,
                           float aMass,
                           RagdollBoneInfo& boneInfo)
{
    btScalar mass(aMass);
    btVector3 localInertia(0,0,0);
    boneInfo.shape = new btBoxShape(halfSize);

    if (mass != 0.f) {
        boneInfo.shape->calculateLocalInertia(mass, localInertia);
    }

    // set the starting transformation position
    btTransform &startTransform = boneInfo.motionState.transform;
    startTransform.setIdentity();
    startTransform.setOrigin(position);
    startTransform.setRotation(orientation);

    btRigidBody::btRigidBodyConstructionInfo rbodyCInfo(mass,
                                                        &boneInfo.motionState,
                                                        boneInfo.shape,
                                                        localInertia);
    boneInfo.rigidBody = new btRigidBody(rbodyCInfo);
}

////////////////////////////////////////////////////////////////////////////////
bool
RagDoll::buildRagdollBoneInfo(const TempBoneInfoVec& boneInfo)
{
    ASSERT(boneInfo.size() == boneInfo.max_size());

    if (!mRagdollBones.empty()) {
        debugERROR("We already contain information in the mRagdollBones\n");
        return false;
    }

    mRagdollBones.resize(mRagdollBones.max_size());
    mAdditionalOffsets.clear();

    // common variables
    btQuaternion q1,q2,q3;
    btVector3 p1,p2,p3;
    btVector3 halfSize;
    float d1, d2, height, width;
    RagdollBoneInfo* bi = 0;

    static const float bodyPartMass = 10.f;

    ////////////////////////////////////////////////////////////////////////////
    // Construct the BP_HEAD
    // The head will have the size of the sizes between the clavicles?
    bi = &mRagdollBones[BodyPartID::BP_HEAD];
    // width and height
    height = width = boneInfo[B_CLAVICLE_L].worldPos.distance(boneInfo[B_CLAVICLE_R].worldPos)*2.f;
    d2 = boneInfo[B_NECK].worldPos.distance2(boneInfo[B_HEAD].worldPos);
    bi->offset = btVector3(0, 0, 0); // no offset
    halfSize = btVector3(d2, height, width);
    halfSize *= 0.5f;
    bi->bone = boneInfo[B_HEAD].bone;
    bi->boneID = B_HEAD;
    constructBodyPart(boneInfo[B_HEAD].worldPos,
                      boneInfo[B_HEAD].worldRot,
                      halfSize,
                      bodyPartMass * 0.1f,
                      *bi);

    ////////////////////////////////////////////////////////////////////////////
    // construct the BP_UPPER_ARM_L
    //
    bi = &mRagdollBones[BodyPartID::BP_UPPER_ARM_L];
    // calculate the height and width taking into account the distance between
    // the two bones, we will use a % of the length
    d1 = boneInfo[B_UPPER_ARM_L].worldPos.distance(boneInfo[B_FORE_ARM_L].worldPos);
    height = width = d1 * 0.35f;
    p3 = (boneInfo[B_UPPER_ARM_L].worldPos + boneInfo[B_FORE_ARM_L].worldPos) * 0.5f; // position of the shape
    bi->offset = -boneInfo[B_UPPER_ARM_L].toLocal(p3);
    halfSize = btVector3(d1, height, width) * 0.5f;
    bi->bone = boneInfo[B_UPPER_ARM_L].bone;
    bi->boneID = B_UPPER_ARM_L;
    constructBodyPart(p3,
                      boneInfo[B_UPPER_ARM_L].worldRot,
                      halfSize,
                      bodyPartMass * 0.05f,
                      *bi);

    ////////////////////////////////////////////////////////////////////////////
    // construct the BP_UPPER_ARM_R
    //
    bi = &mRagdollBones[BodyPartID::BP_UPPER_ARM_R];
    // calculate the height and width taking into account the distance between
    // the two bones, we will use a % of the length
    d1 = boneInfo[B_UPPER_ARM_R].worldPos.distance(boneInfo[B_FORE_ARM_R].worldPos);
    height = width = d1 * 0.35f;
    p3 = (boneInfo[B_UPPER_ARM_R].worldPos + boneInfo[B_FORE_ARM_R].worldPos) * 0.5f; // position of the shape
    bi->offset = -boneInfo[B_UPPER_ARM_R].toLocal(p3);
    halfSize = btVector3(d1, height, width) * 0.5f;
    bi->bone = boneInfo[B_UPPER_ARM_R].bone;
    bi->boneID = B_UPPER_ARM_R;
    constructBodyPart(p3,
                      boneInfo[B_UPPER_ARM_R].worldRot,
                      halfSize,
                      bodyPartMass *0.05f,
                      *bi);

    ////////////////////////////////////////////////////////////////////////////
    // construct the BP_FORE_ARM_L
    //
    bi = &mRagdollBones[BodyPartID::BP_FORE_ARM_L];
    // calculate the height and width taking into account the distance between
    // the two bones, we will use a % of the length
    d1 = boneInfo[B_FORE_ARM_L].worldPos.distance(boneInfo[B_HAND_L].worldPos);
    height = width = d1 * 0.35f;
    p3 = (boneInfo[B_FORE_ARM_L].worldPos + boneInfo[B_HAND_L].worldPos) * 0.5f; // position of the shape
    bi->offset = -boneInfo[B_FORE_ARM_L].toLocal(p3);
    halfSize = btVector3(d1, height, width) * 0.5f;
    bi->bone = boneInfo[B_FORE_ARM_L].bone;
    bi->boneID = B_FORE_ARM_L;
    constructBodyPart(p3,
                      boneInfo[B_FORE_ARM_L].worldRot,
                      halfSize,
                      bodyPartMass *0.025f,
                      *bi);

    // we will track the hand child
    bi->childJoints.push_back(mAdditionalOffsets.size());
    mAdditionalOffsets.push_back(BoneChildOffset(boneInfo[B_HAND_L].bone,
        boneInfo[B_FORE_ARM_L].toLocal(boneInfo[B_HAND_L].worldPos) + bi->offset));

    ////////////////////////////////////////////////////////////////////////////
    // construct the BP_FORE_ARM_R
    //
    bi = &mRagdollBones[BodyPartID::BP_FORE_ARM_R];
    // calculate the height and width taking into account the distance between
    // the two bones, we will use a % of the length
    d1 = boneInfo[B_FORE_ARM_R].worldPos.distance(boneInfo[B_HAND_R].worldPos);
    height = width = d1 * 0.35f;
    p3 = (boneInfo[B_FORE_ARM_R].worldPos + boneInfo[B_HAND_R].worldPos) * 0.5f; // position of the shape
    bi->offset = -boneInfo[B_FORE_ARM_R].toLocal(p3);
    halfSize = btVector3(d1, height, width) * 0.5f;
    bi->bone = boneInfo[B_FORE_ARM_R].bone;
    bi->boneID = B_FORE_ARM_R;
    constructBodyPart(p3,
                      boneInfo[B_FORE_ARM_R].worldRot,
                      halfSize,
                      bodyPartMass * 0.025f,
                      *bi);

    // we will track the hand child
    bi->childJoints.push_back(mAdditionalOffsets.size());
    mAdditionalOffsets.push_back(BoneChildOffset(boneInfo[B_HAND_R].bone,
        boneInfo[B_FORE_ARM_R].toLocal(boneInfo[B_HAND_R].worldPos) + bi->offset));

    ////////////////////////////////////////////////////////////////////////////
    // construct the BP_SPINE
    // this is the "TORSO" and should be the size from the spine to the neck
    // and height = from one thigh to the other one
    bi = &mRagdollBones[BodyPartID::BP_SPINE];
    height = boneInfo[B_THIGH_L].worldPos.distance(boneInfo[B_THIGH_R].worldPos) * 1.15f;
    width = height * 0.3f;
    d1 = boneInfo[B_SPINE].worldPos.distance(boneInfo[B_NECK].worldPos);
    p3 = (boneInfo[B_SPINE].worldPos + boneInfo[B_NECK].worldPos) * 0.5f; // position of the shape
    bi->offset = -boneInfo[B_SPINE].toLocal(p3);
    halfSize = btVector3(d1, height, width) * 0.5f;
    bi->bone = boneInfo[B_SPINE].bone;
    bi->boneID = B_SPINE;
    constructBodyPart(p3,
                      boneInfo[B_SPINE].worldRot,
                      halfSize,
                      bodyPartMass * 0.4f,
                      *bi);

    // for the spine in particular we will create 4 child bones associated to this
    // B_NECK, B_CLAVICLE_R, B_CLAVICLE_L, B_PELVIS
    bi->childJoints.push_back(mAdditionalOffsets.size());
    mAdditionalOffsets.push_back(BoneChildOffset(boneInfo[B_NECK].bone ,
        boneInfo[B_SPINE].toLocal(boneInfo[B_NECK].worldPos) + bi->offset));
    bi->childJoints.push_back(mAdditionalOffsets.size());
    mAdditionalOffsets.push_back(BoneChildOffset(boneInfo[B_CLAVICLE_R].bone ,
        boneInfo[B_SPINE].toLocal(boneInfo[B_CLAVICLE_R].worldPos) + bi->offset));
    bi->childJoints.push_back(mAdditionalOffsets.size());
    mAdditionalOffsets.push_back(BoneChildOffset(boneInfo[B_CLAVICLE_L].bone ,
        boneInfo[B_SPINE].toLocal(boneInfo[B_CLAVICLE_L].worldPos) + bi->offset));

    ////////////////////////////////////////////////////////////////////////////
    // construct the BP_PELVIS
    bi = &mRagdollBones[BodyPartID::BP_PELVIS];
    height = boneInfo[B_THIGH_L].worldPos.distance(boneInfo[B_THIGH_R].worldPos);
    width = height * 0.3f;
    d1 = boneInfo[B_PELVIS].worldPos.distance(boneInfo[B_SPINE].worldPos);
    p3 = (boneInfo[B_PELVIS].worldPos + boneInfo[B_SPINE].worldPos) * 0.5f; // position of the shape
    bi->offset = -boneInfo[B_PELVIS].toLocal(p3);
    halfSize = btVector3(d1, height, width) * 0.5f;
    bi->bone = boneInfo[B_PELVIS].bone;
    bi->boneID = B_PELVIS;
    constructBodyPart(p3,
                      boneInfo[B_PELVIS].worldRot,
                      halfSize,
                      bodyPartMass * 0.4f,
                      *bi);

    ////////////////////////////////////////////////////////////////////////////
    // construct the BP_THIGH_L
    //
    bi = &mRagdollBones[BodyPartID::BP_THIGH_L];
    // calculate the height and width taking into account the distance between
    // the two bones, we will use a % of the length
    d1 = boneInfo[B_THIGH_L].worldPos.distance(boneInfo[B_CALF_L].worldPos);
    height = width = d1 * 0.35f;
    p3 = (boneInfo[B_THIGH_L].worldPos + boneInfo[B_CALF_L].worldPos) * 0.5f; // position of the shape
    bi->offset = -boneInfo[B_THIGH_L].toLocal(p3);
    halfSize = btVector3(d1, height, width) * 0.5f;
    bi->bone = boneInfo[B_THIGH_L].bone;
    bi->boneID = B_THIGH_L;
    constructBodyPart(p3,
                      boneInfo[B_THIGH_L].worldRot,
                      halfSize,
                      bodyPartMass * 0.1f,
                      *bi);

    ////////////////////////////////////////////////////////////////////////////
    // construct the BP_THIGH_R
    //
    bi = &mRagdollBones[BodyPartID::BP_THIGH_R];
    // calculate the height and width taking into account the distance between
    // the two bones, we will use a % of the length
    d1 = boneInfo[B_THIGH_R].worldPos.distance(boneInfo[B_CALF_R].worldPos);
    height = width = d1 * 0.35f;
    p3 = (boneInfo[B_THIGH_R].worldPos + boneInfo[B_CALF_R].worldPos) * 0.5f; // position of the shape
    bi->offset = -boneInfo[B_THIGH_R].toLocal(p3);
    halfSize = btVector3(d1, height, width) * 0.5f;
    bi->bone = boneInfo[B_THIGH_R].bone;
    bi->boneID = B_THIGH_R;
    constructBodyPart(p3,
                      boneInfo[B_THIGH_R].worldRot,
                      halfSize,
                      bodyPartMass * 0.1f,
                      *bi);

    ////////////////////////////////////////////////////////////////////////////
    // construct the BP_LEG_L
    //
    bi = &mRagdollBones[BodyPartID::BP_LEG_L];
    // calculate the height and width taking into account the distance between
    // the two bones, we will use a % of the length
    d1 = boneInfo[B_CALF_L].worldPos.distance(boneInfo[B_FOOT_L].worldPos);
    height = width = d1 * 0.35f;
    p3 = (boneInfo[B_CALF_L].worldPos + boneInfo[B_FOOT_L].worldPos) * 0.5f; // position of the shape
    bi->offset = -boneInfo[B_CALF_L].toLocal(p3);
    halfSize = btVector3(d1, height, width) * 0.5f;
    bi->bone = boneInfo[B_CALF_L].bone;
    bi->boneID = B_CALF_L;
    constructBodyPart(p3,
                      boneInfo[B_CALF_L].worldRot,
                      halfSize,
                      bodyPartMass * 0.1f,
                      *bi);

    // track foot child bone
    bi->childJoints.push_back(mAdditionalOffsets.size());
    mAdditionalOffsets.push_back(BoneChildOffset(boneInfo[B_FOOT_L].bone,
        boneInfo[B_CALF_L].toLocal(boneInfo[B_FOOT_L].worldPos) + bi->offset));

    ////////////////////////////////////////////////////////////////////////////
    // construct the BP_LEG_R
    //
    bi = &mRagdollBones[BodyPartID::BP_LEG_R];
    // calculate the height and width taking into account the distance between
    // the two bones, we will use a % of the length
    d1 = boneInfo[B_CALF_R].worldPos.distance(boneInfo[B_FOOT_R].worldPos);
    height = width = d1 * 0.35f;
    p3 = (boneInfo[B_CALF_R].worldPos + boneInfo[B_FOOT_R].worldPos) * 0.5f; // position of the shape
    bi->offset = -boneInfo[B_CALF_R].toLocal(p3);
    halfSize = btVector3(d1, height, width) * 0.5f;
    bi->bone = boneInfo[B_CALF_R].bone;
    bi->boneID = B_CALF_R;
    constructBodyPart(p3,
                      boneInfo[B_CALF_R].worldRot,
                      halfSize,
                      bodyPartMass * 0.1f,
                      *bi);

    // track foot child bone
    bi->childJoints.push_back(mAdditionalOffsets.size());
    mAdditionalOffsets.push_back(BoneChildOffset(boneInfo[B_FOOT_R].bone,
        boneInfo[B_CALF_R].toLocal(boneInfo[B_FOOT_R].worldPos) + bi->offset));


    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
RagDoll::setConstraints(const TempBoneInfoVec& boneInfo)
{
    if (!mConstraints.empty() || mRagdollBones.empty()) {
        debugERROR("There are some constraints already set?! or no bones set\n");
        return false;
    }

    // configure the rigid body values
    for (RagdollBoneInfo& rb: mRagdollBones) {
        ASSERT(rb.rigidBody);
        rb.rigidBody->setDamping(0.05, 0.85);
        rb.rigidBody->setDeactivationTime(0.8);
        rb.rigidBody->setSleepingThresholds(1.6, 2.5);
    }

    mConstraints.resize(mConstraints.max_size());

    // configure the constraints one by one
    btHingeConstraint* hingeC = 0;
    btConeTwistConstraint* coneC = 0;
    btTransform localA, localB;
    btVector3 p1,p2,p3;
    float d1, d2;

    ////////////////////////////////////////////////////////////////////////////
    // spine and head
    localA.setIdentity(); localB.setIdentity();
    localA.getOrigin() = mRagdollBones[BodyPartID::BP_SPINE].offset +
        boneInfo[B_SPINE].toLocal(boneInfo[B_NECK].worldPos);
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_HEAD].offset +
        boneInfo[B_HEAD].toLocal(boneInfo[B_NECK].worldPos);
    coneC = new btConeTwistConstraint(*mRagdollBones[BodyPartID::BP_SPINE].rigidBody,
                                      *mRagdollBones[BodyPartID::BP_HEAD].rigidBody,
                                      localA,
                                      localB);
    coneC->setLimit(M_PI_4, M_PI_4, M_PI_2);
    mConstraints[BodyConstraints::BC_SPINE_HEAD] = coneC;

    ////////////////////////////////////////////////////////////////////////////
    // spine and upper arm l, for this particular case we need to do a little
    // trick since the upper arm is not very close to the neck, we need to calculate
    // the local for the neck that is moving the neck in the direction of the
    // upper arm.
    localA.setIdentity(); localB.setIdentity();
    localA.getBasis().setEulerZYX(M_PI,0,M_PI_2);
    localA.getOrigin() = mRagdollBones[BodyPartID::BP_SPINE].offset +
        boneInfo[B_SPINE].toLocal(boneInfo[B_UPPER_ARM_L].worldPos);
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_UPPER_ARM_L].offset;
    coneC = new btConeTwistConstraint(*mRagdollBones[BodyPartID::BP_SPINE].rigidBody,
                                      *mRagdollBones[BodyPartID::BP_UPPER_ARM_L].rigidBody,
                                      localA,
                                      localB);
    coneC->setLimit(M_PI_2, M_PI_2, 0);
    mConstraints[BodyConstraints::BC_LEFT_SHOULDER] = coneC;

    ////////////////////////////////////////////////////////////////////////////
    // spine and upper arm r. The same than above here.
    localA.setIdentity(); localB.setIdentity();
    localA.getBasis().setEulerZYX(M_PI,0,-M_PI_2);
    localA.getOrigin() = mRagdollBones[BodyPartID::BP_SPINE].offset +
        boneInfo[B_SPINE].toLocal(boneInfo[B_UPPER_ARM_R].worldPos);
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_UPPER_ARM_R].offset;
    coneC = new btConeTwistConstraint(*mRagdollBones[BodyPartID::BP_SPINE].rigidBody,
                                      *mRagdollBones[BodyPartID::BP_UPPER_ARM_R].rigidBody,
                                      localA,
                                      localB);
    coneC->setLimit(M_PI_2, M_PI_2, 0);
    mConstraints[BodyConstraints::BC_RIGHT_SHOULDER] = coneC;

    ////////////////////////////////////////////////////////////////////////////
    // upper arm l and forearm l
    localA.setIdentity(); localB.setIdentity();
    localA.getOrigin() = mRagdollBones[BodyPartID::BP_UPPER_ARM_L].offset +
        boneInfo[B_UPPER_ARM_L].toLocal(boneInfo[B_FORE_ARM_L].worldPos);
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_FORE_ARM_L].offset;
    hingeC = new btHingeConstraint(*mRagdollBones[BodyPartID::BP_UPPER_ARM_L].rigidBody,
                                      *mRagdollBones[BodyPartID::BP_FORE_ARM_L].rigidBody,
                                      localA,
                                      localB);
    hingeC->setLimit(-M_PI_2, 0);
    mConstraints[BodyConstraints::BC_LEFT_ELBOW] = hingeC;

    ////////////////////////////////////////////////////////////////////////////
    // upper arm r and forearm r
    localA.setIdentity(); localB.setIdentity();
    localA.getOrigin() = mRagdollBones[BodyPartID::BP_UPPER_ARM_R].offset +
        boneInfo[B_UPPER_ARM_R].toLocal(boneInfo[B_FORE_ARM_R].worldPos);
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_FORE_ARM_R].offset;
    hingeC = new btHingeConstraint(*mRagdollBones[BodyPartID::BP_UPPER_ARM_R].rigidBody,
                                      *mRagdollBones[BodyPartID::BP_FORE_ARM_R].rigidBody,
                                      localA,
                                      localB);
    hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
    mConstraints[BodyConstraints::BC_RIGHT_ELBOW] = hingeC;

    ////////////////////////////////////////////////////////////////////////////
    // pelvis and spine
    //
    localA.setIdentity(); localB.setIdentity();
    localA.getOrigin() = -mRagdollBones[BodyPartID::BP_PELVIS].offset;
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_SPINE].offset;
    hingeC = new btHingeConstraint(*mRagdollBones[BodyPartID::BP_PELVIS].rigidBody,
                                   *mRagdollBones[BodyPartID::BP_SPINE].rigidBody,
                                   localA,
                                   localB);
    hingeC->setLimit(btScalar(-M_PI_4), btScalar(M_PI_2));
    mConstraints[BodyConstraints::BC_SPINE_PELVIS] = hingeC;

    ////////////////////////////////////////////////////////////////////////////
    // PELVIS and thigh l
    //
    localA.setIdentity(); localB.setIdentity();
    localA.getOrigin() = mRagdollBones[BodyPartID::BP_PELVIS].offset +
        boneInfo[B_PELVIS].toLocal(boneInfo[B_THIGH_L].worldPos);
    localB.getBasis().setEulerZYX(0,0,M_PI);
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_THIGH_L].offset;
    coneC = new btConeTwistConstraint(*mRagdollBones[BodyPartID::BP_PELVIS].rigidBody,
                                      *mRagdollBones[BodyPartID::BP_THIGH_L].rigidBody,
                                      localA,
                                      localB);
    coneC->setLimit(M_PI_4, M_PI_2, 0);
    mConstraints[BodyConstraints::BC_LEFT_HIP] = coneC;

    ////////////////////////////////////////////////////////////////////////////
    // pelvis and thigh r, the same here
    localA.setIdentity(); localB.setIdentity();
    localA.getOrigin() = mRagdollBones[BodyPartID::BP_PELVIS].offset +
         boneInfo[B_PELVIS].toLocal(boneInfo[B_THIGH_R].worldPos);
    localB.getBasis().setEulerZYX(0,0,-M_PI);
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_THIGH_R].offset;
    coneC = new btConeTwistConstraint(*mRagdollBones[BodyPartID::BP_PELVIS].rigidBody,
                                      *mRagdollBones[BodyPartID::BP_THIGH_R].rigidBody,
                                      localA,
                                      localB);
    coneC->setLimit(M_PI_4, M_PI_2, 0);
    mConstraints[BodyConstraints::BC_RIGHT_HIP] = coneC;

    ////////////////////////////////////////////////////////////////////////////
    // thigh l and leg l
    localA.setIdentity(); localB.setIdentity();
    localA.getOrigin() = -mRagdollBones[BodyPartID::BP_THIGH_L].offset;
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_LEG_L].offset;
    hingeC = new btHingeConstraint(*mRagdollBones[BodyPartID::BP_THIGH_L].rigidBody,
                                   *mRagdollBones[BodyPartID::BP_LEG_L].rigidBody,
                                   localA.getOrigin(),
                                   localB.getOrigin(),
                                   btVector3(0,1,0),
                                   btVector3(0,1,0));
    hingeC->setLimit(0, M_PI_2);
    mConstraints[BodyConstraints::BC_LEFT_KNEE] = hingeC;

    ////////////////////////////////////////////////////////////////////////////
    // thigh r and leg r
    localA.setIdentity(); localB.setIdentity();
    localA.getOrigin() = -mRagdollBones[BodyPartID::BP_THIGH_R].offset;
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_LEG_R].offset;
    hingeC = new btHingeConstraint(*mRagdollBones[BodyPartID::BP_THIGH_R].rigidBody,
                                   *mRagdollBones[BodyPartID::BP_LEG_R].rigidBody,
                                   localA.getOrigin(),
                                   localB.getOrigin(),
                                   btVector3(0,1,0),
                                   btVector3(0,1,0));
    hingeC->setLimit(0, M_PI_2);
    mConstraints[BodyConstraints::BC_RIGHT_KNEE] = hingeC;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
void
RagDoll::createPrimitives(void)
{
    ASSERT(!mRagdollBones.empty());

    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();

    for (RagdollBoneInfo& bi : mRagdollBones) {
        ASSERT(bi.shape != 0);
        const btVector3& halfSize = bi.shape->getHalfExtentsWithoutMargin();
        const btTransform& tr = bi.motionState.transform;
        const Ogre::Vector3 center = bulletToOgre(tr.getOrigin());
        const Ogre::Quaternion rot = bulletToOgre(tr.getRotation());
        const Ogre::Vector3 sizes = bulletToOgre(halfSize) * 2.f;
        bi.motionState.primitive = pd.createBox(Ogre::Vector3::ZERO, sizes, pd.getFreshColour());
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
RagDoll::RagDoll(btDynamicsWorld* bdw) :
    mDynamicWorld(bdw)
,   mParentNode(0)
,   mEnabled(false)
{

}

////////////////////////////////////////////////////////////////////////////////
RagDoll::~RagDoll()
{
    clear();
}

////////////////////////////////////////////////////////////////////////////////
bool
RagDoll::buildFromSkeleton(const BoneTable& ogreBones,
                           Ogre::SceneNode* parentNode)
{
    ASSERT(parentNode);
    ASSERT(mDynamicWorld);

    if (ogreBones.size() != ogreBones.max_size()) {
        debugERROR("We need all the bones to construct the ragdoll\n");
        return false;
    }

    if (!mRagdollBones.empty()) {
        debugERROR("This skeleton was already built\n");
        return false;
    }

    // first we will get the information of each bone, the position and orientation
    // (global world position / orientation) and the size that it should have.
    // Then we will create each ragdoll and shape and put them in the correct
    // place.
    // At the end we will create the constraints using the information we have.
    //

    // construct the TempBoneInfoVec structure with the information of all the
    // bones
    TempBoneInfoVec boneInfo;
    boneInfo.resize(ogreBones.size());
    for (unsigned int i = 0; i < ogreBones.size(); ++i) {
        TempBoneInfo& bi = boneInfo[i];
        bi.bone = ogreBones[i];
        obtainGlobalInfo(bi.bone, parentNode, bi.worldPos, bi.worldRot);
    }

    // now build the ragdoll rigid bodies
    if (!buildRagdollBoneInfo(boneInfo)) {
        debugERROR("Error constructing the ragdoll bone info\n");
        return false;
    }

    // configure the initial position/rotation of each bone relative not in
    // world coordinates
    //
    mInitialInfo.clear();
    for (Ogre::Bone* b : ogreBones) {
        if (b->getParent()){
            mInitialInfo.push_back(InitialStateBoneInfo(b->_getDerivedPosition(),
                                                        b->_getDerivedOrientation()));
        } else {
            mInitialInfo.push_back(InitialStateBoneInfo(b->getPosition(),
                                                        b->getOrientation()));
        }
    }

//#ifdef DEBUG
//    createPrimitives();
//#endif

    // construct the constraints now
    if (!setConstraints(boneInfo)) {
        debugERROR("Error setting the constraints\n");
        return false;
    }

    // everything fine
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
RagDoll::configureRagdoll(const BoneTable& bones,
                          Ogre::SceneNode* parentNode)
{
    ASSERT(parentNode);
    ASSERT(!mRagdollBones.empty() && "We need to set first the skeleton");

    if (bones.size() != bones.max_size()) {
        debugERROR("This bone table is not correct, we need %d and we have %d\n",
            bones.max_size(), bones.size());
        return;
    }

    // disable the ragdoll
    setEnable(false);

    // set the parent node
    mParentNode = parentNode;

    // set the current bones and its positions in the physics world
    for (RagdollBoneInfo* bi = mRagdollBones.begin(), *end = mRagdollBones.end();
            bi != end; ++bi) {
        bi->bone = bones[bi->boneID];
        bi->motionState.dirty = true;
        configureRigidBody(*bi->rigidBody, bi->bone, parentNode, -bi->offset);
        bi->motionState.setWorldTransform(bi->rigidBody->getWorldTransform());
    }

    // we will track the global position of the pelvis as reference to move the
    // mParentNode
    mParentLastPos = mRagdollBones[BodyPartID::BP_PELVIS].motionState.transform.getOrigin();
}

////////////////////////////////////////////////////////////////////////////////
void
RagDoll::resetBones(BoneTable& bones)
{
    ASSERT(bones.size() == mInitialInfo.size());
    InitialStateBoneInfo* bi = mInitialInfo.begin(), *bie = mInitialInfo.end();
    Ogre::Bone** bone = bones.begin(), **bonee = bones.end();
    while(bone != bonee) {
        if ((*bone)->getParent()) {
            (*bone)->_setDerivedOrientation(bi->rotation);
            (*bone)->_setDerivedPosition(bi->position);
        } else {
            (*bone)->setOrientation(bi->rotation);
            (*bone)->setPosition(bi->position);
        }
        (*bone)->setManuallyControlled(false);
        ++bone;
        ++bi;
    }
}

////////////////////////////////////////////////////////////////////////////////
void
RagDoll::setEnable(bool enable)
{
    if (enable == mEnabled) {
        // nothing to do
        return;
    }
    // now we need to check if we have to enable / disable
    if (enable) {
        for (RagdollBoneInfo& bi : mRagdollBones) {
            // clear first all the possible forces to avoid
            bi.rigidBody->clearForces();
            bi.rigidBody->setAngularVelocity(btVector3(0,0,0));
            bi.rigidBody->setLinearVelocity(btVector3(0,0,0));
            mDynamicWorld->addRigidBody(bi.rigidBody);
            bi.bone->setManuallyControlled(true);
        }
        for (btTypedConstraint* c : mConstraints) {
            mDynamicWorld->addConstraint(c, true);
        }
        for (BoneChildOffset& co : mAdditionalOffsets) {
            co.bone->setManuallyControlled(true);
        }
    } else {
        for (RagdollBoneInfo& bi : mRagdollBones) {
            mDynamicWorld->removeRigidBody(bi.rigidBody);
        }
        for (btTypedConstraint* c : mConstraints) {
            mDynamicWorld->removeConstraint(c);
        }
//        for (BoneChildOffset& co : mAdditionalOffsets) {
//            co.bone->setManuallyControlled(false);
//        }
    }

    mEnabled = enable;
}

////////////////////////////////////////////////////////////////////////////////
void
RagDoll::clear(void)
{
    // remove all the constraints and all the rigid bodies from the world
    ASSERT(mDynamicWorld);

    for (btTypedConstraint* c: mConstraints) {
        mDynamicWorld->removeConstraint(c);
        delete c;
    }
    mConstraints.clear();

    for (RagdollBoneInfo& bi: mRagdollBones) {
        mDynamicWorld->removeRigidBody(bi.rigidBody);
        delete bi.rigidBody;
        delete bi.shape;
    }
    mRagdollBones.clear();
    mEnabled = false;
}

////////////////////////////////////////////////////////////////////////////////
void
RagDoll::getBodyPartsOBB(const BoneTable& bones,
                         const Ogre::SceneNode* parentNode,
                         OBBInfoVec& obbs) const
{
    ASSERT(parentNode);
    ASSERT(!mRagdollBones.empty() && "We need to set first the skeleton");

    if (bones.size() != bones.max_size()) {
        debugERROR("This bone table is not correct, we need %d and we have %d\n",
            bones.max_size(), bones.size());
        return;
    }
    obbs.resize(mRagdollBones.size());

    // get an configure each obb taking into account the ragdolls and bone position
    // and orientation
    for (unsigned int i = 0; i < mRagdollBones.size(); ++i) {
        const RagdollBoneInfo& bi = mRagdollBones[i];
        OBBInfo& ob = obbs[i];
        configureOBB(ob,
                     bones[bi.boneID],
                     parentNode,
                     *bi.rigidBody,
                     -bi.offset,
                     bi.shape);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
RagDoll::getClosestIntersection(const BoneTable& bones,
                                const Ogre::SceneNode* parentNode,
                                const Ogre::Ray& ray,
                                BodyPartID& bpIntersected) const
{
    // first of all we will calculate the distance of the ray with each boxShape
    // and check if the distance is less than the extents of the box, if it is
    // we probably are intersecting that box, if not, is not possible.
    // For all those boxes that we are maybe intersection we will convert the
    // ray into the OBB coordinates system and check using the
    // Ogre::Math::intersects() method to verify that.
    //
    OBBInfoVec obbs;
    getBodyPartsOBB(bones, parentNode, obbs);
    const btVector3 origin = ogreToBullet(ray.getOrigin());
    const btVector3 dir = ogreToBullet(ray.getDirection());
    float minDistance = std::numeric_limits<float>::max();
    bool anyCollision = false;

    for (unsigned int i = 0; i < mRagdollBones.size(); ++i) {
            const RagdollBoneInfo& bi = mRagdollBones[i];
            OBBInfo& ob = obbs[i];

            // calculate the distance between the ray and the center of the box
            // and also the squared extents
            const float sqrExtent = ob.extents.length2();
            const float sqrLen =
                dir.cross(ob.worldTransform.getOrigin() - origin).length2();

            if (sqrLen <= sqrExtent) {
                // it probably intersect... check for that
                // transform the ray from world coordinate into a local ray
                // (local of the current OBB).
                std::pair<bool, float> result =
                    BulletUtils::intersects(ob.worldTransform, ob.extents, ray);
                if (result.first && result.second < minDistance) {
                    anyCollision = true;
                    minDistance = result.second;
                    bpIntersected = BodyPartID(i);
                }
            }
    }


    return anyCollision;
}

////////////////////////////////////////////////////////////////////////////////
bool
RagDoll::update(void)
{
    ASSERT(mParentNode);
    ASSERT(mEnabled);

    bool needToUpdate = false;

    // we will update the position of the sceneNode to match with the pelvis
    RagdollBoneInfo& pelvis = mRagdollBones[BP_PELVIS];
    const btVector3& currentPos = pelvis.motionState.transform.getOrigin();
    mParentNode->translate(bulletToOgre(currentPos - mParentLastPos));
    mParentLastPos = currentPos;

    // we will update the position of each bone of the current skeleton
    const btVector3 parentPos = ogreToBullet(mParentNode->_getDerivedPosition());
    const Ogre::Quaternion parentRot = mParentNode->_getDerivedOrientation();
    const Ogre::Quaternion parentInv = parentRot.Inverse();

    for (RagdollBoneInfo& bi: mRagdollBones) {
        // for each ragdoll, we need to get the associated world position to the bone
        // since they are moved with an local offset.
        // 1) Transform the local offset into world offset.
        // 2) Get the world position of the ragdoll to move the associated bone
        // 3) Get the current world orientation of the ragdoll and set it to the
        //    bone.
        // 4) Move the childs bones to the associated positions using its
        //    offsets and do not change its local orientation.
        //

        // check if we need to update this bone or not
        if (bi.motionState.dirty) {
            const btTransform& ragdollTrans = bi.motionState.transform;
            const btVector3 worldBonePos = ragdollTrans * bi.offset;

            // get the posToSet
            const Ogre::Vector3 posToSet = parentInv * bulletToOgre(worldBonePos - parentPos);
            const Ogre::Quaternion rotToSet = parentInv *
                bulletToOgre(ragdollTrans.getRotation());

            if (bi.bone->getParent()) {
                bi.bone->_setDerivedPosition(posToSet);
                bi.bone->_setDerivedOrientation(rotToSet);
            } else {
                bi.bone->setPosition(posToSet);
                bi.bone->setOrientation(rotToSet);
            }

            // move the childs now
            if (!bi.childJoints.empty()) {
                for (unsigned short& i : bi.childJoints) {
                    ASSERT(i < mAdditionalOffsets.size());
                    BoneChildOffset& co = mAdditionalOffsets[i];
                    ASSERT(co.bone);
                    const btVector3 worldBonePosChild = ragdollTrans * co.offset;
                    const Ogre::Vector3 posToSetChild = parentInv *
                        bulletToOgre(worldBonePosChild - parentPos);
                    if (co.bone->getParent()) {
                        co.bone->_setDerivedPosition(posToSetChild);
                    } else {
                        co.bone->setPosition(posToSetChild);
                    }
                }
            }
            bi.motionState.dirty = false;
            needToUpdate = true;
        }
    }

    return needToUpdate;
}

} /* namespace physics */
