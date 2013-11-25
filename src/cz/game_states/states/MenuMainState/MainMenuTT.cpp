/*
 * MainMenuTT.cpp
 *
 *  Created on: Nov 25, 2013
 *      Author: agustin
 */

#include "MainMenuTT.h"

#include "substates/MainMenuCreditsState.h"
#include "substates/MainMenuHelpState.h"
#include "substates/MainMenuHistoryState.h"
#include "substates/MainMenuMainState.h"

namespace cz {

////////////////////////////////////////////////////////////////////////////////
MainMenuTT::MainMenuTT()
{

}

////////////////////////////////////////////////////////////////////////////////
MainMenuTT::~MainMenuTT()
{
    destroy();
}

////////////////////////////////////////////////////////////////////////////////
void
MainMenuTT::build(void)
{
    destroy();

    // local enum for the states
    enum States {
        MAIN_MENU = 0,
        CREDITS,
        HELP,
        HISTORY,
        COUNT,
    };

    // add one by one, the first one will be the main state
    mStates.resize(COUNT);
    mStates[MAIN_MENU] = new MainMenuMainState;
    mStates[CREDITS] = new MainMenuCreditsState;
    mStates[HELP] = new MainMenuHelpState;
    mStates[HISTORY] = new MainMenuHistoryState;

    // create the table here.
    mTable.resize(mStates.size());
    // main menu
    (mTable[MAIN_MENU])[MainMenuSubStateEvent::MMSSE_CREDITS] = mStates[CREDITS];
    (mTable[MAIN_MENU])[MainMenuSubStateEvent::MMSSE_HISTORY] = mStates[HISTORY];
    (mTable[MAIN_MENU])[MainMenuSubStateEvent::MMSSE_HELP] = mStates[HELP];

    // credits
    (mTable[CREDITS])[MainMenuSubStateEvent::MMSSE_DONE] = mStates[MAIN_MENU];

    // History
    (mTable[HISTORY])[MainMenuSubStateEvent::MMSSE_DONE] = mStates[MAIN_MENU];

    // Help
    (mTable[HELP])[MainMenuSubStateEvent::MMSSE_DONE] = mStates[MAIN_MENU];

    // nothing else for now
}
void
MainMenuTT::destroy(void)
{
    for (MainMenuSubState* s : mStates) {
        delete s;
    }
    mStates.clear();
    mTable.clear();
}

////////////////////////////////////////////////////////////////////////////////
MainMenuSubState*
MainMenuTT::mainState(void) const
{
    // check if we have the states, return the first one
    if (mStates.empty()) {
        return 0;
    }
    return mStates[0];
}

////////////////////////////////////////////////////////////////////////////////
MainMenuSubState*
MainMenuTT::getNext(const MainMenuSubState* current, const MainMenuSubStateEvent& event) const
{
     for (unsigned int i = 0; i < mStates.size(); ++i) {
         MainMenuSubState* s = mStates[i];
         if (current == s) {
             EventToStateMap::iterator it = mTable[i].find(event);
             if (it == mTable[i].end()) {
                 // not found
                 return 0;
             }
             return it->second;
         }
     }
     return 0;
}

////////////////////////////////////////////////////////////////////////////////
MainMenuSubState*
MainMenuTT::subStates(unsigned int& size)
{
    size = mStates.size();
    if (size == 0) {
        return 0;
    }
    return mStates.begin();
}
const MainMenuSubState*
MainMenuTT::subStates(unsigned int& size) const
{
    size = mStates.size();
    if (size == 0) {
        return 0;
    }
    return mStates.begin();
}

} /* namespace cz */
