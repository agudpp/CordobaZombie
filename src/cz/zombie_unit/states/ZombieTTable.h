/*
 * ZombieTTable.h
 *
 *  Created on: Sep 10, 2013
 *      Author: agustin
 */

#ifndef ZOMBIETTABLE_H_
#define ZOMBIETTABLE_H_

#include "ZombieEvent.h"
#include "ZombieState.h"

// states
#include "ZSRunning.h"
#include "ZSBeingHit.h"
#include "ZSDying.h"

namespace cz {

// define the Table here.
//
class ZombieTTable
{
public:
    ZombieTTable(){};
    ~ZombieTTable(){};

    // @brief Return the main state of the FSM.
    //
    ZombieState*
    mainState(void)
    {
        return &mRunning;
    }

    // @brief Get the next state given a particular state and event
    // @param currentState      The current state of the state machine
    // @param event             The given event
    // @returns nextState on success | 0 otherwise
    //
    ZombieState*
    getNext(const ZombieState* current, const ZombieEvent& event)
    {
        if (event == ZombieEvent::ZE_BEING_HIT) {
            ASSERT(current != &mDying);
            return &mBeingHit;
        } else if (event == ZombieEvent::ZE_DIE) {
            ASSERT(current != &mDying);
            return &mDying;
        } else if (event == ZombieEvent::ZE_DONE) {
            if (current == &mBeingHit) {
                return &mRunning;
            } else {
                ASSERT(false && "From Dying nor Running we can return DONE!");
            }
        }
        ASSERT(false);
        return 0;
    }

private:
    // define the states here
    ZSRunning mRunning;
    ZSBeingHit mBeingHit;
    ZSDying mDying;
};

} /* namespace cz */
#endif /* ZOMBIETTABLE_H_ */
