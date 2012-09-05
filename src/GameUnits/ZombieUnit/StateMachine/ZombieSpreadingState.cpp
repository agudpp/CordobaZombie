/*
 * ZombieSpreadingState.cpp
 *
 *  Created on: 04/07/2012
 *      Author: raul
 *
 */

#include "ZombieSpreadingState.h"
#include <stdlib.h>

ZombieSpreadingState::ZombieSpreadingState()
{
}

ZombieSpreadingState::~ZombieSpreadingState()
{
}


// Enter the state
void ZombieSpreadingState::enter(ZombieUnit *ref)
{
	ASSERT(ref);

	if(ref->getFSM().getLastEvent() != ZombieUnit::E_SPREAD){
		//I'm spreading
		ref->findNearbyZombies();

		//TODO: poner animacion de grito aca
		ref->changeAnimation(ZombieUnit::ANIM_SPREAD_BASE
						+ std::rand() % ANIM_SPREAD_COUNT);
		ref->getActualAnimation()->setLoop(false);

		ZombieUnit::ZombieUnitVec & zvector = ref->getNearbyZombies();
		for(int i = zvector.size()-1; i >= 0; i--){
			zvector[i]->setSpreader(ref);
			zvector[i]->getFSM().newEvent(ZombieUnit::E_SPREAD);
		}

	}else{
		// I've received a spread

		//TODO: poner animacion de giro aca
		ref->changeAnimation(ZombieUnit::ANIM_IDLE_BASE
						+ std::rand() % ANIM_IDLE_COUNT);
		ref->getActualAnimation()->setLoop(false);
		ref->setActualTartet(ref->getSpreader()->getActualTarget());

		// random time at which start engaging the target
		float atime = ref->getActualAnimation()->getLength();
		ref->setTimer((static_cast<float>(std::rand()) / RAND_MAX) * atime);
	}

}

// exit state
void ZombieSpreadingState::exit(ZombieUnit *ref)
{
	// do nothing
}

// update state
void ZombieSpreadingState::update(ZombieUnit *ref)
{
	if(ref->getFSM().getLastEvent() != ZombieUnit::E_SPREAD){
		//I'm spreading

		if(ref->getActualAnimation()->hasEnded()){
			ref->getFSM().newEvent(ZombieUnit::E_DONE);
			return;
		}
	}else{

		//TODO hacerlos girar de a poco aca
		//Someone is spreading to me

		float atime = ref->getActualAnimation()->getTimePosition();

		if(atime*2 > ref->getTimer()){
			ref->lookAt(ref->getSpreader()->getPosition());
		}

		if(atime > ref->getTimer()){
			ref->getFSM().newEvent(ZombieUnit::E_DONE);
		}

		return;
	}
}
