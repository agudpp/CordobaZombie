/*
 * MainPlayer.h
 *
 *  Created on: Oct 10, 2013
 *      Author: agustin
 */

#ifndef MAINPLAYER_H_
#define MAINPLAYER_H_

#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreBone.h>
#include <OgreAnimationState.h>
#include <OgreOverlay.h>
#include <OgreOverlayContainer.h>

#include <types/StackVector.h>
#include <main_player/weapon/Weapon.h>
#include <main_player/weapon/WeaponDefs.h>
#include <main_player/MainPlayerWeaponEvent.h>
#include <main_player/camera_controller/CameraController.h>
#include <global_data/GlobalData.h>


namespace cz {

class MainPlayer
{
    // The states of the player
    //
    enum State {
        S_IDLE,
        S_FIRING,
        S_RELOADING,
        S_CHANGING_W,
    };

    enum Event {
        E_DONE = 0,
        E_CHANGE_W,
        E_FIRE,
        E_RELOAD,
    };

    // This will be the animations of the hands for each weapon
    //
    enum AnimIndex {
        A_FIRE = 0,
        A_RELOAD,
        A_IDLE_1,
        A_IDLE_2,
        A_IDLE_3,
        A_PUT_IN,
        A_PUT_OUT,

        A_COUNT,
    };


    // Define the maximum number of animations that the player could handle per
    // weapon for all the weapons
    //
    static const unsigned int MAX_NUM_ANIMS = WID_COUNT * A_COUNT;

public:

    // Helper class to set the limits of the camera
    struct CamLimit {
        Ogre::Radian pitchMax;
        Ogre::Radian pitchMin;
        Ogre::Radian rollMax;
        Ogre::Radian rollMin;
    };

public:
    MainPlayer();
    ~MainPlayer();

    // @brief Build the player. This method should be called once and before using
    //        the player.
    //        We will need the camera
    // @param camera        The camera
    // @param camLimits     The camera movement limits (rotation).
    //
    void
    build(Ogre::Camera* camera,
          const CamLimit& camLimits);


    ////////////////////////////////////////////////////////////////////////////
    // Ogre / world methods

    // @brief Return the base scene node used to move the player and rotate
    //        them.
    //
    inline Ogre::SceneNode*
    sceneNode(void);
    inline Ogre::Entity*
    entity(void);

    // @brief Set the position of the player
    // @param pos       The position
    //
    inline void
    setPosition(const Ogre::Vector3& pos);

    // @brief Set the direction of the sight we want to have (look at) from the
    //        current position.
    // @param direction     The direction we want to see from the current position
    //
    void
    lookInDirection(const Ogre::Vector3& direction);

    // @brief Rotate the field of sight of the player.
    // @param roll      Roll radians to rotate
    // @param pitch     Pitch radians to rotate
    //
    inline void
    rotateFieldOfSight(Ogre::Radian roll, Ogre::Radian pitch);

    // @brief Show / hide the cross
    //
    inline void
    showCross(bool show);

    ////////////////////////////////////////////////////////////////////////////
    // Weapon handling

    // @brief Add a weapon to the player. If the weapon already exists we return
    //        false and the weapon is not added.
    // @param weapon        The weapon we want to add
    // @return true if we could add the weapon, false otherwise
    //
    bool
    addWeapon(Weapon* weapon);

    // @brief Get current weapon ID used by the player
    //
    inline WeaponID
    currentWeaponID(void) const;

    // @brief Get the current weapon used by the player
    //
    inline Weapon*
    currentWeapon(void);

    // @brief Change the current weapon to another one
    // @param wid       The weapon id we want to use
    //
    void
    changeWeapon(WeaponID wid);

    // @brief Add ammunitions to a given weapon ID
    // @param magazines     The number of magazines to add to the weapon
    // @param wid           The weapon id we want to add the magazines
    //
    void
    addAmmunition(unsigned short magazines, WeaponID wid);

    // @brief Fire the current weapon. This method will be called everytime
    //        we press the fire button, so doesn't mean we will fire (probably
    //        we cannot, or we are doing any other thing like reloading).
    //
    void
    doFire(void);

