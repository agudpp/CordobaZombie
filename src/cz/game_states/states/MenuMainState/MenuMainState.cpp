/*
 * MenuMainState.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#include "MenuMainState.h"

#include <debug/DebugUtil.h>



namespace cz {

////////////////////////////////////////////////////////////////////////////////
void
MenuMainState::changeState(MainMenuSubState* newState)
{
    ASSERT(newState);

    // if we have a running state we have to uninit it first
    if (mCurrentState) {
        // hide the state
        mCurrentState->hide();
    }
    // init the new one
    mCurrentState = newState;
    mCurrentState->show();
}

////////////////////////////////////////////////////////////////////////////////
bool
MenuMainState::processSubStateEvent(MainMenuSubStateEvent event)
{
    switch (event) {
    case MainMenuSubStateEvent::MMSSE_PLAY_GAME:
        mEventInfo = MainStateEvent::EVENT_DONE;
        return true;
    case MainMenuSubStateEvent::MMSSE_EXIT_GAME:
        mEventInfo = MainStateEvent::EVENT_EXIT;
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MenuMainState::MenuMainState() :
    mCurrentState(0)
,   mID(MainStateID::MenuState)
{
    // since when we build an instance of a MainState we already have the
    // global information we can safely set it to the substates
    MainMenuSubState::setOgreData(sOgreInfo);
    MainMenuSubState::setSoundManager(sSoundManager);
    MainMenuSubState::setCommonHandlers(sCommonHandlers);
}

////////////////////////////////////////////////////////////////////////////////
MenuMainState::~MenuMainState()
{
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//                           Inherited methods
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
const MainStateID&
MenuMainState::ID(void) const
{
    return mID;
}

////////////////////////////////////////////////////////////////////////////////
bool
MenuMainState::configureState(const MainStateInformation& info)
{
    // Here we will build the transition table and configure each of the
    // sub states
    mTransitionTable.build();
    mCurrentState = 0;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
MenuMainState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    // get all the sub states and get its resources to load
    unsigned int size = 0;
    MainMenuSubState** subStates = mTransitionTable.subStates(size);
    if (subStates == 0 || size == 0) {
        debugERROR("There are no substates to work with! something is wrong\n");
        return false;
    }

    // iterate over all the substates and get its resources
    for (int i = 0; i < size; ++i) {
        ResourceGroupList rl;
        ASSERT(subStates[i]);
        MainMenuSubState& subs = *subStates[i];
        if (!subs.getResourcesToLoad(rl)) {
            debugERROR("Error getting resources for a substate\n");
            return false;
        }
        // append the results to the global list
        for (auto elem : rl) {
            resourceList.push_back(elem);
        }
    }

    // TODO: add the resources associated to the video here (if we have to load
    //       an overlay or whatever here is the place, or if we will create
    //       a empty overlay for the background.
    debugERROR("TODO:!!!\n");

    // everything goes fine
    return true;
}


////////////////////////////////////////////////////////////////////////////////
bool
MenuMainState::readyToGo(void)
{
    // here we need to tell each substate to be ready to show its information.
    unsigned int size = 0;
    MainMenuSubState** subStates = mTransitionTable.subStates(size);

    if (subStates == 0 || size == 0) {
        debugERROR("There are no substates to work with! something is wrong\n");
        return false;
    }

    for (int i = 0; i < size; ++i) {
        ASSERT(subStates[i]);
        MainMenuSubState& subs = *subStates[i];
        if (!subs.load()) {
            debugERROR("Error loading a substate\n");
            return false;
        }
    }

    // TODO: here we also need to set the video to be ready to be reproduced
    //       with the video player.
    debugERROR("TODO:!!!\n");

    // get the main state and show its information since it will be the first
    // state
    changeState(mTransitionTable.mainState());

    return true;
}


////////////////////////////////////////////////////////////////////////////////
bool
MenuMainState::update(float timeFrame)
{
    ASSERT(mCurrentState);

    // now what we need to do is update the current state
    MainMenuSubStateEvent event = mCurrentState->update(timeFrame);
    if (event == MainMenuSubStateEvent::MMSSE_CONTINUE) {
        // nothing to do, we need to continue with this state
        return true;
    }

    // if this sub state change then we need to check if there is another
    // sub state to process or if it was the last one and we have to go
    // out from this MenuState (for example, exit or playGame was pressed).
    //
    MainMenuSubState* nextState = mTransitionTable.getNext(mCurrentState, event);
    if (nextState != 0) {
        // we can continue with the new state normally
        changeState(nextState);
        return true;
    }

    // else we need to check if the event is associated to an event that will
    // affect this state
    //
    if (processSubStateEvent(event)) {
        // we need to finish right now
        return false;
    }

    // else some error occur since we have not a next substate and we couldn't
    // process the event...
    debugERROR("We couldn't get an substate with an event %d\n", event);
    mEventInfo = MainStateEvent::EVENT_EXIT;
    return false;
}


////////////////////////////////////////////////////////////////////////////////
bool
MenuMainState::unload(void)
{
    // here we should call all the substates and unload all the information
    unsigned int size = 0;
    MainMenuSubState** subStates = mTransitionTable.subStates(size);

    if (subStates == 0 || size == 0) {
        debugERROR("There are no substates to work with! something is wrong\n");
        return false;
    }

    for (int i = 0; i < size; ++i) {
        ASSERT(subStates[i]);
        MainMenuSubState& subs = *subStates[i];

        if (!subs.unload()) {
            debugERROR("Error unloading a substate\n");
            return false;
        }
    }

    // TODO: here we should also remove all the video information
    debugERROR("TODO:!!\n");

    // everything fine
    return true;
}


////////////////////////////////////////////////////////////////////////////////
bool
MenuMainState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    ASSERT(false && "TODO");

    // here we should iterate over all the substates and get its resources
    // to unload
    unsigned int size = 0;
    MainMenuSubState** subStates = mTransitionTable.subStates(size);
    if (subStates == 0 || size == 0) {
        debugERROR("There are no substates to work with! something is wrong\n");
        return false;
    }

    // iterate over all the substates and get its resources
    for (int i = 0; i < size; ++i) {
        ResourceGroupList rl;
        ASSERT(subStates[i]);
        MainMenuSubState& subs = *subStates[i];

        if (!subs.getResourcesToUnload(rl)) {
            debugERROR("Error getting resources to unload for a substate\n");
            return false;
        }
        // append the results to the global list
        for (auto elem : rl) {
            resourceList.push_back(elem);
        }
    }

    // TODO: unload everything related with the background video here.
    //
    debugERROR("TODO:!!!\n");

    // everything goes fine
    return true;
}



} /* namespace cz */
