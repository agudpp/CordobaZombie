/*
 * PlayerFSMBuilder.h
 *
 *  Created on: 03/03/2012
 *      Author: agustin
 */

#ifndef PLAYERFSMBUILDER_H_
#define PLAYERFSMBUILDER_H_


#include "PlayerDefs.h"
#include "PlayerUnit.h"
#include "SMTransitionTable.h"

class PlayerFSMBuilder
{
public:
	/**
	 * Builds the state machine of the player unit
	 * @param id	The id of the PlayerTable to be builded (0 the default)
	 */
	static PlayerSMTTable *build(int id = 0);

private:
	static PlayerSMTTable *buildDefault(void);
};

#endif /* PLAYERFSMBUILDER_H_ */
