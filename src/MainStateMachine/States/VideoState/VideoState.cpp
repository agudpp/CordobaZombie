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
VideoState::VideoState()
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

	for(int i = 0 ; i < VIDEO_STATE_LIST_SIZE; i++){
		mVpapi.queue_video(VIDEO_STATE_LIST[i]);
	}

	mVpapi.set_repeat(false);
	mVpapi.set_visible(true);

	if( VideoPlayerAPI::VIDEO_OK != mVpapi.play()){
		debugERROR("Can't play videos in VideoState :S player doesn't play\n");
	}

}





////////////////////////////////////////////////////////////////////////////////
MainMachineEvent VideoState::update(MainMachineInfo &info)
{
	while(1){

		GLOBAL_KEYBOARD->capture();
		if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_ESCAPE)){
			if(mVpapi.next() == VIDEO_ERROR){
				debugERROR("Error on videoPlayerApi while skipping video in "
									"video state\n");
				return MME_ERROR;
			}
		}
		int ret = mVpapi.update(0.0f);
		if (ret == VideoPlayerAPI::VIDEO_ERROR){
			debugERROR("Error while updating the videoPlayerApi in "
					"video state\n");
			return MME_ERROR;
		}else if( ret == VideoPlayerAPI::VIDEO_ENDED){
			return MME_DONE;
		}
	}

}




////////////////////////////////////////////////////////////////////////////////
void VideoState::exit(void)
{
//borrar cosas
}

