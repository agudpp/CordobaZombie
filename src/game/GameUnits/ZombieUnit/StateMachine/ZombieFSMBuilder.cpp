/*
 * ZombieFSMBuilder.cpp
 *
 *  Created on: 28/02/2012
 *      Author: agustin
 *
 */
#include "ZombieAttackingState.h"
#include "ZombieBeenHitState.h"
#include "ZombieBorningState.h"
#include "ZombieDyingState.h"
#include "ZombieEngagingState.h"
#include "ZombieIdleState.h"
#include "ZombieFollowSoundState.h"
#include "ZombieFSMBuilder.h"
#include "ZombieSpreadingState.h"


ZombieSMTTable *ZombieFSMBuilder::build(void)
{
	ZombieSMTTable *result = new ZombieSMTTable;

	// create all the states
	ZombieSpreadingState *spread = new ZombieSpreadingState;
	ZombieIdleState *idle = new ZombieIdleState;
	ZombieAttackingState *attack = new ZombieAttackingState;
	ZombieBeenHitState *beenHit = new ZombieBeenHitState;
	ZombieBorningState *borning = new ZombieBorningState;
	ZombieDyingState *dying = new ZombieDyingState;
	ZombieEngagingState *engaging = new ZombieEngagingState;
	ZombieFollowSoundState *fsound = new ZombieFollowSoundState;

	result->setStartState(borning);
	result->addNewEntry(idle, ZombieUnit::E_SPREAD, spread);
	result->addNewEntry(borning, ZombieUnit::E_DONE, idle);
	result->addNewEntry(idle, ZombieUnit::E_PERCEIVE_TARGET, spread);
	result->addNewEntry(fsound, ZombieUnit::E_PERCEIVE_TARGET, spread);
	result->addNewEntry(engaging, ZombieUnit::E_TARGET_LOST, idle);
	result->addNewEntry(engaging, ZombieUnit::E_TARGET_CLOSE, attack);

	result->addNewEntry(attack, ZombieUnit::E_TARGET_LOST, engaging);
	result->addNewEntry(attack, ZombieUnit::E_TARGET_KILLED, engaging);
	result->addNewEntry(attack, ZombieUnit::E_TARGET_FAR, engaging);

//	result->addNewEntry(beenHit, ZombieUnit::E_DYING, dying);
//	result->addNewEntry(engaging, ZombieUnit::E_DYING, dying);
	result->addNewEntry(StarState, ZombieUnit::E_DYING, dying);
	result->addNewEntry(beenHit, ZombieUnit::E_DONE, engaging);
	result->addNewEntry(idle, ZombieUnit::E_FOLLOW_SOUND, fsound);
	result->addNewEntry(fsound, ZombieUnit::E_FOLLOW_SOUND, fsound);
	result->addNewEntry(fsound, ZombieUnit::E_DONE, idle);
	result->addNewEntry(spread, ZombieUnit::E_DONE, engaging);

	result->addNewEntry(idle, ZombieUnit::E_ENGAGE, engaging);

	// beenhit state cannot be used with the StarState because we must avoid
	// enter this state when the zombie is already dead (dying state).

	result->addNewEntry(attack, 	ZombieUnit::E_BEEN_HIT, beenHit);
	result->addNewEntry(engaging,	ZombieUnit::E_BEEN_HIT, beenHit);
	result->addNewEntry(idle, 		ZombieUnit::E_BEEN_HIT, beenHit);
	result->addNewEntry(beenHit, 	ZombieUnit::E_BEEN_HIT, beenHit);
	result->addNewEntry(fsound, 	ZombieUnit::E_BEEN_HIT, beenHit);

	return result;

}
