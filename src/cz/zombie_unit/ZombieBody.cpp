/*
 * ZombieBody.cpp
 *
 *  Created on: Oct 22, 2013
 *      Author: agustin
 */

#include "ZombieBody.h"

namespace cz {


////////////////////////////////////////////////////////////////////////////////

void
ZombieBody::ExtirpationTable::build(Ogre::Entity* entity)
{
    ASSERT(entity);
    debugERROR("Hardcoded values for the body part, we probably can load this "
        "from a table or other data structure outside of this class\n");

    // we will create the table using the diagram in the DropBox. This will be
    // hardcoded for now
    //
    unsigned int count = 0;

    // Fore arm L
    {
        ExtirpationInfo& ei = mTable[count++];
        ei.bp = BodyPart::BP_FORE_ARM_L;
        ei.node.hidedPart = entity->getSubEntity("antei2");
        ei.node.normalPart = entity->getSubEntity("antei");
    }

    // Upper arm L
    {
        ExtirpationInfo& ei = mTable[count++];
        ei.bp = BodyPart::BP_UPPER_ARM_L;
        ei.node.hidedPart = entity->getSubEntity("brazoi2");
        ei.node.normalPart = entity->getSubEntity("brazoi");
        ei.childs.push_back(&(mTable[count-2]));
    }

    // Fore arm R
    {
        ExtirpationInfo& ei = mTable[count++];
        ei.bp = BodyPart::BP_FORE_ARM_R;
        ei.node.hidedPart = entity->getSubEntity("anted2");
        ei.node.normalPart = entity->getSubEntity("anted");
    }

    // Upper arm R
    {
        ExtirpationInfo& ei = mTable[count++];
        ei.bp = BodyPart::BP_UPPER_ARM_R;
        ei.node.hidedPart = entity->getSubEntity("brazod2");
        ei.node.normalPart = entity->getSubEntity("brazod");
        ei.childs.push_back(&(mTable[count-2]));
    }

    // head
    {
        ExtirpationInfo& ei = mTable[count++];
        ei.bp = BodyPart::BP_HEAD;
        ei.node.hidedPart = entity->getSubEntity("cabeza2");
        ei.node.normalPart = entity->getSubEntity("cabeza");
    }
}

void
ZombieBody::ExtirpationTable::reset(void)
{
    for (ExtirpationInfo& ei : mTable) {
        ei.node.reset();
        for (ExtirpationInfo*& en : ei.childs) {
            en->node.reset();
        }
    }
}

bool
ZombieBody::ExtirpationTable::extirpate(BodyPart bp, physics::BodyPartMask& mask)
{
    // search for that body part and then extirpate it and full extirpate all
    // its childrens
    for (ExtirpationInfo& ei : mTable) {
        if (ei.bp == bp) {
            ei.node.extirpate();
            mask[bp] = false;
            for (ExtirpationInfo*& en : ei.childs) {
                en->node.fullExtirtpation();
                mask[en->bp] = false;
            }
            return true;
        }
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ZombieBody::ZombieBody() :
    mRagdollQueue(0)
,   mBodyPartQueue(0)
,   mSkeleton(0)
,   mNode(0)
,   mEntity(0)
,   mCurrentRagdoll(0)
{
}
////////////////////////////////////////////////////////////////////////////////
ZombieBody::~ZombieBody()
{
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieBody::build(void)
{
    // ensure everything is already set
    ASSERT(mRagdollQueue);
    ASSERT(mBodyPartQueue);
    ASSERT(mSkeleton);
    ASSERT(mEntity);
    ASSERT(mNode);

    // Build the bone table
    debugERROR("We are hardcoding the BoneTable.\n");
    mBoneTable.resize(mBoneTable.max_size());
    mBoneTable[physics::BonesID::B_HEAD] = mSkeleton->getBone("B_HEAD");
    mBoneTable[physics::BonesID::B_NECK] = mSkeleton->getBone("B_NECK");
    mBoneTable[physics::BonesID::B_CLAVICLE_R] = mSkeleton->getBone("B_CLAVICLE_R");
    mBoneTable[physics::BonesID::B_CLAVICLE_L] = mSkeleton->getBone("B_CLAVICLE_L");
    mBoneTable[physics::BonesID::B_UPPER_ARM_R] = mSkeleton->getBone("B_UPPER_ARM_R");
    mBoneTable[physics::BonesID::B_UPPER_ARM_L] = mSkeleton->getBone("B_UPPER_ARM_L");
    mBoneTable[physics::BonesID::B_FORE_ARM_R] = mSkeleton->getBone("B_FORE_ARM_R");
    mBoneTable[physics::BonesID::B_FORE_ARM_L] = mSkeleton->getBone("B_FORE_ARM_L");
    mBoneTable[physics::BonesID::B_HAND_R] = mSkeleton->getBone("B_HAND_R");
    mBoneTable[physics::BonesID::B_HAND_L] = mSkeleton->getBone("B_HAND_L");
    mBoneTable[physics::BonesID::B_SPINE] = mSkeleton->getBone("B_SPINE");
    mBoneTable[physics::BonesID::B_PELVIS] = mSkeleton->getBone("B_PELVIS");
    mBoneTable[physics::BonesID::B_THIGH_R] = mSkeleton->getBone("B_THIGH_R");
    mBoneTable[physics::BonesID::B_THIGH_L] = mSkeleton->getBone("B_THIGH_L");
    mBoneTable[physics::BonesID::B_CALF_R] = mSkeleton->getBone("B_CALF_R");
    mBoneTable[physics::BonesID::B_CALF_L] = mSkeleton->getBone("B_CALF_L");
    mBoneTable[physics::BonesID::B_FOOT_R] = mSkeleton->getBone("B_FOOT_R");
    mBoneTable[physics::BonesID::B_FOOT_L] = mSkeleton->getBone("B_FOOT_L");

    // build the Extirpation table
    mExtirpationTable.build(mEntity);

    // reset everything
    resetBody();
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieBody::resetBody(void)
{
    // reset any ragdoll stuff
    setRagdollEnable(false);

    // all the body parts are available
    mBodyMask.set();

    // show all the parts again
    mExtirpationTable.reset();
}

////////////////////////////////////////////////////////////////////////////////
bool
ZombieBody::checkIntersection(const Ogre::Ray& ray,
                              Ogre::Vector3& intPoint,
                              BodyPart& bp) const
{
    ASSERT(mRagdollQueue);

    // pick any ragdoll to use since all of them should have the same dimensions
    // this will be fine to do. Note that is not necessary to use the ragdoll
    // itself, we only want to calculate the possible intersection configuring
    // some boxes using the skeleton.
    physics::RagDoll* ragdoll = mRagdollQueue->last();
    if (ragdoll == 0) {
        debugERROR("WTF, we will need to implement another method to be able "
            "ALWAYS to get an instance\n");
        ASSERT(ragdoll);
        return false;
    }

    // check the intersection using the ragdoll
    return ragdoll->getClosestIntersection(mBoneTable,
                                           mNode,
                                           ray,
                                           mBodyMask,
                                           intPoint,
                                           bp);
}

////////////////////////////////////////////////////////////////////////////////
physics::BulletObject*
ZombieBody::extirpate(BodyPart bodyPart)
{
    // we will remove the body part and return the associated body part
    if (mBodyMask[bodyPart] == false) {
        // that bodypart doesn't exists anymore
        return 0;
    }
    if (!mExtirpationTable.extirpate(bodyPart, mBodyMask)) {
        // we couldn't extirpate that part!
        return 0;
    }

    // we could extirpate and we already mark the parts as removed, so get the
    // BulletObject from the BodyPartQueue now
    debugERROR("TODO: here we need to get the BulletObject from the queue\n");
    return 0;
}

} /* namespace cz */