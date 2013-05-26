/*
 * ZombieEngagingState.h
 *
 *  Created on: 28/02/2012
 *      Author: agustin
 */

#ifndef ZOMBIEENGAGINGSTATE_H_
#define ZOMBIEENGAGINGSTATE_H_

#include "FSM/IState.h"
#include "ZombieUnit.h"


class ZombieEngagingState : public ia::IState<ZombieUnit *>
{
public:
	ZombieEngagingState();
	virtual ~ZombieEngagingState();

	// Enter the state
	void enter(ZombieUnit *);

	// exit state
	void exit(ZombieUnit *);

	// update state
	void update(ZombieUnit *);

private:
	/**
	 * Function used to verify if we have to update the actual path or
	 * the path is ok. In any case we set the correct path to the unit.
	 * This function take in account the group of zombies
	 */
	void updatePath(ZombieUnit *ref);
};

#endif /* ZOMBIEENGAGINGSTATE_H_ */
