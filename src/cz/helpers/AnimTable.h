/*
 * AnimTable.h
 *
 *  Created on: Sep 9, 2013
 *      Author: agustin
 */

#ifndef ANIMTABLE_H_
#define ANIMTABLE_H_

#include <bitset>

#include <OgreAnimationState.h>
#include <OgreEntity.h>

#include <types/StackVector.h>
#include <debug/DebugUtil.h>

namespace cz {

template<unsigned int NUM_ANIMS>
class AnimTable
{
public:

    inline AnimTable(){};

    // @brief Configure the animation table from a list of strings and an
    //        entity.
    // @param names     The list of names in the order we want to add them
    //                  into the table. The names should be terminated by a null
    //                  pointer.
    // @param entity    The entity.
    // @param loop      The initial loop value for the animations
    //
    inline void
    loadAnims(const char** names,
              Ogre::Entity* entity,
              bool loop = false);

    // @brief Start to reproduce one particular animation. This method will
    //        put the animation in the list of "Running animations".
    // @param animID    The animation ID (index)
    // @param enabled   Mark the animation as enabled?
    // @param resetTime Set the time position at 0?
    //
    inline void
    startAnim(unsigned int animID, bool enabled = true, bool resetTime = true);

    // @brief Check if an animation is already active
    // @param animID    The animation ID (index)
    // @return true if the animation is active (started) or false otherwise
    //
    inline bool
    isAnimStarted(unsigned int animID) const;

    // @brief Get a particular animation to modify / get information.
    // @param animID    The animation index.
    //
    inline Ogre::AnimationState*
    operator[](unsigned int animID);

    // @brief Stop a particular animation
    // @param animID    The animation index
    // @note that this will set the animation as disabled
    //
    inline void
    stopAnim(unsigned int animID);

    // @brief Stop all active animations.
    //
    inline void
    stopAll(void);

    // @brief This method will update all the animations until the animation
    //        has ended.
    // @param timeFrame The time frame passed.
    //
    inline void
    update(float timeFrame);

private:
    struct ActiveAnim {
        Ogre::AnimationState* anim;
        unsigned short index;
        ActiveAnim(Ogre::AnimationState* a = 0, unsigned short i = 0) :
            anim(a), index(i)
        {}
    };

    std::bitset<NUM_ANIMS> mActiveMask;
    core::StackVector<Ogre::AnimationState*, NUM_ANIMS> mAnims;
    // we will make the assumption that we will never play the half of the
    // animations at the same time...
    core::StackVector<ActiveAnim, NUM_ANIMS/2> mActiveAnims;
};



////////////////////////////////////////////////////////////////////////////////
//                              INLINE STUFF                                  //
////////////////////////////////////////////////////////////////////////////////

template<unsigned int NUM_ANIMS>
inline void
AnimTable<NUM_ANIMS>::loadAnims(const char** names,
                                Ogre::Entity* entity,
                                bool loop)
{
    ASSERT(names);
    ASSERT(entity);

    mAnims.clear();
    mActiveAnims.clear();
    mActiveMask.reset();

    // we will be not robust here, we will crash if we cannot find an animation
    //
    Ogre::AnimationStateSet* animSet = entity->getAllAnimationStates();
    ASSERT(animSet);
    unsigned int i = 0;
    while (names[i] != 0) {
        ASSERT(animSet->hasAnimationState(names[i]));
        Ogre::AnimationState* anim = animSet->getAnimationState(names[i]);
        mAnims.push_back(anim);
        mActiveMask[i] = anim->getEnabled();
        anim->setLoop(loop);
        ++i;
    }
}

////////////////////////////////////////////////////////////////////////////////
template<unsigned int NUM_ANIMS>
inline void
AnimTable<NUM_ANIMS>::startAnim(unsigned int animID, bool enabled, bool resetTime)
{
    Ogre::AnimationState* anim = mAnims[animID];
    anim->setEnabled(enabled);
    if (resetTime) {
        anim->setTimePosition(0);
    }
    // add it if it should be enabled
    if (enabled && !isAnimStarted(animID)) {
        // ensure that the animation is not already there
        mActiveAnims.push_back(ActiveAnim(anim, animID));
        mActiveMask[animID] = 1;
    }
}

////////////////////////////////////////////////////////////////////////////////
template<unsigned int NUM_ANIMS>
inline bool
AnimTable<NUM_ANIMS>::isAnimStarted(unsigned int animID) const
{
    return mActiveMask[animID];
}

////////////////////////////////////////////////////////////////////////////////
template<unsigned int NUM_ANIMS>
inline Ogre::AnimationState*
AnimTable<NUM_ANIMS>::operator[](unsigned int animID)
{
    return mAnims[animID];
}

template<unsigned int NUM_ANIMS>
inline void
AnimTable<NUM_ANIMS>::stopAnim(unsigned int animID)
{
    Ogre::AnimationState* anim = mAnims[animID];
    unsigned int i = 0;
    for (ActiveAnim* beg = mActiveAnims.begin(), *end = mActiveAnims.end();
            beg != end && (beg->anim != anim); ++beg, ++i) {
    }
    if (i < mActiveAnims.size()) {
        mActiveAnims.disorder_remove(i);
        anim->setEnabled(false);
        mActiveMask[animID] = 0;
    }
}

template<unsigned int NUM_ANIMS>
inline void
AnimTable<NUM_ANIMS>::stopAll(void)
{
    for (ActiveAnim* beg = mActiveAnims.begin(), *end = mActiveAnims.end();
            beg != end; ++beg) {
        beg->anim->setEnabled(false);
    }
    mActiveAnims.clear();
    mActiveMask.reset();
}

////////////////////////////////////////////////////////////////////////////////
template<unsigned int NUM_ANIMS>
inline void
AnimTable<NUM_ANIMS>::update(float timeFrame)
{
    for (unsigned int i = 0; i < mActiveAnims.size(); ++i) {
        ActiveAnim& anim = mActiveAnims[i];
        if (anim.anim->hasEnded() || !anim.anim->getEnabled()) {
            // remove this one
            anim.anim->setEnabled(false);
            mActiveMask[anim.index] = 0;
            mActiveAnims.disorder_remove(i);
            --i;
        } else {
            // update it
            anim.anim->addTime(timeFrame);
        }
    }
}

} /* namespace cz */
#endif /* ANIMTABLE_H_ */
