/*
 * ExitState.cpp
 *
 *  Created on: 06/10/2012
 *      Author: agustin
 *
 */

#include "ExitState.h"

#include "DebugUtil.h"

ExitState::ExitState() :
IMainState("ExitState")
{

}

ExitState::~ExitState()
{

}


////////////////////////////////////////////////////////////////////////////////
void ExitState::enter(const MainMachineInfo &info)
{
    // TODO:
    ASSERT(false);
}

////////////////////////////////////////////////////////////////////////////////
MainMachineEvent ExitState::update(MainMachineInfo &info)
{
    // TODO:
    ASSERT(false);
}

////////////////////////////////////////////////////////////////////////////////
void ExitState::exit(void)
{
    // TODO:
    ASSERT(false);
}
