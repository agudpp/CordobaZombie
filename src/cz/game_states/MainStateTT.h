/*
 * MainStateTT.h
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#ifndef MAINSTATETT_H_
#define MAINSTATETT_H_

#include <types/StackVector.h>
#include <map>

#include "IMainState.h"
#include "MainStateEvent.h"
#include "MainStateBuilder.h"
#include "MainStateID.h"

namespace cz {

class MainStateTT
{
public:
    MainStateTT();
    ~MainStateTT();

    // @brief Build the Table using a builder to get all the states
    // @param builder       The builder to use
    // @return true on success | false otherwise
    //
    bool
    build(const MainStateBuilder& builder);

    // @brief Return the main state of the FSM.
    //
    inline IMainState*
    mainState(void) const;

    // @brief Get the next state given a particular state and event
    // @param currentState      The current state of the state machine
    // @param event             The given event
    // @returns nextState on success | 0 otherwise
    //
    IMainState*
    getNext(const IMainState* current, const MainStateEvent& event) const;

    // @brief Free all the allocated states (free its memory too).
    //
    void
    freeAllStates(void);

private:

    // @brief Helper method to add a new transtition
    // @param from      The current state
    // @param event     The event camming from (transition)
    // @param to        The next state
    //
    void
    addTransition(IMainState* from, const MainStateEvent& event, IMainState* to);

    // @brief Transform MainStateID to unsigned int
    //
    inline unsigned int
    toUInt(const MainStateID& id) const;

private:
    static const unsigned int NUM_STATES = static_cast<unsigned int>(MainStateID::Count);
    typedef std::map<MainStateEvent, IMainState*> EventToStateMap;

    core::StackVector<EventToStateMap, NUM_STATES> mTransitionTable;
    core::StackVector<IMainState*, NUM_STATES> mStates;
};









////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline unsigned int
MainStateTT::toUInt(const MainStateID& id) const
{
    return static_cast<unsigned int>(id);
}

inline IMainState*
MainStateTT::mainState(void) const
{
    return mStates[toUInt(MainStateID::InfoState)];
}

} /* namespace cz */
#endif /* MAINSTATETT_H_ */
