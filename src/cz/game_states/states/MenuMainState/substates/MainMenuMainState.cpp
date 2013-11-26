/*
 * MainMenuMainState.cpp
 *
 *  Created on: Nov 25, 2013
 *      Author: agustin
 */

#include "MainMenuMainState.h"

namespace cz {

MainMenuMainState::MainMenuMainState()
{
    // TODO Auto-generated constructor stub

}

MainMenuMainState::~MainMenuMainState()
{
    // TODO Auto-generated destructor stub
}


////////////////////////////////////////////////////////////////////////////
//                          Inherited methods                             //
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
bool
MainMenuMainState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuMainState::load(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuMainState::show(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
MainMenuSubStateEvent
MainMenuMainState::update(float timeFrame)
{
//    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return MainMenuSubStateEvent::MMSSE_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuMainState::hide(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuMainState::unload(void)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuMainState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    return true;
}

} /* namespace cz */
