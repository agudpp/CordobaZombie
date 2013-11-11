/*
 * Weapon.h
 *
 *  Created on: Oct 9, 2013
 *      Author: agustin
 */

#ifndef WEAPON_H_
#define WEAPON_H_

#include <string>
#include <cmath>

#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreAnimationState.h>

#include <debug/DebugUtil.h>
#include <types/StackVector.h>
#include <global_data/GlobalData.h>

#include "WeaponDefs.h"

namespace cz {

// @brief The interface used for each of the weapons of the main character.
//

// Here we will define the main basic information that all the weapons should
// contain.
//
struct WeaponInfo {
    unsigned short maxMagazines;
    unsigned short maxBulletsPerMagazine;
    unsigned short bulletsInMagazine;
    unsigned short magazineCount;
    std::string name;

    WeaponInfo() :
        maxMagazines(0)
    ,   maxBulletsPerMagazine(0)
    ,   bulletsInMagazine(0)
    ,   magazineCount(0)
    {}

};


class Weapon
{
    // Define the maximum number of animations we will support per weapon
    //
    static const unsigned int MAX_ANIMS = 10;
    typedef core::StackVector<Ogre::AnimationState*, MAX_ANIMS> AnimationsVec;

public:
    Weapon() :
        mEntity(0),
        mCurrentAnim(0),
        mOffsetPos(Ogre::Vector3::ZERO),
        mOffsetRot(Ogre::Quaternion::IDENTITY)
    {};
    virtual
    ~Weapon();

    // @brief Set the CrossFire information to be used for all the weapons
    // @param cfi       CrossFireInformation pointer
    //
    static inline void
    setCrossFireInfo(CrossFireInfo* cfi);

    // @brief Set / get the weapon Type ID of this instance
    //
    inline void
    setWeaponID(WeaponID id);
    inline WeaponID
    weaponID(void) const;

    ////////////////////////////////////////////////////////////////////////////
    //                    Ogre / Graphic associated methods                   //
    ////////////////////////////////////////////////////////////////////////////

    // @brief Set / get the associated Entity of this weapon.
    // @param entity    The associated entity.
    // @note ONCE YOU SET THE ENTITY TO THIS CLASS THIS CLASS IS
    //       THE OWNER OF THE MEMORY AND WILL DESTROY THE ENTITY ONCE WE DESTROY
    //       THIS INSTANCE
    //
    inline void
    setOgreStuff(Ogre::Entity* entity);
    inline Ogre::Entity*
    entity(void);

    // @brief Configure / get the rotation offset and translation offset of the
    //        entity we need to apply when we add it to the hand bone of the
    //        player.
    //
    inline void
    configureEntityOffsets(const Ogre::Vector3& posOffset = Ogre::Vector3::ZERO,
                           const Ogre::Quaternion& rotOffset = Ogre::Quaternion::IDENTITY);
    inline const Ogre::Vector3&
    posEntityOffset(void) const;
    inline const Ogre::Quaternion&
    rotEntityOffset(void) const;

    ////////////////////////////////////////////////////////////////////////////
    //                  General methods for all the weapons                   //
    ////////////////////////////////////////////////////////////////////////////

    // @brief Returns the number of bullets that the weapons currently has.
    //        This is the total of bullet taking into account the magazines.
    //
    inline unsigned int
    bulletsCount(void) const;

    // @brief Check if we have more bullets in the current magazine
    //
    inline bool
    hasMoreBulletsInMagazine(void) const;

    // @brief Check if we need to reload the weapon. If we have no more bullets
    //        nor magazines we will be not able to reload the weapon.
    //
    inline bool
    haveToReload(void) const;

    // @brief Get the information associated to this weapon. This is:
    //        - Number of maximum magazines that the weapon can contain.
    //        - Number of bullets per magazine.
    //        - The current number of bullets in the current magazine
    //        - The number of magazines that the weapon currently has.
    //        - The name of the weapon.
    //
    inline const WeaponInfo&
    weaponInfo(void) const;

