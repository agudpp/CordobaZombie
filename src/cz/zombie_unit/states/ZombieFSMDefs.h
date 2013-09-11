/*
 * ZombieFSMDefs.h
 *
 *  Created on: Sep 10, 2013
 *      Author: agustin
 */

#ifndef ZOMBIEFSMDEFS_H_
#define ZOMBIEFSMDEFS_H_

#include <ia/fsm/FSMMachine.h>

#include "ZombieEvent.h"
#include "ZombieState.h"
#include "ZombieTTable.h"


namespace cz {

// Forward
//
class ZombieUnit;


// State machine
//
typedef FSMMachine<ZombieState, ZombieUnit, ZombieEvent, ZombieTTable> ZombieFSM;


}
#endif /* ZOMBIEFSMDEFS_H_ */
