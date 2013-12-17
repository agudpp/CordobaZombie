/*
 * ExitMainState.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#include "ExitMainState.h"

namespace cz {

ExitMainState::ExitMainState()
{

}

ExitMainState::~ExitMainState()
{

}

////////////////////////////////////////////////////////////////////////////
//                           Inherited methods
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
const MainStateID&
ExitMainState::ID(void) const
{
    static MainStateID id = MainStateID::ExitState;
    return id;
}

////////////////////////////////////////////////////////////////////////////////
bool
ExitMainState::configureState(const MainStateInformation& info)
{
    debugERROR("TODO: fix this, we will do nothing for now\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
ExitMainState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    debugERROR("TODO: fix this, we will do nothing for now\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
ExitMainState::readyToGo(void)
{
    debugERROR("TODO: fix this, we will do nothing for now\n");
    mEventInfo = MainStateEvent::EVENT_EXIT;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
ExitMainState::update(float timeFrame)
{
    debugERROR("TODO: fix this, we will do nothing for now, we will return"
        " false for now just to skip this state\n");
    return false;
}

////////////////////////////////////////////////////////////////////////////////
bool
ExitMainState::unload(void)
{
    debugERROR("TODO: fix this, we will do nothing for now\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
ExitMainState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    debugERROR("TODO: fix this, we will do nothing for now\n");
    return true;
}

} /* namespace cz */
