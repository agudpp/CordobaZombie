/*
 * PlayerAttackingState.cpp
 *
 *  Created on: 20/03/2012
 *      Author: agustin
 *
 */

#include "PlayerAttackingState.h"

PlayerAttackingState::PlayerAttackingState()
{

}

PlayerAttackingState::~PlayerAttackingState()
{
}


void PlayerAttackingState::enter(PlayerUnit *ref)
{
	ASSERT(ref->getActualWeapon());
	ASSERT(ref->getActualTarget());

	Weapon *w = ref->getActualWeapon();

	// check if we can shoot
	if(!w->canShoot()){
		// we cannot, check if we had ammo.
		if(w->getAmmunition() == 0){
			// have no more ammo. change the weapon?
			ref->changeBestNextWeapon();
			ref->getFSM().newEvent(PlayerUnit::E_DONE);
			return;
		} else {
			// need to reload
			ref->getFSM().newEvent(PlayerUnit::E_NEED_RELOAD);
			return;
		}
	}

	// we can shoot here..
	// check if we are really closer to the enemy
	if(ref->getPosition().squaredDistance(ref->getActualTarget()->getPosition()) >
		w->getSqrRange()){
		// we are far away
		ref->getFSM().newEvent(PlayerUnit::E_ENEMY_FAR_AWAY);
		return;
	}


	// depending of the weapon, we will get the animation
	switch(w->getType()){
	case Weapon::W_SHORT:
		ref->changeAnimation(PlayerUnit::ANIM_ATTACK_SHORT);
		break;

	case Weapon::W_LONG:
		// check the enemy distance
		if(ref->getActualTarget()->getPosition().squaredDistance(
				ref->getPosition()) <= ref->getSqrRadius()){
			// second animation
			ref->changeAnimation(PlayerUnit::ANIM_ATTACK_LONG2);
		} else {
			ref->changeAnimation(PlayerUnit::ANIM_ATTACK_LONG);
		}
		break;

	case Weapon::W_GRANADE:
		// check the enemy distance
		if(ref->getActualTarget()->getPosition().squaredDistance(
				ref->getPosition()) <= ref->getSqrRadius()){
			// second animation
			ref->changeAnimation(PlayerUnit::ANIM_ATTACK_SHORT2);
		} else {
			ref->changeAnimation(PlayerUnit::ANIM_ATTACK_GRANADE);
		}

		break;

	default:
		ASSERT(false);

	}
	ref->lookAt(ref->getActualTarget()->getPosition());
	w->shoot(ref->getActualTarget()->getPosition());

	// check if we have to incrase/decrase the velocity of the animation
	if(w->getAnimVelFactor() != 1.0f){
		ref->setAnimationAccelCoef(w->getAnimVelFactor());
	}
}

// exit state
void PlayerAttackingState::exit(PlayerUnit *ref)
{
	// restore anim accel
	ref->setAnimationAccelCoef(1.0f);
}

// update state
void PlayerAttackingState::update(PlayerUnit *ref)
{
	// get the weapon
	Weapon *w = ref->getActualWeapon();

	// TODO: tneemos que ver si queremos implementar el caso que sea mas lento
//	if(w->getAnimVelFactor() < 1.0f){
//				Ogre::AnimationState *anim = ref->getActualAnimation();
//				ASSERT(anim->getTimePosition() > 0.0f);
//				anim->setTimePosition(anim->getTimePosition() - GLOBAL_TIME_FRAME);
//				anim->addTime(GLOBAL_TIME_FRAME * w->getAnimVelFactor());

	// wait until we stop anim
	if(!ref->hasActualAnimEnd()){
		return;
	}

	// check if the zombie dies
	if(ref->getActualTarget()->getLife() <= 0){
		// the target dies
		ref->getFSM().newEvent(PlayerUnit::E_ENEMY_DIE);
		debug("Enemy killed!\n");
		return;
	}

	// now check again all
	// check if we can shoot
	if(!w->canShoot()){
		// we cannot, check if we had ammo.
		if(w->getAmmunition() == 0){
			// have no more ammo. change the weapon?
			ref->changeBestNextWeapon();
			ref->getFSM().newEvent(PlayerUnit::E_DONE);
			return;
		} else {
			// need to reload
			ref->getFSM().newEvent(PlayerUnit::E_NEED_RELOAD);
			return;
		}
	}

	// check if we are really closer to the enemy
	const float dist = ref->getPosition().squaredDistance(
			ref->getActualTarget()->getPosition());
	if(dist > w->getSqrRange()){
		// we are far away
		ref->getFSM().newEvent(PlayerUnit::E_ENEMY_FAR_AWAY);
		return;
	}

	// else we are closer need to check witch animation set
	if(dist < ref->getSqrRadius()){
		switch(w->getType()){
		case Weapon::W_SHORT:
			ref->changeAnimation(PlayerUnit::ANIM_ATTACK_SHORT);
			break;

		case Weapon::W_LONG:
			// check the enemy distance
			ref->changeAnimation(PlayerUnit::ANIM_ATTACK_LONG2);
			break;

		case Weapon::W_GRANADE:
			// check the enemy distance
			ref->changeAnimation(PlayerUnit::ANIM_ATTACK_SHORT2);
			break;
		}
	} else {
		ref->restartActualAnimation();
	}

	// everything ok, reinit animation
	ref->lookAt(ref->getActualTarget()->getPosition());
	w->shoot(ref->getActualTarget()->getPosition());

}
