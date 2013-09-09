/*
 * FSMTransitionTable.h
 *
 *  Created on: Sep 7, 2013
 *      Author: agustin
 */

#ifndef FSMTRANSITIONTABLE_H_
#define FSMTRANSITIONTABLE_H_

namespace cz {

// @brief This class will be not used, but will show how is the interface of
//        the transtition table. The methods that need to be implemented.
//        We will not use virtual stuff since it is a crap and we don't want
//        that.
//

template<typename EventType,
         typename StateType>
class FSMTransitionTable
{
public:
    FSMTransitionTable(){};
    ~FSMTransitionTable(){};

    // @brief Return the main state of the FSM.
    //
    StateType*
    mainState(void);

    // @brief Get the next state given a particular state and event
    // @param currentState      The current state of the state machine
    // @param event             The given event
    // @returns nextState on success | 0 otherwise
    //
    StateType*
    getNext(const StateType* current, const EventType& event);

};

} /* namespace cz */
#endif /* FSMTRANSITIONTABLE_H_ */
