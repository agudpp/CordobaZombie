/*
 * MainMenuHistoryState.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: agustin
 */

#include "MainMenuHistoryState.h"

namespace cz {

MainMenuHistoryState::MainMenuHistoryState()
{
    // TODO Auto-generated constructor stub

}

MainMenuHistoryState::~MainMenuHistoryState()
{
    // TODO Auto-generated destructor stub
}


////////////////////////////////////////////////////////////////////////////
//                          Inherited methods                             //
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHistoryState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHistoryState::load(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHistoryState::show(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
MainMenuSubStateEvent
MainMenuHistoryState::update(float timeFrame)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return MainMenuSubStateEvent::MMSSE_DONE;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHistoryState::hide(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHistoryState::unload(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHistoryState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

} /* namespace cz */
