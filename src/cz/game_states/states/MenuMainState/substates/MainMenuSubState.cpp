/*
 * MainMenuSubState.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: agustin
 */

#include "MainMenuSubState.h"

#include <debug/DebugUtil.h>

namespace cz {

static OgreCommon MainMenuSubState::sOgreInfo;
static mm::SoundManager* MainMenuSubState::sSoundManager = 0;


////////////////////////////////////////////////////////////////////////////////
MainMenuSubState::MainMenuSubState()
{
    ASSERT(sSoundManager != 0);
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
