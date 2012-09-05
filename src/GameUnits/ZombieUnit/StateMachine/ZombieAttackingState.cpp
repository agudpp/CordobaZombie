/*
 * ZombieAttackingState.cpp
 *
 *  Created on: 28/02/2012
 *      Author: agustin
 *
 */

#include "ZombieAttackingState.h"

ZombieAttackingState::ZombieAttackingState()
{

}

ZombieAttackingState::~ZombieAttackingState()
{
}


// Enter the state
void ZombieAttackingState::enter(ZombieUnit *ref)
{
//	debug("Entering Attack state\n");
	ASSERT(ref->getActualTarget());

	// Look to the target
	Ogre::Vector3 v3 = ref->getActualTarget()->getSceneNode()->getPosition();
	sm::Vector2 v2 = sm::Vector2(v3.x, v3.z);
	ref->lookAt(v2);

	// change the animation
	ref->changeAnimation(ZombieUnit::ANIM_ATTACK_BASE
				+ std::rand() % ANIM_ATTACK_COUNT);

	// perform the attack here (we assume that if we are entering this state
	// is because we really can attack
	performAttack(ref);

	// check if we kill the target
	if(ref->getActualTarget()->getLife() <= 0){
		ref->setActualTartet(0);	// remove the target
		ref->getFSM().newEvent(ZombieUnit::E_TARGET_KILLED);
		return;
	}

	// check if we are in the attack range
	if(!ref->canAttackActualTarget()){
		// we cannot attack
		ref->getFSM().newEvent(ZombieUnit::E_TARGET_FAR);
		return;
	}

}

// exit state
void ZombieAttackingState::exit(ZombieUnit *ref)
{
	// do nothing
}

// update state
void ZombieAttackingState::update(ZombieUnit *ref)
{
	// check if we have target
	if(!ref->getActualTarget()){
		// no target... emit new event
		ref->setActualTartet(0);
		ref->getFSM().newEvent(ZombieUnit::E_TARGET_LOST);
		return;
	}

	// we are close.. then hit the enemy if we can reproduce the animation
	if(ref->hasActualAnimEnd()){
		// attack again
		performAttack(ref);

		// check if we kill the target
		if(ref->getActualTarget()->getLife() <= 0){
			ref->setActualTartet(0);	// remove the target
			ref->getFSM().newEvent(ZombieUnit::E_TARGET_KILLED);
			return;
		}

		// initialize again the animation
		ref->restartActualAnimation();

		// check if we are in the attack range
		if(!ref->canAttackActualTarget()){
			// we cannot attack
			ref->getFSM().newEvent(ZombieUnit::E_TARGET_FAR);
			return;
		}
	}
}
