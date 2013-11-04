/*
 * RagDollQueue.h
 *
 *  Created on: Oct 22, 2013
 *      Author: agustin
 */

#ifndef RAGDOLLQUEUE_H_
#define RAGDOLLQUEUE_H_

#include <OgreSkeletonInstance.h>

#include <debug/DebugUtil.h>
#include <physics/RagDoll.h>
#include <types/StackQueue.h>

#include "ResourceQueue.h"

// forward
//
class btDynamicsWorld;

namespace cz {

template<unsigned int NUM_INSTANCES = 10>
class RagDollQueue : public ResourceQueue<NUM_INSTANCES, physics::RagDoll>
{
public:
    // @brief Configre all the ragdoll instances and construct them from a
    //        particular skeleton.
    // @param dw            The dynamic world instance needed for the ragdolls
    // @param skeleton      The base skeleton instance to be used for all the ragdolls
    // @param parentNode    The parent scene node where the skeleton is attached to
    //                      [Probably this is not needed, TODO]
    // @return true on success | false otherwise
    //
    inline bool
    configure(btDynamicsWorld* dw,
              const Ogre::SkeletonInstance* skeleton,
              const Ogre::SceneNode* parentNode)
    {
        ASSERT(dw);
        ASSERT(skeleton);

        // construct the table
        physics::BoneTable boneTable;
        boneTable.resize(boneTable.max_size());
        boneTable[physics::BonesID::B_HEAD] = skeleton->getBone("B_HEAD");
        boneTable[physics::BonesID::B_NECK] = skeleton->getBone("B_NECK");
        boneTable[physics::BonesID::B_CLAVICLE_R] = skeleton->getBone("B_CLAVICLE_R");
        boneTable[physics::BonesID::B_CLAVICLE_L] = skeleton->getBone("B_CLAVICLE_L");
        boneTable[physics::BonesID::B_UPPER_ARM_R] = skeleton->getBone("B_UPPER_ARM_R");
        boneTable[physics::BonesID::B_UPPER_ARM_L] = skeleton->getBone("B_UPPER_ARM_L");
        boneTable[physics::BonesID::B_FORE_ARM_R] = skeleton->getBone("B_FORE_ARM_R");
        boneTable[physics::BonesID::B_FORE_ARM_L] = skeleton->getBone("B_FORE_ARM_L");
        boneTable[physics::BonesID::B_HAND_R] = skeleton->getBone("B_HAND_R");
        boneTable[physics::BonesID::B_HAND_L] = skeleton->getBone("B_HAND_L");
        boneTable[physics::BonesID::B_SPINE] = skeleton->getBone("B_SPINE");
        boneTable[physics::BonesID::B_PELVIS] = skeleton->getBone("B_PELVIS");
        boneTable[physics::BonesID::B_THIGH_R] = skeleton->getBone("B_THIGH_R");
        boneTable[physics::BonesID::B_THIGH_L] = skeleton->getBone("B_THIGH_L");
        boneTable[physics::BonesID::B_CALF_R] = skeleton->getBone("B_CALF_R");
        boneTable[physics::BonesID::B_CALF_L] = skeleton->getBone("B_CALF_L");
        boneTable[physics::BonesID::B_FOOT_R] = skeleton->getBone("B_FOOT_R");
        boneTable[physics::BonesID::B_FOOT_L] = skeleton->getBone("B_FOOT_L");

        for (unsigned int i = 0; i < NUM_INSTANCES; ++i) {
            physics::RagDoll& r = this->mElements[i];
            r.setDynamicWorld(dw);
            if (!r.buildFromSkeleton(boneTable, parentNode)) {
                debugERROR("Some error occur trying to configure one of the ragdolls\n");
                return false;
            }
        }

        return true;
    }
};



////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//
/*
template<unsigned int NUM_INSTANCES>
inline bool
RagDollQueue<NUM_INSTANCES>::configure(btDynamicsWorld* dw,
                                       const Ogre::SkeletonInstance* skeleton,
                                       const Ogre::SceneNode* parentNode)
*/


} /* namespace cz */
#endif /* RAGDOLLQUEUE_H_ */
