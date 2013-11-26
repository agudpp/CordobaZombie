/*
 * MainMenuSubState.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: agustin
 */

#include "MainMenuSubState.h"

#include <debug/DebugUtil.h>

namespace cz {

OgreCommon MainMenuSubState::sOgreInfo;
mm::SoundManager* MainMenuSubState::sSoundManager = 0;


////////////////////////////////////////////////////////////////////////////////
MainMenuSubState::MainMenuSubState()
{
    debugERROR("We need to uncomment this asserts\n");
//    ASSERT(sSoundManager != 0);
}

////////////////////////////////////////////////////////////////////////////////
MainMenuSubState::~MainMenuSubState()
{

}

////////////////////////////////////////////////////////////////////////////////
void
MainMenuSubState::setOgreData(const OgreCommon& ogreData)
{
    sOgreInfo = ogreData;
}

////////////////////////////////////////////////////////////////////////////////
void
MainMenuSubState::setSoundManager(mm::SoundManager* soundManager)
{
    sSoundManager = soundManager;
}

} /* namespace cz */
