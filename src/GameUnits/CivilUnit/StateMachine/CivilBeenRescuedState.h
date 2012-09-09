/*
 * CivilBeenRescuedState.h
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#ifndef CIVILBEENRESCUEDSTATE_H_
#define CIVILBEENRESCUEDSTATE_H_


#include "FSM/IState.h"
#include "CivilUnit.h"


class CivilBeenRescuedState : public ia::IState<CivilUnit *>
{
public:
	CivilBeenRescuedState();
	virtual ~CivilBeenRescuedState();

	// Enter the state
	void enter(CivilUnit *ref);

	// exit state
	void exit(CivilUnit *ref);

	// update state
	void update(CivilUnit *ref);
};

#endif /* CIVILBEENRESCUEDSTATE_H_ */
