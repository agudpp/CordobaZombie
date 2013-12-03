/*
 * PlayingMainState.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#include "PlayingMainState.h"

namespace cz {

PlayingMainState::PlayingMainState()
{
    // TODO Auto-generated constructor stub

}

PlayingMainState::~PlayingMainState()
{
    // TODO Auto-generated destructor stub
}


////////////////////////////////////////////////////////////////////////////
//                           Inherited methods
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
const MainStateID&
PlayingMainState::ID(void) const
{
    debugERROR("TODO\n");
    static MainStateID id;
    return id;
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayingMainState::configureState(const MainStateInformation& info)
{
    debugERROR("TODO\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayingMainState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    debugERROR("TODO\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayingMainState::readyToGo(void)
{
    debugERROR("TODO\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayingMainState::update(float timeFrame)
{
    debugERROR("TODO\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayingMainState::unload(void)
{
    debugERROR("TODO\n");
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayingMainState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    debugERROR("TODO\n");
    return true;
}

} /* namespace cz */