    ////////////////////////////////////////////////////////////////////////////
    //                  Methods to implement by each weapon                   //
    ////////////////////////////////////////////////////////////////////////////

    // @brief This method will be called to add bullets / magazines
    // @param magazines     The number of magazines we want to add (it is assumed
    //                      that each magazine contains the maximum number of
    //                      bullets)
    // @param bullets       The number of bullets to the current magazine
    //
    virtual void
    addAmmunition(unsigned short magazines, unsigned short bullets) = 0;

    // @brief Check if the current weapon can fire or not (probably because it
    //        hasn't more bullets or because it is in the middle of any process).
    //
    virtual bool
    canFire(void) const = 0;

    // @brief Check if we can reload right now (probably we cannot because we
    //        are firing / already loading / no more bullets / any).
    //
    virtual bool
    canReload(void) const = 0;

    // @brief This method will be called to reload the current weapon.
    // @return true if it is able to reload | false otherwise
    //
    virtual bool
    reload(void) = 0;

    // @brief This method will fire the weapon and return true if it could do it
    //        or false otherwise
    // @return true if it could | false otherwise
    //
    virtual bool
    fire(void) = 0;

    // @brief This method should be called each frame to update the logic of the
    //        weapon (this implies the current animation / etc).
    //
    virtual void
    update(float timeFrame) = 0;

    // @brief Perform any particular operation to this weapon. This could be
    //        used to activate / deactivate specific features to the weapon.
    // @param feature       The feature argument, for now this will be an int...
    //
    virtual void
    setFeature(WFeature feature) = 0;

protected:
    // Some helper methods

    // @brief Reload the current weapon decreasing the bullets / magazines as
    //        needed.
    //        This will only modify the WeaponInformation, it will not perform
    //        any other operation.
    // @return true if we could do it | false otherwise
    //
    inline bool
    performReload(void);

    // @brief Remove one bullet from  the current weapon modifying the magazines
    //        and bullets as needed
    //        This will only modify the WeaponInformation, it will not perform
    //        any other operation.
    // @return true if we could do it | false otherwise
    //
    inline bool
    performFire(void);

    // @brief Add ammunition to the current weapon. We will only modify the
    //        WeaponInfo structure, nothing else.
    // @param magazines     The num of magazines we want to add.
    // @param bullets       The num of bullets we want to add to the current
    //                      magazine
    // @return true on success || false otherwise
    //
    inline bool
    performAddAmmunition(unsigned short magazines, unsigned short bullets);

    ////////////////////////////////////////////////////////////////////////////
    //                          Handling animations                           //
    ////////////////////////////////////////////////////////////////////////////

    // @brief Configure the animation table from a list of strings and using
    //        the current entity.
    // @param names     The list of names in the ORDER we want to add them
    //                  into the table. The names should be terminated by a null
    //                  pointer.
    //
    inline void
    loadAnims(const char** names);

    // @brief Stop the current animation
    //
    inline void
    stopCurrentAnim(void);

    // @brief Check if the current animation has ended.
    // @return true if no animation is set or if the current one has already end
    //         false otherwise
    //
    inline bool
    currentAnimEnded(void) const;

    // @brief Start a new animation stopping the current one first.
    // @param animIndex     The animation index to start
    //
    inline void
    startAnimation(unsigned int animIndex);


protected:
    WeaponInfo mInfo;
    Ogre::Entity* mEntity;
    AnimationsVec mAnimations;
    Ogre::AnimationState* mCurrentAnim;
    WeaponID mID;
    Ogre::Vector3 mOffsetPos;
    Ogre::Quaternion mOffsetRot;

