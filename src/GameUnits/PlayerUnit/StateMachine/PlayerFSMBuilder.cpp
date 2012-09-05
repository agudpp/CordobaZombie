/*
 * PlayerFSMBuilder.cpp
 *
 *  Created on: 03/03/2012
 *      Author: agustin
 */

#include "DebugUtil.h"
#include "PlayerFSMBuilder.h"

#include "PlayerIdleState.h"
#include "PlayerMovingState.h"
#include "PlayerEngagingState.h"
#include "PlayerAttackingState.h"
#include "PlayerReloadingState.h"
#include "PlayerBombState.h"
#include "PlayerPickObjectState.h"
#include "SMTransitionTable.h"



PlayerSMTTable *PlayerFSMBuilder::buildDefault(void)
{
	PlayerSMTTable *result = new PlayerSMTTable;

	// create all the states
	PlayerMovingState 		*moving 	= new PlayerMovingState;
	PlayerIdleState 		*idle 		= new PlayerIdleState;
	PlayerEngagingState 	*engage 	= new PlayerEngagingState;
	PlayerAttackingState 	*attacking 	= new PlayerAttackingState;
	PlayerReloadingState 	*reloading 	= new PlayerReloadingState;
	PlayerBombState			*bomb 		= new PlayerBombState;
	PlayerPickObjectState 	*pickObject = new PlayerPickObjectState;

	result->setStartState(idle);
	result->addNewEntry(idle, 		PlayerUnit::E_MOVE_TO, 			moving);
	result->addNewEntry(idle, 		PlayerUnit::E_ENGAGE_UNIT, 		engage);
	result->addNewEntry(engage, 	PlayerUnit::E_ENEMY_SEEN, 		attacking);
	result->addNewEntry(engage, 	PlayerUnit::E_ENEMY_LOST, 		idle);
	result->addNewEntry(engage, 	PlayerUnit::E_MOVE_TO, 			moving);
	result->addNewEntry(attacking, 	PlayerUnit::E_ENEMY_FAR_AWAY, 	engage);
	result->addNewEntry(attacking, 	PlayerUnit::E_ENEMY_LOST, 		idle);
	result->addNewEntry(attacking, 	PlayerUnit::E_ENEMY_DIE, 		idle);
	result->addNewEntry(attacking, 	PlayerUnit::E_NEED_RELOAD, 		reloading);
	result->addNewEntry(attacking, 	PlayerUnit::E_DONE, 			engage);
	result->addNewEntry(attacking, 	PlayerUnit::E_MOVE_TO, 			moving);
	result->addNewEntry(reloading, 	PlayerUnit::E_DONE, 			attacking);
	result->addNewEntry(moving, 	PlayerUnit::E_DONE, 			idle);
	result->addNewEntry(moving, 	PlayerUnit::E_ENGAGE_UNIT, 		engage);
	/*
	 * Here we use StarState because it doesn't care in which state we are,
	 * we always want to jump to the pick object state when the
	 * corresponding event comes.
	 */
	result->addNewEntry(StarState, 	PlayerUnit::E_PICK_OBJECT, 		pickObject);
	result->addNewEntry(pickObject, PlayerUnit::E_DONE, 			idle);

	// bomb
	result->addNewEntry(idle, 		PlayerUnit::E_PLANT_BOMB, 		bomb);
	result->addNewEntry(engage, 	PlayerUnit::E_PLANT_BOMB, 		bomb);
	result->addNewEntry(moving, 	PlayerUnit::E_PLANT_BOMB,		bomb);
	result->addNewEntry(bomb, 		PlayerUnit::E_DONE, 			idle);



	return result;
}


/**
 * Builds the state machine of the player unit
 * @param id	The id of the PlayerTable to be builded (0 the default)
 */
PlayerSMTTable *PlayerFSMBuilder::build(int id)
{
	switch(id){
	case 0:
		// default (debug?)
		return buildDefault();
		break;
	default:
		ASSERT(false);
		return 0;
	}
}
