/*
 * MainStateMachine.h
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#ifndef MAINSTATEMACHINE_H_
#define MAINSTATEMACHINE_H_

#include <debug/DebugUtil.h>
#include <ResourceHandler.h>

#include "IMainState.h"
#include "MainStateEvent.h"
#include "MainStateID.h"
#include "MainStateInformation.h"
#include "MainStateTT.h"

namespace cz {

class MainStateMachine
{
public:
    // @brief Construct the machine with the associated TransitionTable and
    //        the instance of the resource handle to use
    //
    MainStateMachine(const MainStateTT& tt, rrh::ResourceHandler& rh);
    ~MainStateMachine();

    // @brief Start the state machine
    //
    void
    start(void);

    // @brief Reset the state machine
    //
    void
    reset(void);

    // @brief Execute the last executed state
    //
    void
    executeLastState(void);

    // @brief Return the current / last state
    //
    inline IMainState*
    currentState(void);
    inline const IMainState*
    currentState(void) const;
    inline IMainState*
    lastState(void);
    inline const IMainState*
    lastState(void) const;

    // @brief Return the last event
    //
    inline const MainStateEvent&
    lastEvent(void) const;

    // @brief Execute (update) the current state of the state machine
    // @param timeFrame     The time frame to be used by the states.
    //
    void
    update(float timeFrame);


private:

    // @brief Change an state, uninitialize the current if needed an init the
    //        new one.
    // @param newState      The new state we will execute
    // @return true on success | false if the old state fails when unloading or
    //                           the new one fails when trying to load.
    bool
    changeState(IMainState* newState);

    // @brief Unload a state (its resources and make all the needed calls)
    // @param state      The state we want to unload
    // @return true on success | false if some error occur
    //
    bool
    unloadState(IMainState* state);

    // @brief Load a new state (its resources and make all the needed calls)
    // @param state     The state we want to load
    // @return true on success | false if some error occur
    //
    bool
    loadState(IMainState* state);

private:
    const MainStateTT& mTransitionTable;
    rrh::ResourceHandler& mResourceHandler;
    MainStateInformation mInfo;
    MainStateEvent mLastEvent;
    IMainState* mCurrentState;
    IMainState* mLastState;
};






////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

// @brief Return the current / last state
//
inline IMainState*
MainStateMachine::currentState(void)
{
    return mCurrentState;
}

inline const IMainState*
MainStateMachine::currentState(void) const
{
    return mCurrentState;
}

inline IMainState*
MainStateMachine::lastState(void)
{
    return mLastState;
}

inline const IMainState*
MainStateMachine::lastState(void) const
{
    return mLastState;
}

inline const MainStateEvent&
MainStateMachine::lastEvent(void) const
{
    return mLastEvent;
}


} /* namespace cz */
#endif /* MAINSTATEMACHINE_H_ */
