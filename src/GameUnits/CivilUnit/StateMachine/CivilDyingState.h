/*
 * CivilDyingState.h
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#ifndef CIVILDYINGSTATE_H_
#define CIVILDYINGSTATE_H_

#include "FSM/IState.h"
#include "CivilUnit.h"

class CivilDyingState : public ia::IState<CivilUnit *>
{
public:
	CivilDyingState();
	virtual ~CivilDyingState();

	// Enter the state
	void enter(CivilUnit *ref);

	// exit state
	void exit(CivilUnit *ref);

	// update state
	void update(CivilUnit *ref);
};

#endif /* CIVILDYINGSTATE_H_ */
