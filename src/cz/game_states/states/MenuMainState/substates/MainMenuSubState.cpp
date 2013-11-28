/*
 * MainMenuSubState.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: agustin
 */

#include "MainMenuSubState.h"

#include <debug/DebugUtil.h>
#include <global_data/GlobalData.h>

namespace cz {

OgreCommon MainMenuSubState::sOgreInfo;
mm::SoundHandler* MainMenuSubState::sSoundHandler = 0;
CommonHandlers MainMenuSubState::sCommonHandlers;


////////////////////////////////////////////////////////////////////////////////
bool
MainMenuSubState::convertToAbsolute(Ogre::String& relPath) const
{
    // get the root resource path
    std::string rootPath;
    if (!GlobalData::getRootResourcesPath(rootPath)) {
        return false;
    }

    // now append the rel path to it
    relPath = rootPath + relPath;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MainMenuSubState::MainMenuSubState()
{
    debugERROR("We need to uncomment this asserts\n");
//    ASSERT(sSoundManager != 0);
    ASSERT(sCommonHandlers.frontEndManager);
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
MainMenuSubState::setSoundHandler(mm::SoundHandler* soundHandler)
{
    sSoundHandler = soundHandler;
}

////////////////////////////////////////////////////////////////////////////////
void
MainMenuSubState::setCommonHandlers(const CommonHandlers& ch)
{
    sCommonHandlers = ch;
}


} /* namespace cz */
