/*
 * ZombieIdleState.h
 *
 *  Created on: 22/02/2012
 *      Author: agustin
 */

#ifndef ZOMBIEIDLESTATE_H_
#define ZOMBIEIDLESTATE_H_


#include <cstdlib>

#include "FSM/IState.h"
#include "ZombieDefs.h"
#include "ZombieUnit.h"

class ZombieIdleState : public ia::IState<ZombieUnit *>
{
	static const int RND_NUM_NODES_MIN 	=	4;
	static const int RND_NUM_NODES_MAX 	=	14;
public:
	ZombieIdleState();
	~ZombieIdleState();

	// Enter the state
	void enter(ZombieUnit *);

	// exit state
	void exit(ZombieUnit *);

	// update state
	void update(ZombieUnit *);

private:
	inline void getRandomChoice(ZombieUnit *ref);
	inline void playRandomGrunt(ZombieUnit *ref);
	void configDoingNothingState(ZombieUnit *ref);
	void configWalkingAroundState(ZombieUnit *ref);
};







////////////////////////////////////////////////////////////////////////////////
inline void ZombieIdleState::getRandomChoice(ZombieUnit *ref)
{
	// set one of the 2 possible states
	int p = std::rand() % 100;
	if(p < IDLE_CHANCE) {
		configDoingNothingState(ref);
	} else {
		configWalkingAroundState(ref);
	}
}


#endif /* ZOMBIEIDLESTATE_H_ */
