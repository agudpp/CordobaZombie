/*
 * CivilFSMBuilder.cpp
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#include "CivilFSMBuilder.h"
#include "CivilBeenHitState.h"
#include "CivilBeenRescuedState.h"
#include "CivilDyingState.h"
#include "CivilEscapingState.h"
#include "CivilFollowingState.h"
#include "CivilIdleState.h"

#include "SMTransitionTable.h"

/**
 * Builds the state machine of the zombie unit
 */
CivilSMTTable *CivilFSMBuilder::build(void)
{
	CivilBeenHitState 		*beenHit 		= new CivilBeenHitState;
	CivilBeenRescuedState 	*beenRescued 	= new CivilBeenRescuedState;
	CivilDyingState			*dying			= new CivilDyingState;
	CivilEscapingState		*escaping		= new CivilEscapingState;
	CivilFollowingState		*following		= new CivilFollowingState;
	CivilIdleState			*idle			= new CivilIdleState;

	CivilSMTTable *table = new CivilSMTTable;

	table->setStartState(idle);

	// from: idle
	table->addNewEntry(idle,		CivilUnit::E_BEEN_HIT, 		beenHit);
	table->addNewEntry(idle,		CivilUnit::E_FOLLOW_UNIT,	following);
	table->addNewEntry(idle,		CivilUnit::E_DYING,			dying);

	// from: following
	table->addNewEntry(following,	CivilUnit::E_UNIT_LOST,		idle);
	table->addNewEntry(following,	CivilUnit::E_BEEN_HIT,		beenHit);

	// from: beenHit
	table->addNewEntry(beenHit,		CivilUnit::E_DONE,			following);
	table->addNewEntry(beenHit,		CivilUnit::E_ESCAPE,		escaping);
	table->addNewEntry(beenHit,		CivilUnit::E_DYING,			dying);

	// from: escaping
	table->addNewEntry(escaping,	CivilUnit::E_DONE,			following);
//	table->addNewEntry(escaping,	CivilUnit::E_BEEN_HIT,		beenHit);

	// start
	table->addNewEntry(StarState,	CivilUnit::E_BEEN_RESCUED,	beenRescued);

	return table;
}
