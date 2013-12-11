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
    // We will define an internal state for the states that will be loaded.
    // This way we will avoid the big long timeframes between loading
    // the resources and executing the state.
    //
    enum State {
        MSM_ST_INIT = 0,
        MSM_ST_LOAD,
        MSM_ST_RUNNING,
    };

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
    // @return true on success | false on error or if we finish and need to
    //         go out
    //
    bool
    update(float timeFrame);


private:

    // @brief Change an state, uninitialize the current if needed an init the
    //        new one (but not load it). This will not change the internal state.
    // @param newState      The new state we will execute
    // @return true on success | false if the old state fails when unloading or
    //                           the new one fails when trying to load.
    bool
    changeState(IMainState* newState);

    // @brief Uninit the state (its resources and make all the needed calls)
    // @param state      The state we want to unload
    // @return true on success | false if some error occur
    //
    bool
    uninitState(IMainState* state);

    // @brief Init a new state (its resources and make all the needed calls)
    // @param state     The state we want to load
    // @return true on success | false if some error occur
    //
    bool
    initState(IMainState* state);

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
    State mInternalState;
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
