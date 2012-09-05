/*
 * PlayerIdleState.cpp
 *
 *  Created on: 03/03/2012
 *      Author: agustin
 */

#include "PlayerIdleState.h"

PlayerIdleState::PlayerIdleState()
{

}

PlayerIdleState::~PlayerIdleState()
{
}


// Enter the state
void PlayerIdleState::enter(PlayerUnit *ref)
{
	// by now we only set the idle animation, we have to take in account the
	// world context and the internal attacking behavior
	ref->changeAnimation(PlayerUnit::ANIM_IDLE);
	ref->getActualAnimation()->setLoop(true);

	// set the actual state
	ref->getIAState().event = IAEP_IDLE;
}

// exit state
void PlayerIdleState::exit(PlayerUnit *ref)
{
	// do nothing??
}

// update state
void PlayerIdleState::update(PlayerUnit *ref)
{
	// here we check the internal attacking behaviour
	switch(ref->getAttackingBehavior()){
	case PlayerUnit::BH_NORMAL:
		// do nothing?
		break;
	case PlayerUnit::BH_AGRESIVE:
	{
		// TODO: verificamos por enemigos cercanos y vemos dependiendo una serie
		// de factores si atacamos o no... por el momento no hacmeos nada
		PlayerUnit::ZombieUnitVec &vec = ref->getWorldContext().getVisibleZombies();
		if(!vec.empty()){
			// we see zombies...
			//TODO: por ahora solamente atacamos el primero, deberiamos
			// ver que estan atacando nuestros "amigos" para evitar atacar el
			// mismo

			// try to attack some of the units
			ref->setActualTartet(vec[0]);
//			int i = vec.size() - 1;
//			while ((!ref->setActualTartet(vec[i])) && i >= 0) --i;

			return;
		}
	}
		// else we have no enemy closer, do nothing?
		break;
	default:
		ASSERT(false);
	}
}
