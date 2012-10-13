/*
 * VideoState.cpp
 *
 *  Created on: 01/09/2012
 *      Author: raul
 */



#include "DebugUtil.h"
#include "VideoState.h"
#include "MainStateMachineDefs.h"




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
VideoState::VideoState():
	mVideoIndex(0)
{
}

////////////////////////////////////////////////////////////////////////////////
VideoState::~VideoState()
{
	exit();
}




////////////////////////////////////////////////////////////////////////////////
void VideoState::enter(const MainMachineInfo &info)
{
	// Video screen will cover all the Ogre main window because we are not
	// giving the sizes as arguments:

	ASSERT(mVpapi);
	mVideoIndex = 0;

	ASSERT(VIDEO_STATE_LIST_SIZE > 0)
	if(VideoPlayerAPI::VIDEO_ERROR == mVpapi.load(VIDEO_STATE_LIST[0])){
		debugERROR("Can't play videos in video state.\n");
		this->exit();
	}
	mVpapi.setRepeat(false);
	mVpapi.setVisible(true);

	if( VideoPlayerAPI::VIDEO_OK != mVpapi.play()){
		debugERROR("Can't play videos in VideoState :S player doesn't play\n");
		this->exit();
	}

}


////////////////////////////////////////////////////////////////////////////////

int VideoState::checkInput(void)
{
    if(input::InputKeyboard::isKeyDown(input::KC_ESCAPE)){
    	return this->nextVideo();
    }
    return OK;
}


////////////////////////////////////////////////////////////////////////////////

int VideoState::nextVideo(void){
	mVpapi.pause();
	if(mVideoIndex+1 < VIDEO_STATE_LIST_SIZE){
		// Load and play next video
		mVideoIndex++;
		if(mVpapi->load(VIDEO_STATE_LIST[mVideoIndex]) !=
				VideoPlayerAPI::VIDEO_OK)
		{
			return ERROR;
		}
		if(mVpapi->play() != VideoPlayerAPI::VIDEO_OK)
		{
			return ERROR;
		}
	}else{
		// No more videos to play
		return DONE;
	}

	return OK;
}

////////////////////////////////////////////////////////////////////////////////
MainMachineEvent VideoState::update(MainMachineInfo &info)
{
	while(1){

		int res = this->checkInput();
		if(res == ERROR){
			return MME_ERROR;
		}else if(res == DONE){
			return MME_DONE;
		}

		debugTODO("Need global time frame here:S\n");
		int ret = mVpapi.update(0.0f);
		if (ret == VideoPlayerAPI::VIDEO_ERROR){
			debugERROR("Error while updating the videoPlayerApi in "
					"video state\n");
			return MME_ERROR;
		}else if( ret == VideoPlayerAPI::VIDEO_ENDED){
			res = this->nextVideo();
			if(res == DONE){
				return MME_DONE;
			}else if(res == ERROR){
				return MME_ERROR;
			}
		}
	}

}




////////////////////////////////////////////////////////////////////////////////
void VideoState::exit(void)
{
//borrar cosas
}

