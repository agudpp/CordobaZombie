/*
 * MainMenuCreditsState.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: agustin
 */

#include "MainMenuCreditsState.h"

namespace cz {

MainMenuCreditsState::MainMenuCreditsState()
{
    // TODO Auto-generated constructor stub

}

MainMenuCreditsState::~MainMenuCreditsState()
{
    // TODO Auto-generated destructor stub
}


////////////////////////////////////////////////////////////////////////////
//                          Inherited methods                             //
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
bool
MainMenuCreditsState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuCreditsState::load(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuCreditsState::show(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
MainMenuSubStateEvent
MainMenuCreditsState::update(float timeFrame)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return MainMenuSubStateEvent::MMSSE_DONE;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuCreditsState::hide(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuCreditsState::unload(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuCreditsState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

} /* namespace cz */