    // @brief Reload the current weapon. This will be called everytime we press
    //        the "reload" button but doesn't means that we will reload the weapon
    //        everytime.
    //
    void
    doReload(void);

    // @brief Perform special action /feature over the current weapon.
    // @param feature       The feature
    //
    void
    activateFeature(WFeature feature);

    ////////////////////////////////////////////////////////////////////////////
    // General methods
    //

    // @brief Add signal system here to show the events like (fire a weapon /
    //        reload a weapon / change a weapon / life change?)
    // TODO: issue #157
    void
    setEventCallback(const WeaponEventCb& cb);

    // @brief Update the internal logic of the player.
    //
    void
    update(void);

private:

    // @brief Load the animations and the animation indices for each weapon
    //
    void
    loadAnimations(void);

    // @brief Get the hand bone from the skeleton of the current entity
    //
    void
    getHandBone(void);

    // @brief Configure the position of the cross overlay (weapon cross) in
    //        the middle of the screen probably.
    //        We will set the current position of the overlay in the CrossFireInfo
    //
    void
    configureCrossFire(void);

    // @brief Configure camera controller for the player
    //
    void
    configureCameraController(const CamLimit& camLimits);

    // @brief Set an specific weapon to the player (this will detach the
    //        old weapon from the HandBone and will attach the new one to it).
    //        This method it will also configure the weapon to be used right now.
    // @param weapon        The new weapon we want to use.
    //
    void
    configurePlayerWeapon(Weapon* weapon);

    // @brief Stop the current animation
    //
    inline void
    stopCurrentAnim(void);

    // @brief Start a new animation stopping the current one first.
    // @param animIndex     The animation index to start
    //
    inline void
    startAnimation(unsigned int animIndex);

    // @brief This method will be called everytime a new event occur, this will
    //        configure the next state to be used as well as we will do all the
    //        things we need to.
    // @param event         The event occurring
    //
    void
    newEvent(Event event);

private:
    Ogre::SceneNode* mBaseNode;
    Ogre::Entity* mHandsEntity;
    Ogre::Bone* mHandBone;

    CameraController mCamController;

    Ogre::Overlay* mCrossOverlay;
    CrossFireInfo mCrossFireInfo;

    Weapon* mWeapons[WeaponID::WID_COUNT];
    Weapon* mCurrentWeapon;
    Weapon* mWeaponToUse;

    Ogre::AnimationState* mCurrentAnim;
    Ogre::AnimationState* mAnimations[MAX_NUM_ANIMS];
    unsigned int mAnimOffsets[WeaponID::WID_COUNT];

    State mState;

    unsigned int mLife;

    WeaponEventCb mWeaponCallback;
};








////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
MainPlayer::stopCurrentAnim(void)
{
    if (mCurrentAnim) {
        mCurrentAnim->setEnabled(false);
    }
    mCurrentAnim = 0;
}

inline void
MainPlayer::startAnimation(unsigned int animIndex)
{
    // stop the current one
    stopCurrentAnim();
    mCurrentAnim = mAnimations[animIndex];
    mCurrentAnim->setTimePosition(0);
    mCurrentAnim->setEnabled(true);
}


inline void
MainPlayer::setPosition(const Ogre::Vector3& pos)
{
    ASSERT(mBaseNode);
    mBaseNode->setPosition(pos);
}

inline void
MainPlayer::rotateFieldOfSight(Ogre::Radian roll, Ogre::Radian pitch)
{
    mCamController.rotate(roll, pitch);
}

inline void
MainPlayer::showCross(bool show)
{
    ASSERT(mCrossOverlay);
    if (show) {
        mCrossOverlay->show();
    } else {
        mCrossOverlay->hide();
    }
}


inline WeaponID
MainPlayer::currentWeaponID(void) const
{
    return mCurrentWeapon ? mCurrentWeapon->weaponID() : WeaponID::WID_INVALID;
}

inline Weapon*
MainPlayer::currentWeapon(void)
{
    return mCurrentWeapon;
}


inline void
MainPlayer::setEventCallback(const WeaponEventCb& cb)
{
	mWeaponCallback = cb;
}


} /* namespace cz */
#endif /* MAINPLAYER_H_ */
