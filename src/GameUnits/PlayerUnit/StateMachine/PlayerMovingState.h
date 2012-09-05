/*
 * PlayerMovingstate.h
 *
 *  Created on: 03/03/2012
 *      Author: agustin
 */

#ifndef PLAYERMOVINGSTATE_H_
#define PLAYERMOVINGSTATE_H_



#include "IState.h"
#include "PlayerUnit.h"


class PlayerMovingState : public IState<PlayerUnit *>
{
public:
	PlayerMovingState();
	virtual ~PlayerMovingState();

	// Enter the state
	void enter(PlayerUnit *);

	// exit state
	void exit(PlayerUnit *);

	// update state
	void update(PlayerUnit *);
};

#endif /* PLAYERMOVINGSTATE_H_ */
