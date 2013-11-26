/*
 * MainMenuHelpState.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: agustin
 */

#include "MainMenuHelpState.h"

namespace cz {

MainMenuHelpState::MainMenuHelpState()
{
    // TODO Auto-generated constructor stub

}

MainMenuHelpState::~MainMenuHelpState()
{
    // TODO Auto-generated destructor stub
}


////////////////////////////////////////////////////////////////////////////
//                          Inherited methods                             //
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHelpState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHelpState::load(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHelpState::show(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
MainMenuSubStateEvent
MainMenuHelpState::update(float timeFrame)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return MainMenuSubStateEvent::MMSSE_DONE;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHelpState::hide(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHelpState::unload(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHelpState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

} /* namespace cz */
