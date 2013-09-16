/*
 * RagDoll.cpp
 *
 *  Created on: Sep 12, 2013
 *      Author: agustin
 */

#include "RagDoll.h"

#include <OgreAxisAlignedBox.h>

#include "BulletObject.h"



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
    return parentNode->_getDerivedOrientation() * bone->_getDerivedOrientation();
}
inline Ogre::Vector3
globalOgrePosition(const Ogre::Bone *bone, const Ogre::SceneNode* parentNode)
{
    return parentNode->getPosition() + parentNode->getOrientation() *
        bone->_getDerivedPosition();
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

    // set the bone information for the motion state
    boneInfo.motionState.bone = boneInfo.bone;

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

    mDynamicWorld->addRigidBody(boneInfo.rigidBody);
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

    // common variables
    btQuaternion q1,q2,q3;
    btVector3 p1,p2,p3;
    btVector3 halfSize;
    float d1, d2, height, width;
    RagdollBoneInfo* bi = 0;

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
    constructBodyPart(boneInfo[B_HEAD].worldPos,
                      boneInfo[B_HEAD].worldRot,
                      halfSize,
                      1.f,
                      *bi);
    head = bi->rigidBody;

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
    constructBodyPart(p3, boneInfo[B_UPPER_ARM_L].worldRot, halfSize, 1.f, *bi);

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
    constructBodyPart(p3, boneInfo[B_UPPER_ARM_R].worldRot, halfSize, 1.f, *bi);

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
    constructBodyPart(p3, boneInfo[B_FORE_ARM_L].worldRot, halfSize, 1.f, *bi);

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
    constructBodyPart(p3, boneInfo[B_FORE_ARM_R].worldRot, halfSize, 1.f, *bi);

    // construct the BP_SPINE
    // this is the "TORSO" and should be the size from the spine to the neck
    // and height = from one upper arm to the another one
    bi = &mRagdollBones[BodyPartID::BP_SPINE];
    height = boneInfo[B_UPPER_ARM_R].worldPos.distance(boneInfo[B_UPPER_ARM_L].worldPos);
    width = height * 0.3f;
    d1 = boneInfo[B_SPINE].worldPos.distance(boneInfo[B_NECK].worldPos);
    p3 = (boneInfo[B_SPINE].worldPos + boneInfo[B_NECK].worldPos) * 0.5f; // position of the shape
    bi->offset = -boneInfo[B_SPINE].toLocal(p3);
    halfSize = btVector3(d1, height, width) * 0.5f;
    bi->bone = boneInfo[B_SPINE].bone;
    constructBodyPart(p3, boneInfo[B_SPINE].worldRot, halfSize, 1.f, *bi);

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
    constructBodyPart(p3, boneInfo[B_THIGH_L].worldRot, halfSize, 1.f, *bi);

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
    constructBodyPart(p3, boneInfo[B_THIGH_R].worldRot, halfSize, 1.f, *bi);

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
    constructBodyPart(p3, boneInfo[B_CALF_L].worldRot, halfSize, 1.f, *bi);

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
    constructBodyPart(p3, boneInfo[B_CALF_R].worldRot, halfSize, 1.f, *bi);

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
    mDynamicWorld->addConstraint(coneC, true);

    // spine and upper arm l, for this particular case we need to do a little
    // trick since the upper arm is not very close to the neck, we need to calculate
    // the local for the neck that is moving the neck in the direction of the
    // upper arm.
    localA.setIdentity(); localB.setIdentity();
    localA.getOrigin() += mRagdollBones[BodyPartID::BP_SPINE].offset +
        boneInfo[B_SPINE].toLocal(boneInfo[B_UPPER_ARM_L].worldPos);
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_UPPER_ARM_L].offset;
    coneC = new btConeTwistConstraint(*mRagdollBones[BodyPartID::BP_SPINE].rigidBody,
                                      *mRagdollBones[BodyPartID::BP_UPPER_ARM_L].rigidBody,
                                      localA,
                                      localB);
    coneC->setLimit(M_PI_2, M_PI_2, 0);
    mConstraints[BodyConstraints::BC_LEFT_SHOULDER] = coneC;
    mDynamicWorld->addConstraint(coneC, true);

    // spine and upper arm r. The same than above here.
    localA.setIdentity(); localB.setIdentity();
    localA.getOrigin() += mRagdollBones[BodyPartID::BP_SPINE].offset +
        boneInfo[B_SPINE].toLocal(boneInfo[B_UPPER_ARM_R].worldPos);
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_UPPER_ARM_R].offset;
    coneC = new btConeTwistConstraint(*mRagdollBones[BodyPartID::BP_SPINE].rigidBody,
                                      *mRagdollBones[BodyPartID::BP_UPPER_ARM_R].rigidBody,
                                      localA,
                                      localB);
    coneC->setLimit(M_PI_2, M_PI_2, 0);
    mConstraints[BodyConstraints::BC_RIGHT_SHOULDER] = coneC;
    mDynamicWorld->addConstraint(coneC, true);

    // upper arm l and forearm l
    localA.setIdentity(); localB.setIdentity();
    localA.getOrigin() = mRagdollBones[BodyPartID::BP_UPPER_ARM_L].offset +
        boneInfo[B_UPPER_ARM_L].toLocal(boneInfo[B_FORE_ARM_L].worldPos);
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_FORE_ARM_L].offset;
    hingeC = new btHingeConstraint(*mRagdollBones[BodyPartID::BP_UPPER_ARM_L].rigidBody,
                                      *mRagdollBones[BodyPartID::BP_FORE_ARM_L].rigidBody,
                                      localA,
                                      localB);
    hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
    mConstraints[BodyConstraints::BC_LEFT_ELBOW] = hingeC;
    mDynamicWorld->addConstraint(hingeC, true);

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
    mDynamicWorld->addConstraint(hingeC, true);

    // spine and thigh l, we have to do a little tricky stuff here again...
    //
    localA.setIdentity(); localB.setIdentity();
    localA.getBasis().setEulerZYX(0,0,M_PI);
    localA.getOrigin() = mRagdollBones[BodyPartID::BP_SPINE].offset +
        boneInfo[B_SPINE].toLocal(boneInfo[B_THIGH_L].worldPos);
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_THIGH_L].offset;
    coneC = new btConeTwistConstraint(*mRagdollBones[BodyPartID::BP_SPINE].rigidBody,
                                      *mRagdollBones[BodyPartID::BP_THIGH_L].rigidBody,
                                      localA,
                                      localB);
    coneC->setLimit(M_PI_4, M_PI_2, 0);
    mConstraints[BodyConstraints::BC_LEFT_HIP] = coneC;
    mDynamicWorld->addConstraint(coneC, true);

    // spine and thigh r, the same here
    localA.setIdentity(); localB.setIdentity();
    localA.getBasis().setEulerZYX(0,0,M_PI);
    localA.getOrigin() = mRagdollBones[BodyPartID::BP_SPINE].offset +
         boneInfo[B_SPINE].toLocal(boneInfo[B_THIGH_R].worldPos);
    localB.getOrigin() = mRagdollBones[BodyPartID::BP_THIGH_R].offset;
    coneC = new btConeTwistConstraint(*mRagdollBones[BodyPartID::BP_SPINE].rigidBody,
                                      *mRagdollBones[BodyPartID::BP_THIGH_R].rigidBody,
                                      localA,
                                      localB);
    coneC->setLimit(M_PI_4, M_PI_2, 0);
    mConstraints[BodyConstraints::BC_RIGHT_HIP] = coneC;
    mDynamicWorld->addConstraint(coneC, true);

    // thigh l and leg l
    localA.setIdentity(); localB.setIdentity();
    localA.getOrigin() = mRagdollBones[BodyPartID::BP_THIGH_L].offset +
         boneInfo[B_THIGH_L].toLocal(boneInfo[B_CALF_L].worldPos);
     localB.getOrigin() = mRagdollBones[BodyPartID::BP_LEG_L].offset;
     hingeC = new btHingeConstraint(*mRagdollBones[BodyPartID::BP_THIGH_L].rigidBody,
                                       *mRagdollBones[BodyPartID::BP_LEG_L].rigidBody,
                                       localA,
                                       localB);
    hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
    mConstraints[BodyConstraints::BC_LEFT_KNEE] = hingeC;
    mDynamicWorld->addConstraint(hingeC, true);

    // thigh r and leg r
    localA.setIdentity(); localB.setIdentity();
    localA.getOrigin() = mRagdollBones[BodyPartID::BP_THIGH_R].offset +
         boneInfo[B_THIGH_R].toLocal(boneInfo[B_CALF_R].worldPos);
     localB.getOrigin() = mRagdollBones[BodyPartID::BP_LEG_R].offset;
     hingeC = new btHingeConstraint(*mRagdollBones[BodyPartID::BP_THIGH_R].rigidBody,
                                       *mRagdollBones[BodyPartID::BP_LEG_R].rigidBody,
                                       localA,
                                       localB);
    hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
    mConstraints[BodyConstraints::BC_RIGHT_KNEE] = hingeC;
    mDynamicWorld->addConstraint(hingeC, true);

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
#ifdef DEBUG
    createPrimitives();
