/*
 * IntroMainState.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#include "IntroMainState.h"

#include <string.h>

#include <video/OgreVideoPlayer.h>
#include <os_utils/OSHelper.h>
#include <ResourceHandler.h>
#include <game_states/IMainState.h>
#include <video/OgreVideoPlayer.h>



namespace cz {



const char* IntroMainState::INTRO_VIDEO_NAME = "intro.ogv";
const char* IntroMainState::INTRO_RC_PATH =
		"multimedia/video/resources.cfg";




IntroMainState::IntroMainState()
{
}


///////////////////////////////////////////////////////////////////////////////
IntroMainState::~IntroMainState()
{
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
		debugERROR(	"No root path for the resources manager :S "
					"Esto quizas deberia ser un ASSERT?\n");
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
    if (!sRcHandler->getResourcePath("Videos", INTRO_VIDEO_NAME, path)) {
        debugERROR("Can find intro video of name <%s> in "
            "resource group <Video>\n", INTRO_VIDEO_NAME);
        return false;
    }

    // remove all the previous videos just in case
    sVideoPlayer->dequeueAll();

    // enqueue the video
    if (mm::OgreVideoPlayer::ERROR
        == sVideoPlayer->queue(path.c_str(), path.c_str(), 0, -1)) {
        debugERROR("Can't queue intro video at %s\n", path.c_str());
        return false;
    }

    if (mm::OgreVideoPlayer::OK != sVideoPlayer->play()) {
        debugERROR("Video player doesn't play :(\n");
        return false;
    }

    // show the video player
    sVideoPlayer->setVisible(true);

    return true;
}


///////////////////////////////////////////////////////////////////////////////
bool
IntroMainState::update(float timeFrame)
{
    // check if the video is still playing then we have nothing to do
    if (sVideoPlayer->isPlaying()) {
        return true;
    }

    // else we finish, nothing else to do here
    mEventInfo = MainStateEvent::EVENT_DONE;

    return false;
}


///////////////////////////////////////////////////////////////////////////////
bool
IntroMainState::unload(void)
{
    // remove all the videos just in case
    sVideoPlayer->dequeueAll();

    // hide the video player
    sVideoPlayer->setVisible(false);
    return true;
}


///////////////////////////////////////////////////////////////////////////////
bool
IntroMainState::getResourcesToUnload(ResourceGroupList& resourceList)
{
	//ASSERT(!mrg.sections().empty());
	resourceList.push_back(mrg);
	return true;
}


///////////////////////////////////////////////////////////////////////////////

const MainStateID&
IntroMainState::ID(void) const
{
    static const MainStateID id = MainStateID::IntroState;
	return id;
}


} /* namespace cz */
