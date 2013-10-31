/*
 * MainStateBuilder.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#include "MainStateBuilder.h"

#include "IMainState.h"
#include "MainStateID.h"

namespace cz {

MainStateBuilder::MainStateBuilder()
{

}

MainStateBuilder::~MainStateBuilder()
{
}

////////////////////////////////////////////////////////////////////////////////
void
MainStateBuilder::setOgreCommon(const OgreCommon& ogreData)
{
    // set this data to the states
    IMainState::setOgreData(ogreData);
}

////////////////////////////////////////////////////////////////////////////////
IMainState*
MainStateBuilder::buildState(const MainStateID& id) const
{
    ASSERT(false && "TODO, here we should return a new instance of the associated"
        " state, check issue #192");
    return 0;
}

} /* namespace cz */
