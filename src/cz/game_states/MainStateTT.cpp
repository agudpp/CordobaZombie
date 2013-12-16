/*
 * MainStateTT.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#include "MainStateTT.h"

#include <debug/DebugUtil.h>

namespace cz {

////////////////////////////////////////////////////////////////////////////////
void
MainStateTT::addTransition(IMainState* from,
                           const MainStateEvent& event,
                           IMainState* to)
{
    ASSERT(from);
    ASSERT(to);

    const unsigned int index = static_cast<unsigned int>(from->ID());
    ASSERT(index < NUM_STATES);

    EventToStateMap& map = mTransitionTable[index];
    EventToStateMap::iterator it = map.find(event);
    if (it != map.end() && it->second == to) {
        debugERROR("We are adding the same transition twice, this is not possible "
            "nor supported by this kind of FSM\n");
        ASSERT(false);
        return;
    }

    // add the transition
    map[event] = to;
}

////////////////////////////////////////////////////////////////////////////////
MainStateTT::MainStateTT()
{
    mStates.resize(mStates.max_size());
    for (unsigned int i = 0; i < mStates.size(); ++i) {
        mStates[i] = 0;
    }
    mTransitionTable.resize(mTransitionTable.max_size());
}

////////////////////////////////////////////////////////////////////////////////
MainStateTT::~MainStateTT()
{
    freeAllStates();
}

////////////////////////////////////////////////////////////////////////////////
bool
MainStateTT::build(const MainStateBuilder& builder)
{
    // build all the states, remove them first
    freeAllStates();
    for (unsigned int i = 0; i < NUM_STATES; ++i) {
        IMainState* state = builder.buildState(MainStateID(i));
        if (state == 0) {
            debugERROR("Error creating the state with id %d\n", i);
            return false;
        }
        mStates[i] = state;
    }

    // build the transitions now, following the diagram in issue #184
    //
    addTransition(mStates[toUInt(MainStateID::InfoState)],
                  MainStateEvent::EVENT_DONE,
                  mStates[toUInt(MainStateID::IntroState)]);

    addTransition(mStates[toUInt(MainStateID::IntroState)],
                  MainStateEvent::EVENT_DONE,
                  mStates[toUInt(MainStateID::MenuState)]);

    addTransition(mStates[toUInt(MainStateID::MenuState)],
                  MainStateEvent::EVENT_DONE,
                  mStates[toUInt(MainStateID::PrePlayingState)]);

    addTransition(mStates[toUInt(MainStateID::MenuState)],
                  MainStateEvent::EVENT_EXIT,
                  mStates[toUInt(MainStateID::ExitState)]);

    addTransition(mStates[toUInt(MainStateID::PrePlayingState)],
                  MainStateEvent::EVENT_DONE,
                  mStates[toUInt(MainStateID::PlayingState)]);

    addTransition(mStates[toUInt(MainStateID::PrePlayingState)],
                  MainStateEvent::EVENT_EXIT,
                  mStates[toUInt(MainStateID::MenuState)]);

    return true;
}


////////////////////////////////////////////////////////////////////////////////
IMainState*
MainStateTT::getNext(const IMainState* current, const MainStateEvent& event) const
{
    const unsigned int index = static_cast<unsigned int>(current->ID());
    ASSERT(index < NUM_STATES);

    // check if we have a transition
    if (mTransitionTable[index].empty()) {
        // no transition
        return 0;
    }

    // else we have to check if we could find it
    EventToStateMap::const_iterator it = mTransitionTable[index].find(event);
    if (it == mTransitionTable[index].end()) {
        // we haven't that neither
        return 0;
    }

    return it->second;
}

////////////////////////////////////////////////////////////////////////////////
void
MainStateTT::freeAllStates(void)
{
    for (unsigned int i = 0; i < mStates.size(); ++i) {
        if (mStates[i] != 0) {
            delete mStates[i];
        }
        mStates[i] = 0;
    }
}



} /* namespace cz */
