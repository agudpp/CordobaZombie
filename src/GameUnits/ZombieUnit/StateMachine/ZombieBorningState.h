/*
 * ZombieBorningState.h
 *
 *  Created on: 28/02/2012
 *      Author: agustin
 */

#ifndef ZOMBIEBORNINGSTATE_H_
#define ZOMBIEBORNINGSTATE_H_


#include "IState.h"
#include "ZombieUnit.h"


class ZombieBorningState : public IState<ZombieUnit *>
{
public:
	ZombieBorningState();
	virtual ~ZombieBorningState();

	// Enter the state
	void enter(ZombieUnit *);

	// exit state
	void exit(ZombieUnit *);

	// update state
	void update(ZombieUnit *);
};

#endif /* ZOMBIEBORNINGSTATE_H_ */