#endif

    // construct the constraints now
    if (!setConstraints(boneInfo)) {
        debugERROR("Error setting the constraints\n");
        return false;
    }


/*

    // create the temporary info
    core::StackVector<TempBoneInfo, BP_MAX> tmpInfo;

    // now for each pair of bones we will construct the related box
    for (unsigned int i = 0; i < bodyParts.size(); ++i) {
        const BodyPartInfo& bpi = bodyParts[i];
        Ogre::Bone* root = ogreSkeleton->getBone(bpi.rootBoneName);
        Ogre::Bone* child = ogreSkeleton->getBone(bpi.childBoneName);
        ASSERT(root && child);
        ASSERT(root != child);

        TempBoneInfo tmpBoneInfo;
        tmpBoneInfo.bone = root;
        tmpBoneInfo.mass = bpi.mass;

        // get the position of both joints
        const Ogre::Vector3 rootGlobalPos = globalOgrePosition(root, parentNode);
        const Ogre::Vector3 childGlobalPos = globalOgrePosition(child, parentNode);

        tmpBoneInfo.jointPosition = ogreToBullet(rootGlobalPos);
        tmpBoneInfo.nextJointPosition = ogreToBullet(childGlobalPos);

        // calculate the position of the shape for the rigid body, in this case
        // it will be just in the middle of the two joints
        tmpBoneInfo.shapePos =
            (tmpBoneInfo.jointPosition + tmpBoneInfo.nextJointPosition) * 0.5f;

        // if it is the head then it should be a little more above
        const bool isHead = i == BodyPartID::BP_HEAD;
        if (isHead) {
            btVector3 headOffset = tmpBoneInfo.nextJointPosition - tmpBoneInfo.jointPosition;
            const float len = headOffset.length();
            headOffset.normalize();
            headOffset *= len;
            tmpBoneInfo.shapePos += headOffset * 3.f;
        }

        // construct the associated bounding box in ogre and then calculate
        // the size of the box. This is not necessary but the code was already
        // wrote :p.
        Ogre::AxisAlignedBox bb =
            bbFrom2Pos(rootGlobalPos,
                       childGlobalPos,
                       bpi.height,
                       bpi.width);

        // set the shape size
        tmpBoneInfo.boxHalfSize = ogreToBullet(bb.getHalfSize());

        // get the global orientation of the current root bone.
        //
        tmpBoneInfo.rotation = ogreToBullet(globalOgreRotation(root, parentNode));

        // Now we will set the offset vector. We assume from what we see in the
        // joints of the models that the direction from one joint to another
        // is given by the x axis positive value. So, root points to child
        // in the x root's axis..
        // Note that the offset is not rotated and should be rotated to point
        // in the same direction than root (rotation * offset will give us what
        // we want).
        //
        tmpBoneInfo.offset = btVector3(tmpBoneInfo.boxHalfSize.x(), 0.f, 0.f);

        tmpInfo.push_back(tmpBoneInfo);
    }

    // configure the ragdoll now
    if (!buildRagdollBoneInfo(tmpInfo)) {
        debugERROR("Error building the ragdoll\n");
        return false;
    }

    // construct the constraints now
    if (!setConstraints(tmpInfo)) {
        debugERROR("Error setting the constraints\n");
        return false;
    }

#ifdef DEBUG
    // construct the debug information
    ASSERT(mRagdollBones.size() == mRagdollBones.max_size());
    for (unsigned int i = 0; i < mRagdollBones.size(); ++i) {
        RagdollBoneInfo& ri = mRagdollBones[i];
        core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
        ri.motionState.primitive = pd.createBox(Ogre::Vector3::ZERO,
                                                bulletToOgre(tmpInfo[i].boxHalfSize)*2.f,
                                                pd.getFreshColour());
        btTransform t;
        t.setIdentity();
        t.setRotation(tmpInfo[i].rotation);
        t.setOrigin(tmpInfo[i].shapePos);
        ri.motionState.setWorldTransform(t);
    }
#endif*/

    // everything fine
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
RagDoll::configureRagdoll(const core::StackVector<Ogre::Bone*, BP_MAX>& bones,
                          Ogre::SceneNode* parentNode)
{

}

////////////////////////////////////////////////////////////////////////////////
void
RagDoll::setEnable(bool enable)
{

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
}

} /* namespace physics */
