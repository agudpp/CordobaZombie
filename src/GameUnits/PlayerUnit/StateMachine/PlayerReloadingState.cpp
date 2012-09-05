/*
 * PlayerReloadingState.cpp
 *
 *  Created on: 29/04/2012
 *      Author: agustin
 */

#include "PlayerReloadingState.h"

PlayerReloadingState::PlayerReloadingState()
{

}

PlayerReloadingState::~PlayerReloadingState()
{

}


// Enter the state
void PlayerReloadingState::enter(PlayerUnit *ref)
{
	ASSERT(ref->getActualWeapon());

	Weapon *w = ref->getActualWeapon();

	// depending of the weapon, we will get the animation
	switch(w->getType()){
	case Weapon::W_SHORT:
		ref->changeAnimation(PlayerUnit::ANIM_RELOAD_SHORT);
		break;

	case Weapon::W_LONG:
		ref->changeAnimation(PlayerUnit::ANIM_RELOAD_LONG);
		break;

	default:
		ASSERT(false);
	}

	ASSERT(w->getAmmunition() > 0);
}

// exit state
void PlayerReloadingState::exit(PlayerUnit *ref)
{

}

// update state
void PlayerReloadingState::update(PlayerUnit *ref)
{
	// wait until we stop anim
	if(!ref->hasActualAnimEnd()){
		return;
	}
	ref->getActualWeapon()->reload();
	debug("Reloading: ammo: %d\n", ref->getActualWeapon()->getAmmunition());
	// return done
	ref->getFSM().newEvent(PlayerUnit::E_DONE);
}

