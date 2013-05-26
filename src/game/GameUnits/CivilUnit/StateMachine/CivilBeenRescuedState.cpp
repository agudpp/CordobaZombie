/*
 * CivilBeenRescuedState.cpp
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#include "CivilUnit.h"
#include "DebugUtil.h"
#include "CivilBeenRescuedState.h"

CivilBeenRescuedState::CivilBeenRescuedState()
{
	// TODO Auto-generated constructor stub

}

CivilBeenRescuedState::~CivilBeenRescuedState()
{
	// TODO Auto-generated destructor stub
}


// Enter the state
void CivilBeenRescuedState::enter(CivilUnit *ref)
{
	debugERROR("TODO\n");
}

// exit state
void CivilBeenRescuedState::exit(CivilUnit *ref)
{
	debugWARNING("TODO: tenemos que implementar aca el hecho de que el civil "
			"fue rescatado, o lo que sea.\n");
}

// update state
void CivilBeenRescuedState::update(CivilUnit *ref)
{
	debugERROR("TODO\n");

}

