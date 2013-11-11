/*
 * IntroMainState.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#include <string.h>

#include <video/OgreVideoPlayer.h>
#include <os_utils/OSHelper.h>
#include <ResourceHandler.h>
#include <game_states/IMainState.h>



#include "IntroMainState.h"

namespace cz {



const char* IntroMainState::INTRO_VIDEO_NAME = "intro_video.ogg";
const char* IntroMainState::INTRO_RC_PATH =
		"main_states/intro_main_state/resources.cfg";




IntroMainState::IntroMainState() :
		mVideoPlayer(-1,1,1,-1,sOgreInfo.sceneMngr,800,600)
{
}


///////////////////////////////////////////////////////////////////////////////
IntroMainState::~IntroMainState()
{
    // TODO Auto-generated destructor stub
}


///////////////////////////////////////////////////////////////////////////////
bool
IntroMainState::configureState(const MainStateInformation& info)
{
	return true;
}


///////////////////////////////////////////////////////////////////////////////
bool
IntroMainState::getResourcesToLoad(ResourceGroupList& resourceList)
{
	if(sRcHandler->getResourceRootPath() == ""){
		debugERROR("No root path for the resources manager :S\n");
		return false;
	}
	std::string path;
	sRcHandler->getResourceFullPath(INTRO_RC_PATH, path);
	mrg.setOgreResourceFile(path);
	resourceList.push_back(mrg);
	return true;
}


///////////////////////////////////////////////////////////////////////////////
bool
IntroMainState::readyToGo(void)
{
	std::string path;
	if(!sRcHandler->getResourcePath("Videos",INTRO_VIDEO_NAME,path)){
		debugERROR("Can find intro video of name <%s> in "
				"resource group <Video>\n", INTRO_VIDEO_NAME);
		return false;
	}

	if(mm::OgreVideoPlayer::ERROR ==
			mVideoPlayer.queue(path.c_str(), path.c_str(),0,-1))
	{
		debugERROR("Can't queue intro video at %s\n", path.c_str());
		return false;
	}

	if(mm::OgreVideoPlayer::OK != mVideoPlayer.play()){
		debugERROR("Video player doesn't play :(\n");
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
bool
IntroMainState::update(float timeFrame)
{

	int err = mVideoPlayer.update(timeFrame);

	if(err == mm::OgreVideoPlayer::ERROR){
		mEventInfo = EVENT_DONE;
		return false;
	}else if(err == mm::OgreVideoPlayer::OK){
		mEventInfo = EVENT_DONE;
	}else if(err == mm::OgreVideoPlayer::ENDED){
		mEventInfo == EVENT_EXIT;
		return false;
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////////
bool
IntroMainState::unload(void)
{
	return true;
}


///////////////////////////////////////////////////////////////////////////////
bool
IntroMainState::getResourcesToUnload(ResourceGroupList& resourceList)
{
	resourceList.push_back(mrg);
	return true;
}


///////////////////////////////////////////////////////////////////////////////

const MainStateID&
IntroMainState::ID(void) const
{
	return MainStateID::IntroState;
}


} /* namespace cz */
