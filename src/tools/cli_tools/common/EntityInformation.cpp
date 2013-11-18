/*
 * EntityInformation.cpp
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#include "EntityInformation.h"

#include <OgreBone.h>
#include <OgreSkeleton.h>
#include <OgreAnimationState.h>

#include <debug/DebugUtil.h>

namespace tool {


////////////////////////////////////////////////////////////////////////////////
void
EntityInformation::parseSkeletonInformation(void)
{
    ASSERT(mEntity);
    mBoneNames.clear();

    Ogre::SkeletonInstance* skeleton = mEntity->getSkeleton();
    if (!skeleton) {
        return;
    }

    Ogre::Skeleton::BoneIterator bi = skeleton->getBoneIterator();
    while (bi.hasMoreElements()) {
        Ogre::Bone* bone = bi.getNext();
        ASSERT(bone);
        mBoneNames.push_back(bone->getName());
    }
}

////////////////////////////////////////////////////////////////////////////////
void
EntityInformation::parseAnimInformation(void)
{
    ASSERT(mEntity);
    mAnimNames.clear();

    Ogre::AnimationStateSet* set = mEntity->getAllAnimationStates();
    Ogre::AnimationStateIterator anims = set->getAnimationStateIterator();
    while (anims.hasMoreElements()) {
        Ogre::AnimationState* anim = anims.getNext();
        ASSERT(anim);
        mAnimNames.push_back(anim->getAnimationName());
    }
}

////////////////////////////////////////////////////////////////////////////////
void
EntityInformation::addToStream(const Ogre::String& title,
                               const Ogre::StringVector& vec,
                               std::stringstream& stream) const
{
    stream << title << "\n";
    for (unsigned int i = 0; i < vec.size(); ++i) {
        stream << "\t" << vec[i] << "\n";
    }
    stream << "\n";
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
EntityInformation::EntityInformation()
{
}

////////////////////////////////////////////////////////////////////////////////
EntityInformation::~EntityInformation()
{
}

////////////////////////////////////////////////////////////////////////////////
bool
EntityInformation::parseResource(const Ogre::String& entName,
                                 Ogre::SceneManager* sceneMngr)
{
    ASSERT(sceneMngr);
    mEntity = sceneMngr->createEntity(entName);
    if (!mEntity) {
        debugERROR("We couldn't find the entity %s\n", entName.c_str());
        return false;
    }
    mName = entName;

    parseSkeletonInformation();
    parseAnimInformation();

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
EntityInformation::getInformation(std::stringstream& stream) const
{
    stream << "Entity Name: " << mName << "\n"
        "Resource Location: " << mResourcePath << "\n";

    addToStream("Bone information:", mBoneNames, stream);
    addToStream("Animation information:", mAnimNames, stream);

}

} /* namespace tool */
