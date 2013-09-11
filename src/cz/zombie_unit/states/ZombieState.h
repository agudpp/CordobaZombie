/*
 * ZombieState.h
 *
 *  Created on: Sep 10, 2013
 *      Author: agustin
 */

#ifndef ZOMBIESTATE_H_
#define ZOMBIESTATE_H_


#include <ia/fsm/FSMState.h>

namespace cz {

// Forward
//
class ZombieUnit;

typedef FSMState<ZombieUnit*> ZombieState;

}

#endif /* ZOMBIESTATE_H_ */
