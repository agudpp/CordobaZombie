/*
 * PlayerEngagingState.h
 *
 *  Created on: 20/03/2012
 *      Author: agustin
 *
 */

#ifndef PLAYERENGAGINGSTATE_H_
#define PLAYERENGAGINGSTATE_H_

#include "IState.h"
#include "PlayerUnit.h"

class PlayerEngagingState : public IState<PlayerUnit *> {
public:
	PlayerEngagingState();
	virtual ~PlayerEngagingState();

	// Enter the state
	void enter(PlayerUnit *ref);

	// exit state
	void exit(PlayerUnit *ref);

	// update state
	void update(PlayerUnit *ref);
};

#endif /* PLAYERENGAGINGSTATE_H_ */
