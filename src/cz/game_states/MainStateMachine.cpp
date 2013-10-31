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
        if (!unloadState(mCurrentState)) {
            return false;
        }
    }
    // init the new one
    mLastState = mCurrentState;
    mCurrentState = newState;
    return loadState(mCurrentState);
}

////////////////////////////////////////////////////////////////////////////////
bool
MainStateMachine::unloadState(IMainState* state)
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
MainStateMachine::loadState(IMainState* state)
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

    // everything goes fine, make the last call to load the state, advise the
    // state that we are ready to go!
    return state->readyToGo();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MainStateMachine::MainStateMachine(const MainStateTT& tt, rrh::ResourceHandler& rh) :
    mTransitionTable(tt)
,   mResourceHandler(rh)
,   mCurrentState(0)
,   mLastState(0)
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
    loadState(mCurrentState);
}

////////////////////////////////////////////////////////////////////////////////
void
MainStateMachine::reset(void)
{
    if (mCurrentState == 0) {
        return; // nothing to do
    }

    // unload the current state
    unloadState(mCurrentState);
    mCurrentState = mLastState = 0;
}

////////////////////////////////////////////////////////////////////////////////
void
MainStateMachine::executeLastState(void)
{
    if (mLastState) {
        changeState(mLastState);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
MainStateMachine::update(float timeFrame)
{
    // check if we have a current state
    if (mCurrentState == 0) {
        return; // nothing to do
    }

    // if we have an state, then we need to execute it
    if (!mCurrentState->update(timeFrame)) {
        // we need to change the current state.. Check the event information
        const MainStateEvent& event = mCurrentState->getEventInformation();
        IMainState* nextState = mTransitionTable.getNext(mCurrentState, event);
        ASSERT(nextState);

        // load the new state
        if (!changeState(nextState)) {
            debugERROR("Error changing from state %d to state %d with event %d\n",
                (int) mCurrentState->ID(), (int) nextState->ID(), event);
            ASSERT(false && "How we can handle the error here? probably we cannot "
                "do anything :(");
            return;
        }
        mLastEvent = event;
    }

    // else there is nothing to do, and everything goes fine :)
}



} /* namespace cz */