    // define the CrossFire information to be used for all the weapons
    static CrossFireInfo* sCrossFire;


private:
    // avoid copying
    Weapon(const Weapon&);
    Weapon& operator=(const Weapon&);

};




////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
Weapon::setCrossFireInfo(CrossFireInfo* cfi)
{
    ASSERT(cfi);
    sCrossFire = cfi;
}

inline bool
Weapon::performReload(void)
{
    if (mInfo.magazineCount == 0) {
        return false;
    }
    --(mInfo.magazineCount);
    return true;
}

inline bool
Weapon::performFire(void)
{
    if (mInfo.bulletsInMagazine == 0) {
        if (mInfo.magazineCount == 0) {
            return false;
        }
        // else, we need to decrement the magazine
        --(mInfo.magazineCount);
        mInfo.bulletsInMagazine = mInfo.maxBulletsPerMagazine;
        return true;
    }
    // else we just decrement the number of bullets
    --(mInfo.bulletsInMagazine);
    return true;
}


inline void
Weapon::loadAnims(const char** names)
{
    ASSERT(names);
    ASSERT(mEntity);

    mAnimations.clear();
    mCurrentAnim = 0;

    // we will be not robust here, we will crash if we cannot find an animation
    //
    Ogre::AnimationStateSet* animSet = mEntity->getAllAnimationStates();
    ASSERT(animSet);
    while (*names != 0) {
        ASSERT(animSet->hasAnimationState(*names));
        mAnimations.push_back(animSet->getAnimationState(*names));
        ++names;
    }
}

inline void
Weapon::stopCurrentAnim(void)
{
    if (mCurrentAnim) {
        mCurrentAnim->setEnabled(false);
    }
    mCurrentAnim = 0;
}

inline bool
Weapon::currentAnimEnded(void) const
{
    return (mCurrentAnim == 0) || mCurrentAnim->hasEnded();
}

inline void
Weapon::startAnimation(unsigned int animIndex)
{
    // stop the current one
    stopCurrentAnim();
    mCurrentAnim = mAnimations[animIndex];
    mCurrentAnim->setTimePosition(0);
    mCurrentAnim->setEnabled(true);
}

inline
Weapon::~Weapon()
{
    if (mEntity) {
        GlobalData::sceneMngr->destroyEntity(mEntity);
    }
}


inline void
Weapon::setWeaponID(WeaponID id)
{
    mID = id;
}
inline WeaponID
Weapon::weaponID(void) const
{
    return mID;
}

inline void
Weapon::setOgreStuff(Ogre::Entity* entity)
{
    mEntity = entity;
}
inline Ogre::Entity*
Weapon::entity(void)
{
    return mEntity;
}

inline void
Weapon::configureEntityOffsets(const Ogre::Vector3& posOffset,
                               const Ogre::Quaternion& rotOffset)
{
    mOffsetPos = posOffset;
    mOffsetRot = rotOffset;
}
inline const Ogre::Vector3&
Weapon::posEntityOffset(void) const
{
    return mOffsetPos;
}
inline const Ogre::Quaternion&
Weapon::rotEntityOffset(void) const
{
    return mOffsetRot;
}

inline bool
Weapon::performAddAmmunition(unsigned short magazines, unsigned short bullets)
{
    mInfo.magazineCount = std::min(
    		static_cast<unsigned int>(magazines + mInfo.magazineCount),
    		static_cast<unsigned int>(mInfo.maxMagazines));
    mInfo.bulletsInMagazine = std::min(
    		static_cast<unsigned int>(bullets + mInfo.bulletsInMagazine),
    		static_cast<unsigned int>(mInfo.maxBulletsPerMagazine));
    return true;
}

inline unsigned int
Weapon::bulletsCount(void) const
{
    return mInfo.magazineCount * mInfo.maxBulletsPerMagazine + mInfo.bulletsInMagazine;
}

inline bool
Weapon::hasMoreBulletsInMagazine(void) const
{
    return (mInfo.bulletsInMagazine > 0);
}

inline bool
Weapon::haveToReload(void) const
{
    return !hasMoreBulletsInMagazine() && (mInfo.magazineCount > 0);
}

inline const WeaponInfo&
Weapon::weaponInfo(void) const
{
    return mInfo;
}

} /* namespace cz */
#endif /* WEAPON_H_ */
