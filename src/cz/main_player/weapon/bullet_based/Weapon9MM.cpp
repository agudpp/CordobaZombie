/*
 * Weapon9MM.cpp
 *
 *  Created on: Oct 9, 2013
 *      Author: agustin
 */

#include "Weapon9MM.h"

#include <global_data/GlobalData.h>
#include <main_player/weapon/WeaponDefs.h>

namespace cz {

const char* Weapon9MM::ANIM_NAMES[S_COUNT] = {
        "dispara",
        "recarga",
        0
    };


Weapon9MM::Weapon9MM() :
    mState(State::S_IDLE)
{
    // configure the weapon information here
    mInfo.name = "9mm";
    mInfo.maxMagazines = 10;
    mInfo.maxBulletsPerMagazine = 12;
    setMovementRanges(0.01f, 0.02f);

    debugWARNING("We are hardcoding the values here, we need to change this\n");

    // load the animations
    loadAnims(ANIM_NAMES);

    // set the ID
    setWeaponID(WeaponID::WID_9MM);
}

Weapon9MM::~Weapon9MM()
{
}

////////////////////////////////////////////////////////////////////////////////
void
Weapon9MM::addAmmunition(unsigned short magazines, unsigned short bullets)
{
    performAddAmmunition(magazines, bullets);
}

////////////////////////////////////////////////////////////////////////////////
bool
Weapon9MM::canFire(void) const
{
    return (mState == S_IDLE) && (mInfo.bulletsInMagazine > 0);
}

////////////////////////////////////////////////////////////////////////////////
bool
Weapon9MM::canReload(void) const
{
    return (mInfo.magazineCount > 0) && mState == S_IDLE;
}

////////////////////////////////////////////////////////////////////////////////
bool
Weapon9MM::reload(void)
{
    if (!canReload()) {
        return false;
    }
    // else, we can reload, we will start the associated animation
    //
    startAnimation(S_RELOADING);
    mState = S_RELOADING;

    // note that probably it will be better to load the bullet after the animation
    // ends, but for now it will be ok like it is
    performReload();

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
Weapon9MM::fire(void)
{
    if (!canFire()) {
        return false;
    }
    // fire the bullet, here we should use the a bullet from some particular
    // place, in this case, we need to get a Bullet* from a bulletQueue and
    // call fireBullet(bullet);
    debugERROR("TODO: Firing a bullet\n");

    // we need to reproduce the associated effect here in the position where
    // the weapon is
    debugERROR("TODO: Reproduce the fire effect here\n");

    // Now we remove one bullet from the total of bullets
    performFire();

    // also reproduce the animation
    startAnimation(S_FIRING);
    mState = S_FIRING;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
Weapon9MM::update(float timeFrame)
{
    // here we will update the animation if we have
    if (mState == S_IDLE) {
        return;
    }
    // if we are here then we are reproducing some animation
    ASSERT(mCurrentAnim);
    mCurrentAnim->addTime(timeFrame);

    // check if we finish
    if (currentAnimEnded()) {
        mState = S_IDLE;
    }

}

////////////////////////////////////////////////////////////////////////////////
void
Weapon9MM::setFeature(WFeature feature)
{
    debugWARNING("NOTHING TO DO, remove this line from here\n");
}

} /* namespace cz */
