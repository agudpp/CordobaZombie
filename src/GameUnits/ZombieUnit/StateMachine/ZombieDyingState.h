/*
 * ZombieDyingState.h
 *
 *  Created on: 28/02/2012
 *      Author: agustin
 */

#ifndef ZOMBIEDYINGSTATE_H_
#define ZOMBIEDYINGSTATE_H_


#include "IState.h"
#include "ZombieUnit.h"


class ZombieDyingState : public IState<ZombieUnit *>
{
public:
	ZombieDyingState();
	virtual ~ZombieDyingState();

	// Enter the state
	void enter(ZombieUnit *);

	// exit state
	void exit(ZombieUnit *);

	// update state
	void update(ZombieUnit *);
};

#endif /* ZOMBIEDYINGSTATE_H_ */
