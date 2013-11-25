/*
 * MainPlayer.cpp
 *
 *  Created on: Oct 10, 2013
 *      Author: agustin
 */

#include "MainPlayer.h"

#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlay.h>


////////////////////////////////////////////////////////////////////////////////
// Defines for the player, here we are hardcoding the values, probably it will be
// better if we avoid hardcoding values and use an xml to configure the
// player, see issue #160
//
#define MAINPLAYER_HANDS_ENTITY         "manos.mesh"
#define MAINPLAYER_HANDS_BONE           "palma"
#define MAINPLAYER_HEAD_BONE            "B_HEAD"
#define MAINPLAYER_CROSSFIRE_OVERLAY    "CrossFireOverlay"
#define MAINPLAYER_CAM_VEL_FACTOR       200.f



namespace cz {

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::loadAnimations(void)
{
    // hardcoded values here
    static const char* ANIM_NAMES[A_COUNT] = {
        "dispara",
        "recarga",
        "quieto",
        "quieto2",
        "quieto3",
        "guarda",
        "saca",
    };
    static const char* WEAPON_NAMES[WeaponID::WID_COUNT] = {
        "9mm",
        "fal"
    };

    debugWARNING("We are hardcoding values here also (issue #160), probably it will "
        "be better if we parse this from an xml file\n");

    ASSERT(mHandsEntity);
    unsigned int count = 0;
    Ogre::String animName;
    for (unsigned int i = 0; i < WeaponID::WID_COUNT; ++i) {
        mAnimOffsets[i] = count;
        for (unsigned int j = 0; j < A_COUNT; ++j, ++count) {
            try {
                animName = WEAPON_NAMES[i];
                animName.append("_");
                animName.append(ANIM_NAMES[j]);
                mAnimations[count] = mHandsEntity->getAnimationState(animName);
            } catch (...) {
                debugWARNING("Animation %s couldn't be found\n", animName.c_str());
                mAnimations[count] = 0;
            }
        }
    }

    // done, all the animations are ready and the offset also
}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::getHandBone(void)
{
    ASSERT(mHandsEntity);
    Ogre::SkeletonInstance* skeleton = mHandsEntity->getSkeleton();
    ASSERT(skeleton && "Hands without skeleton??");
    mHandBone = skeleton->getBone(MAINPLAYER_HANDS_BONE);
    ASSERT(mHandBone && "Hands did not contain the bone " MAINPLAYER_HANDS_BONE);
}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::configureCrossFire(void)
{
    Ogre::OverlayManager* om = Ogre::OverlayManager::getSingletonPtr();

    mCrossOverlay = om->getByName(MAINPLAYER_CROSSFIRE_OVERLAY);
    ASSERT(mCrossOverlay && "No overlay with name " MAINPLAYER_CROSSFIRE_OVERLAY
        " was found");

    // save the information in the cross fire info
    Ogre::Overlay::Overlay2DElementsIterator it = mCrossOverlay->get2DElementsIterator();
    ASSERT(it.hasMoreElements() && "No container?, something is ill formed "
        "we need at least one container representing the cross fire");
    Ogre::OverlayContainer* cont = it.getNext();
    const float left = cont->getLeft();
    const float top = cont->getTop();
    const float width = cont->getWidth();
    const float height = cont->getHeight();

    mCrossFireInfo.screenX = left + width * 0.5f;
    mCrossFireInfo.screenY = top + height * 0.5f;

}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::configureCameraController(const CamLimit& camLimits)
{
    // use the current camera and bone
    ASSERT(mHandsEntity);
    ASSERT(mCrossFireInfo.camera);

    Ogre::SkeletonInstance* skeleton = mHandsEntity->getSkeleton();
    ASSERT(skeleton);
    Ogre::Bone* headBone = skeleton->getBone(MAINPLAYER_HEAD_BONE);
    ASSERT(headBone && "Invalid player skeleton? We need the head!");

    debugWARNING("Hardcoded values here!\n");
    Ogre::Vector3 offsetPos = Ogre::Vector3::ZERO;
    Ogre::Quaternion offsetRot;
    offsetRot = Ogre::Quaternion(Ogre::Degree(270), Ogre::Vector3::UNIT_Z);
    mCamController.setData(mBaseNode,
                           mCrossFireInfo.camera,
                           mHandsEntity,
                           MAINPLAYER_HEAD_BONE,
                           offsetPos,
                           offsetRot);

    mCamController.setVelFactor(MAINPLAYER_CAM_VEL_FACTOR);
    mCamController.setConstraints(camLimits.pitchMin,
                                  camLimits.pitchMax,
                                  camLimits.rollMin,
                                  camLimits.rollMax);
}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::configurePlayerWeapon(Weapon* weapon)
{
    ASSERT(mHandsEntity);
    ASSERT(weapon);
    ASSERT(mHandBone);

    if (mCurrentWeapon) {
        mHandsEntity->detachObjectFromBone(mCurrentWeapon->entity());
        mCurrentWeapon = 0;
    }

    // set the new one
    if (weapon) {
        mHandsEntity->attachObjectToBone(mHandBone->getName(),
                                         weapon->entity(),
                                         weapon->rotEntityOffset(),
                                         weapon->posEntityOffset());
        mCurrentWeapon = weapon;
    }
}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::newEvent(Event event)
{

    // we will check the event by state
    switch(mState) {
    /////////////////////////////////////////////////////////////////////
    case State::S_IDLE:
    {
        switch (event) {
        case Event::E_CHANGE_W:
        {
            // we need to change the current weapon
            ASSERT(mCurrentWeapon);
            ASSERT(mWeaponToUse);

            // set the animations to be the putting_in
            const WeaponID wid = mCurrentWeapon->weaponID();
            startAnimation(mAnimOffsets[wid] + AnimIndex::A_PUT_IN);

            // update whatever needs to be updated
            if (mWeaponCallback) {
            	mWeaponCallback(mWeaponToUse,
            					PlayerWeaponEvent::PWE_SWAP_WEAPON);
            }

            // change the current state
            mState = State::S_CHANGING_W;
        }
        break;

        case Event::E_FIRE:
        {
            // we need to fire the current weapon
            ASSERT(mCurrentWeapon);

            // set the proper animation to fire
            const WeaponID wid = mCurrentWeapon->weaponID();
            startAnimation(mAnimOffsets[wid] + AnimIndex::A_FIRE);

            // update whatever needs to be updated
            if (mWeaponCallback) {
            	mWeaponCallback(mCurrentWeapon,
            					PlayerWeaponEvent::PWE_FIRE);
            }

            // set the current state
            mState = State::S_FIRING;
        }
        break;

        case Event::E_RELOAD:
        {
            // we need to reload the current weapon
            ASSERT(mCurrentWeapon);

            // set the proper animation to reload
            const WeaponID wid = mCurrentWeapon->weaponID();
            startAnimation(mAnimOffsets[wid] + AnimIndex::A_RELOAD);

            // update whatever needs to be updated
            if (mWeaponCallback) {
            	mWeaponCallback(mCurrentWeapon,
            					PlayerWeaponEvent::PWE_RELOAD);
            }

            // set the current state
            mState = State::S_RELOADING;
        }
        break;

        default:
            debugERROR("Event %d on State %d not valid\n", event, mState);
            ASSERT(false)
        }
    }
    break;

    /////////////////////////////////////////////////////////////////////
    case State::S_FIRING:
    {
        // the only event we support is done
        ASSERT(event == Event::E_DONE);

        // start the idle state
        const WeaponID wid = mCurrentWeapon->weaponID();
        startAnimation(mAnimOffsets[wid] + AnimIndex::A_IDLE_1);
        mState = State::S_IDLE;
    }
    break;

    /////////////////////////////////////////////////////////////////////
    case State::S_RELOADING:
    {
        // the only event we support is done
        ASSERT(event == Event::E_DONE);

        // start the idle state
        const WeaponID wid = mCurrentWeapon->weaponID();
        startAnimation(mAnimOffsets[wid] + AnimIndex::A_IDLE_1);
        mState = State::S_IDLE;
    }
    break;

    /////////////////////////////////////////////////////////////////////
    case State::S_CHANGING_W:
    {
        // the only event we support is done
        ASSERT(event == Event::E_DONE);

        // start the idle state
        const WeaponID wid = mCurrentWeapon->weaponID();
        startAnimation(mAnimOffsets[wid] + AnimIndex::A_IDLE_1);
        mState = State::S_IDLE;
    }
    break;

    default:
        debugERROR("Invalid current state %d\n", mState);
        ASSERT(false);
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MainPlayer::MainPlayer() :
    mBaseNode(0)
,   mHandsEntity(0)
,   mHandBone(0)
,   mCrossOverlay(0)
,   mCurrentWeapon(0)
,   mWeaponToUse(0)
,   mCurrentAnim(0)
,   mLife(1000)
{
    for (unsigned int i = 0; i < WeaponID::WID_COUNT; ++i) {
        mWeapons[i] = 0;
    }
    for (unsigned int i = 0; i < MAX_NUM_ANIMS; ++i) {
        mAnimations[i] = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
MainPlayer::~MainPlayer()
{
    if (mBaseNode) {
        GlobalData::sceneMngr->destroySceneNode(mBaseNode);
    }
    if (mHandsEntity) {
        GlobalData::sceneMngr->destroyEntity(mHandsEntity);
    }

    if (mCrossOverlay) {
        Ogre::OverlayManager::getSingleton().destroy(mCrossOverlay);
    }

    /*

    Weapon* mWeapons[WeaponID::WID_COUNT];
    Weapon* mCurrentWeapon;

    Ogre::AnimationState* mCurrentAnim;
    Ogre::AnimationState* mAnimations[MAX_NUM_ANIMS];
    WeaponAnimIndices mAnimIndices[WeaponID::WID_COUNT];

    unsigned int mLife;*/
}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::build(Ogre::Camera* camera,
                  const CamLimit& camLimits)
{
    ASSERT(camera);
    ASSERT(mBaseNode == 0 && "The player was already built!");

    // build the SceneNode and the entity of the player
    mBaseNode = GlobalData::sceneMngr->createSceneNode();
    mHandsEntity = GlobalData::sceneMngr->createEntity(MAINPLAYER_HANDS_ENTITY);
    mBaseNode->attachObject(mHandsEntity);

    // get the hand bone
    getHandBone();

    // configure the cross fire overlay where it should
    configureCrossFire();
    mCrossFireInfo.camera = camera;
    Weapon::setCrossFireInfo(&mCrossFireInfo);

    // configure and load the animations for all the weapons
    loadAnimations();

    // Configure the camera controller
    configureCameraController(camLimits);

}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::lookInDirection(const Ogre::Vector3& direction)
{
    ASSERT(false && "TODO: We need to implement this!");

    // TODO: here we have to set the camera orientation to an specific location.
    //       probably we will need to implement that method also in the camera
    //       controller class.
    //
}


////////////////////////////////////////////////////////////////////////////
// Weapon handling

////////////////////////////////////////////////////////////////////////////////
bool
MainPlayer::addWeapon(Weapon* weapon)
{
    ASSERT(weapon);

    // check if the weapon already exists.
    if (mWeapons[weapon->weaponID()] != 0) {
        return false;
    }

    // add this weapon
    mWeapons[weapon->weaponID()] = weapon;

    debugWARNING("Probably we want to set this weapon as current?\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::changeWeapon(WeaponID wid)
{
    if (mWeapons[wid] == 0) {
        debugWARNING("We are trying to change to a weapon that we have not already\n");
        return;
    }

    // set the current weapon to the player using the states
    if (mState != State::S_IDLE) {
        // we cannot change the weapon now
        return;
    }
    mWeaponToUse = mWeapons[wid];
    newEvent(Event::E_CHANGE_W);
}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::addAmmunition(unsigned short magazines, WeaponID wid)
{
    // ensure that the weapon exists
    ASSERT(mWeapons[wid] != 0);
    mWeapons[wid]->addAmmunition(magazines, 0);
}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::doFire(void)
{
    if (!mCurrentWeapon) {
        debugWARNING("No weapon set to fire!\n");
        return;
    }

    // check if we are in a possible state to do it
    if (mState != State::S_IDLE) {
        // we cannot fire
        return;
    }

    // TODO: here we have to define the behavior we want to do for the
    //       weapon, for example, if no more bullets then we can just auto
    //       reload the weapon?
    //
    if (!mCurrentWeapon->canFire()) {
        // we cannot fire
        return;
    }
    // else, we can fire
    if (mCurrentWeapon->fire()) {
        // we could fire
        newEvent(Event::E_FIRE);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::doReload(void)
{
    if (!mCurrentWeapon) {
        debugWARNING("No weapon set to reload!\n");
        return;
    }
    // check if we are in a possible state to do it
    if (mState != State::S_IDLE) {
        // we cannot fire
        return;
    }

    // check if we can reload or not the weapon
    if (!mCurrentWeapon->canReload()) {
        // we cannot reload
        return;
    }
    if (mCurrentWeapon->reload()) {
        newEvent(Event::E_RELOAD);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::activateFeature(WFeature feature)
{
    if (!mCurrentWeapon) {
        debugWARNING("Trying to activate a feature but no weapon set\n");
        return;
    }
    if (mState != State::S_IDLE) {
        // we are not able to do it
        return;
    }
    mCurrentWeapon->setFeature(feature);
}

////////////////////////////////////////////////////////////////////////////
// General methods
//

////////////////////////////////////////////////////////////////////////////////
void
MainPlayer::update(void)
{
    // update the current animation if we have
    ASSERT(mCurrentAnim);
    mCurrentAnim->addTime(GlobalData::lastTimeFrame);

    // update the weapon if we have
    ASSERT(mCurrentWeapon);
    mCurrentWeapon->update(GlobalData::lastTimeFrame);

    // update the logic depending on the current state
    switch(mState) {
    /////////////////////////////////////////////////////////////////////
    case State::S_IDLE:
    {
        // if the current animation didn't ended then we return directly
        if (!mCurrentAnim->hasEnded()) {
            return;
        }

        // else, we can choose one of the idle animations randomly
        unsigned int idleOffset = std::rand() % 3; // TODO: hardcoded
        startAnimation(mAnimOffsets[mCurrentWeapon->weaponID()] +
            AnimIndex::A_IDLE_1 + idleOffset);

        // nothing else to do
    }
    break;

    /////////////////////////////////////////////////////////////////////
    case State::S_FIRING:
    {
        // check if we already finish firing
        if (!mCurrentAnim->hasEnded()) {
            return;
        }

        // yes we finish, nothing else to do right now, emit the event
        newEvent(Event::E_DONE);
    }
    break;

    /////////////////////////////////////////////////////////////////////
    case State::S_RELOADING:
    {
        // check if we already finish reloading
        if (!mCurrentAnim->hasEnded()) {
            return;
        }

        // yes we finish, nothing else to do right now, emit the event
        newEvent(Event::E_DONE);
    }
    break;

    /////////////////////////////////////////////////////////////////////
    case State::S_CHANGING_W:
    {
        // if the current animation didn't ended then we return directly
        if (!mCurrentAnim->hasEnded()) {
            return;
        }

        // else, the animation ended, check if we have to start the putting out
        // or it is everything already
        if (mCurrentAnim ==
            mAnimations[mAnimOffsets[mCurrentWeapon->weaponID()] + AnimIndex::A_PUT_IN]) {
            // the current animation that just finish is when we are putting the
            // current weapon in. So we must change the weapon now
            ASSERT(mWeaponToUse);
            configurePlayerWeapon(mWeaponToUse);

            // configure the animation to be used to put the weapon out
            startAnimation(mAnimOffsets[mCurrentWeapon->weaponID()] + AnimIndex::A_PUT_OUT);
        } else {
            // we just finish the extraction of the new weapon, emmit the event
            // DONE
            newEvent(Event::E_DONE);
        }
    }
    break;

    default:
        debugERROR("Unkown state %d\n", mState);
        ASSERT(false);
    }

}

} /* namespace cz */
