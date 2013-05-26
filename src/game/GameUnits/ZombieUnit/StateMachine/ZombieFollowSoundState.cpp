/*
 * ZombieFollowSoundState.h
 *
 *  Created on: 2/07/2012
 *      Author: raul
 *
 */

#include "ZombieFollowSoundState.h"
#include "Vec2.h"
#include "ZombieDefs.h"
#include <cmath>

ZombieFollowSoundState::ZombieFollowSoundState()
{
}

ZombieFollowSoundState::~ZombieFollowSoundState()
{
}


// Enter the state
void ZombieFollowSoundState::enter(ZombieUnit *ref)
{
	ASSERT(ref);

	ref->getPathTo(ref->getSoundTarget()->getPosition());

	ASSERT(ref->getUnitPath().hasMorePathPoints());
	// change the animation
	ref->changeAnimation(ZombieUnit::ANIM_RUN_BASE
			+ std::rand() % ANIM_RUN_COUNT);
	ref->getActualAnimation()->setLoop(true);
	ref->setVelocity(ref->getAttackVelocity());
}

// exit state
void ZombieFollowSoundState::exit(ZombieUnit *ref)
{
	// do nothing
}

// update state
void ZombieFollowSoundState::update(ZombieUnit *ref)
{
	ASSERT(ref);
	ASSERT(ref->getSoundTarget());
	int animID = ref->getActualAnimID();
	static std::vector<GameUnit *> targets;
	const Ogre::Vector3 & v3 = ref->getSoundTarget()->getPosition();
	math::Point p(v3.x, v3.z);

	// first of all we have to check if we can see an enemy
	if(ref->seeTarges(targets)){
		// TODO: aca decidimos cual es la mejor target para este zombie..
		// probablemente podemos poner una funcion en la clase zombie
		// que dada una lista de targets devuelva cual es la mejor
		// By the way we will set first target
		ref->setActualTartet(targets[0]);

		// we now inform the event and return
		ref->getFSM().newEvent(ZombieUnit::E_PERCEIVE_TARGET);
		return;

	}


	if(animID >= ZombieUnit::ANIM_RUN_BASE &&
		ZombieUnit::ANIM_RUN_BASE + ANIM_RUN_COUNT > animID){

		// Move through path unless are close enough
		if(ref->getPosition().squaredDistance(p) <
				SOUND_CLOSE_ENOUGH_SQRDIST){
			// The zombie looks around:
			ref->changeAnimation(ZombieUnit::ANIM_IDLE_BASE
						+ std::rand() % ANIM_IDLE_COUNT);
			ref->lookAt(ref->getSoundTarget()->getPosition());
		}else{
			ASSERT(ref->getUnitPath().hasMorePathPoints());
			ref->moveThroughPath();
		}
		return;
	}

	if(animID >= ZombieUnit::ANIM_IDLE_BASE &&
		ZombieUnit::ANIM_IDLE_BASE + ANIM_IDLE_COUNT > animID){
		// End when finish looking around
		if(ref->getActualAnimation()->hasEnded()){
			ref->getFSM().newEvent(ZombieUnit::E_DONE);
		}
		return;
	}

	// else we keep running to the sound target
}
