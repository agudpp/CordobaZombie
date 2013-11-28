/*
 * IMainState.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#include "IMainState.h"

#include <debug/DebugUtil.h>

namespace cz {

OgreCommon IMainState::sOgreInfo;
mm::VideoPlayer* IMainState::sVideoPlayer = 0;
mm::SoundHandler* IMainState::sSoundHandler = 0;
const rrh::ResourceHandler *IMainState::sRcHandler = 0;
CommonHandlers IMainState::sCommonHandlers;


IMainState::IMainState()
{
    // we will ensure that we have the ogre information already set
    //
    ASSERT(sOgreInfo.camera);
    ASSERT(sOgreInfo.renderWindow);
    ASSERT(sOgreInfo.root);
    ASSERT(sOgreInfo.sceneMngr);
    debugERROR("We need to uncomment this asserts\n");
//    ASSERT(sSoundHandler);
//    ASSERT(sVideoPlayer);
    ASSERT(sRcHandler);
    ASSERT(sCommonHandlers.frontEndManager);
    ASSERT(sCommonHandlers.inputHelper);

}

////////////////////////////////////////////////////////////////////////////////
void
IMainState::setOgreData(const OgreCommon& ogreData)
{
    sOgreInfo = ogreData;
}

void
IMainState::setVideoPlayer(mm::VideoPlayer* videoPlayer)
{
    sVideoPlayer = videoPlayer;
}

void
IMainState::setSoundHandler(mm::SoundHandler* soundHandler)
{
    sSoundHandler = soundHandler;
}

void
IMainState::setRcHandler(const rrh::ResourceHandler* rch)
{
	sRcHandler = rch;
}
void
IMainState::setCommonHandlers(const CommonHandlers& ch)
{
    sCommonHandlers = ch;
}



} /* namespace cz */
