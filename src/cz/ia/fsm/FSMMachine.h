/*
 * FSMMachine.h
 *
 *  Created on: Sep 7, 2013
 *      Author: agustin
 */

#ifndef FSMMACHINE_H_
#define FSMMACHINE_H_

#include <debug/DebugUtil.h>

namespace cz {

// @brief This is the only useful class that will save us writing some code.
//        We need to templetizate some things
//

template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
class FSMMachine
{
public:
    // We will construct the machine with a table, since without table we cannot
    // do almost anything
    //
    inline FSMMachine(TableType& table);
    inline ~FSMMachine() {};

    // @brief Set the reference to be used for this particular table
    // param ref    The reference to be passed to each state
    //
    inline void
    setRef(StateArgType* ref);

    // @brief Start the state machine
    //
    inline void
    start(void);

    // @brief Reset the state machine
    //
    inline void
    reset(void);

    // @brief Execute the last executed state
    //
    inline void
    executeLastState(void);

    // @brief Return the current / last state
    //
    inline StateType*
    currentState(void);
    inline const StateType*
    currentState(void) const;
    inline StateType*
    lastState(void);
    inline const StateType*
    lastState(void) const;

    // @brief Return the last event
    //
    inline const EventType&
    lastEvent(void) const;

    // @brief Produce / execute some particular event (this will change the
    //        current state of the FSM).
    // @param event     The event to execute
    //
    inline void
    newEvent(const EventType& event);

    // @brief Execute (update) the current state of the state machine
    //
    inline void
    update(void);

private:

    // @brief Change an state
    // @param newState
    //
    inline void
    changeState(StateType* newState);

private:
    TableType& mTable;
    StateType* mActualState;
    StateType* mLastState;
    EventType mLastEvent;
    StateArgType* mRef;
};






////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//


template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline void
FSMMachine<StateType,StateArgType,EventType,TableType>::changeState(StateType* newState)
{
    ASSERT(newState);
    mActualState->exit(mRef);
    mLastState = mActualState;
    mActualState = newState;
    mActualState->enter(mRef);
}

// We will construct the machine with a table, since without table we cannot
// do almost anything
//
template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline
FSMMachine<StateType,StateArgType,EventType,TableType>::FSMMachine(TableType& table) :
    mTable(table)
,   mActualState(0)
,   mLastState(0)
,   mRef(0)
{}

template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline
FSMMachine<StateType,StateArgType,EventType,TableType>::~FSMMachine()
{}

template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline void
FSMMachine<StateType,StateArgType,EventType,TableType>::setRef(StateArgType* ref)
{
    ASSERT(ref);
    mRef = ref;
}

template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline void
FSMMachine<StateType,StateArgType,EventType,TableType>::start(void)
{
    ASSERT(mTable.mainState());
    ASSERT(mActualState == 0);

    mActualState = mTable.mainState();
    mActualState->enter(mRef);
}

template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline void
FSMMachine<StateType,StateArgType,EventType,TableType>::reset(void)
{
    ASSERT(mTable.mainState());

    mLastEvent = 0;
    changeState(mTable.mainState());
}

template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline void
FSMMachine<StateType,StateArgType,EventType,TableType>::executeLastState(void)
{
    changeState(mLastState);
}

// @brief Return the current / last state
//
template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline StateType*
FSMMachine<StateType,StateArgType,EventType,TableType>::currentState(void)
{
    return mActualState;
}

template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline const StateType*
FSMMachine<StateType,StateArgType,EventType,TableType>::currentState(void) const
{
    return mActualState;
}

template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline StateType*
FSMMachine<StateType,StateArgType,EventType,TableType>::lastState(void)
{
    return mLastState;
}
template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline const StateType*
FSMMachine<StateType,StateArgType,EventType,TableType>::lastState(void) const
{
    return mLastState;
}

template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline const EventType&
FSMMachine<StateType,StateArgType,EventType,TableType>::lastEvent(void) const
{
    return mLastEvent;
}

template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline void
FSMMachine<StateType,StateArgType,EventType,TableType>::newEvent(const EventType& event)
{
    ASSERT(mActualState);
    StateType* nextState = mTable.getNext(mActualState, event);

    if(nextState == 0){
        debugERROR("We cannot get next state from actual state: %d"
                " and event %d\n", mActualState->ID(), event);
        return;
    } else {
        // stop the actual event
        mLastEvent = event;
        changeState(nextState);
    }
}

template <typename StateType,
          typename StateArgType,
          typename EventType,
          typename TableType>
inline void
FSMMachine<StateType,StateArgType,EventType,TableType>::update(void)
{
    ASSERT(mActualState);
    mActualState->update(mRef);
}



} /* namespace cz */
#endif /* FSMMACHINE_H_ */
