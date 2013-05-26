/*
 * StatesFactory.h
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#ifndef STATESFACTORY_H_
#define STATESFACTORY_H_

#include <vector>

namespace mm_states {

class IState;

class StatesFactory {
public:
	/**
	 * Function that creates all the states to be used in the MainMenuState.
	 * It will return a vector with all the states to be used, the first state
	 * (result[0]) is the "start" state of the StateMachine.
	 * The memory of the states must be deleted by the caller of this function
	 *
	 * @param	result	Vector of supported states
	 * @return	true	on success, false otherwise
	 */
	static bool buildStates(std::vector<IState *> &result);

};

}

#endif /* STATESFACTORY_H_ */
