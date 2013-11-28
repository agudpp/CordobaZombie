/*
 * MainMenuTT.h
 *
 *  Created on: Nov 25, 2013
 *      Author: agustin
 */

#ifndef MAINMENUTT_H_
#define MAINMENUTT_H_

#include <map>

#include <types/StackVector.h>

#include <game_states/states/MenuMainState/substates/MainMenuSubState.h>


namespace cz {

class MainMenuTT
{
public:
    MainMenuTT();
    ~MainMenuTT();

    // @brief Build / destroy this Table (and build / destroy the substates).
    //        but the states will be not initialized. To do that you have to
    //        get all the states and initialize one by one.
    //
    void
    build(void);
    void
    destroy(void);

    // @brief Return the main state of the FSM.
    //
    MainMenuSubState*
    mainState(void) const;

    // @brief Get the next state given a particular state and event
    // @param currentState      The current state of the state machine
    // @param event             The given event
    // @returns nextState on success | 0 otherwise
    //
    MainMenuSubState*
    getNext(const MainMenuSubState* current, const MainMenuSubStateEvent& event) const;

    // @brief Return the list of all the sub states
    // @param size      The number of sub states
    // @return the pointer to the first sub states | 0 if no substates are
    //
    MainMenuSubState**
    subStates(unsigned int& size);
    const MainMenuSubState*const *
    subStates(unsigned int& size) const;

private:
    typedef std::map<MainMenuSubStateEvent, MainMenuSubState*> EventToStateMap;

    core::StackVector<EventToStateMap,
                      MainMenuSubStateEvent::MMSSE_COUNT> mTable;
    core::StackVector<MainMenuSubState*, 4> mStates;
};

} /* namespace cz */
#endif /* MAINMENUTT_H_ */
