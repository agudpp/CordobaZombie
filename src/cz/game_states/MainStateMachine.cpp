/*
 * MainStateMachine.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#include "MainStateMachine.h"

namespace cz {


////////////////////////////////////////////////////////////////////////////////
bool
MainStateMachine::changeState(IMainState* newState)
{
    ASSERT(newState);

    // if we have a running state we have to uninit it first
    if (mCurrentState) {
        if (!uninitState(mCurrentState)) {
            return false;
        }
    }
    // init the new one
    mLastState = mCurrentState;
    mCurrentState = newState;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
MainStateMachine::uninitState(IMainState* state)
{
    ASSERT(state);
    if (!state->unload()) {
        debugERROR("Error unloading state %d\n", (int) state->ID());
        return false;
    }
    ResourceGroupList resourceList;
    if (!state->getResourcesToUnload(resourceList)){
        debugERROR("Som error occur when trying to get the resources to unload for"
            " the state %d\n", (int) state->ID());
        return false;
    }

    for (rrh::ResourceGroup& rg : resourceList) {
        mResourceHandler.unloadResourceGroup(rg);
    }

    // everything goes fine
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
MainStateMachine::initState(IMainState* state)
{
    ASSERT(state);

    // configure the state
    if (!state->configureState(mInfo)) {
        debugERROR("Error configuring the state %d\n", state->ID());
        return false;
    }

    // get the resources to load
    ResourceGroupList resourceList;
    if (!state->getResourcesToLoad(resourceList)) {
        debugERROR("Error getting the resources for state %d\n", state->ID());
        return false;
    }

    // load the resources
    for (rrh::ResourceGroup& rg : resourceList) {
        if (!mResourceHandler.loadResourceGroup(rg)) {
            debugERROR("We fail loading some of the resources\n");
            return false;
        }
    }

    // everything goes fine
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
MainStateMachine::loadState(IMainState* state)
{
    ASSERT(state);
    // advise the state we are just ready to go
    return state->readyToGo();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MainStateMachine::MainStateMachine(const MainStateTT& tt, rrh::ResourceHandler& rh) :
    mTransitionTable(tt)
,   mResourceHandler(rh)
,   mCurrentState(0)
,   mLastState(0)
,   mInternalState(State::MSM_ST_INIT)
{
}

////////////////////////////////////////////////////////////////////////////////
MainStateMachine::~MainStateMachine()
{

}

////////////////////////////////////////////////////////////////////////////////
void
MainStateMachine::start(void)
{
    ASSERT(mTransitionTable.mainState());

    // reset the state machine, just in case
    reset();
    mCurrentState = mTransitionTable.mainState();

    // load the main state
    initState(mCurrentState);
    mInternalState = State::MSM_ST_LOAD;
}

////////////////////////////////////////////////////////////////////////////////
void
MainStateMachine::reset(void)
{
    if (mCurrentState == 0) {
        return; // nothing to do
    }

    // unload the current state
    uninitState(mCurrentState);
    mCurrentState = mLastState = 0;
}


////////////////////////////////////////////////////////////////////////////////
bool
MainStateMachine::update(float timeFrame)
{
    // check if we have a current state
    if (mCurrentState == 0) {
        return false; // nothing to do
    }

    switch (mInternalState) {
    case State::MSM_ST_INIT:
        // we need to init the current state
        if (!initState(mCurrentState)) {
            debugERROR("Error initing the current state %d\n", mCurrentState->ID());
            return false;
        }
        // change the state to be in "Load"
        mInternalState = State::MSM_ST_LOAD;
        break;
    case State::MSM_ST_LOAD:
        // return the current state
        if (!loadState(mCurrentState)) {
            debugERROR("Error loading current state %d\n", mCurrentState->ID());
        }
        // pass to the running state
        mInternalState = State::MSM_ST_RUNNING;
        break;
    case State::MSM_ST_RUNNING:
        // update the current state and do the checks
        if (!mCurrentState->update(timeFrame)) {
            // we need to change the current state.. Check the event information
            const MainStateEvent& event = mCurrentState->getEventInformation();
            IMainState* nextState = mTransitionTable.getNext(mCurrentState, event);
            if (nextState == 0) {
                // no more states, probably we finish?
                debug("No more states found from %d and event %d\n",
                      mCurrentState->ID(), event);
                return false;
            }

            // load the new state
            if (!changeState(nextState)) {
                debugERROR("Error changing from state %d to state %d with event %d\n",
                    (int) mCurrentState->ID(), (int) nextState->ID(), event);
                return false;
            }
            mLastEvent = event;

            // now we have to init the new state
            mInternalState = State::MSM_ST_INIT;
        }
        break;
    }

    // else there is nothing to do, and everything goes fine :)
    return true;
}



} /* namespace cz */
