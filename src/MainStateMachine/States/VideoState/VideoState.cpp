/*
 * VideoState.cpp
 *
 *  Created on: 01/09/2012
 *      Author: raul
 */



#include "DebugUtil.h"
#include "VideoState.h"
#include "MainStateMachineDefs.h"
#include "Util.h"
#include "InputKeyboard.h"



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace{
	const int VIDEO_STATE_LIST_SIZE = 2;

	const char* VIDEO_STATE_LIST[VIDEO_STATE_LIST_SIZE] =
				{
				"perrosInfectados.ogg",
				"menu_ar3:2.ogg"
				};
}

////////////////////////////////////////////////////////////////////////////////



VideoState::VideoState():
	IMainState("VideoState"),
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

	mVideoIndex = 0;

	if(!mVpapi){
		mVpapi = new VideoPlayerAPI;
	}

	ASSERT(mVpapi);

	ASSERT(VIDEO_STATE_LIST_SIZE > 0)
	Ogre::String videoPath;
	Common::Util::getResourcePath(Ogre::String("Videos"),
			Ogre::String(VIDEO_STATE_LIST[0]), videoPath);
	if(VideoPlayerAPI::VIDEO_ERROR == mVpapi->load(videoPath.c_str())){
		debugERROR("Can't play videos in video state.\n");
		this->exit();
	}
	mVpapi->setRepeat(false);
	mVpapi->setVisible(true);

	if( VideoPlayerAPI::VIDEO_OK != mVpapi->play()){
		debugERROR("Can't play videos in VideoState :S player doesn't play\n");
		this->exit();
	}

}


////////////////////////////////////////////////////////////////////////////////

int VideoState::checkInput(void)
{

	GLOBAL_KEYBOARD->capture();
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_ESCAPE)){
		return this->nextVideo();
	}

    return OK;
}


////////////////////////////////////////////////////////////////////////////////

int VideoState::nextVideo(void){
	mVpapi->pause();
	if(mVideoIndex+1 < VIDEO_STATE_LIST_SIZE){
		// Load and play next video
		mVideoIndex++;

		Ogre::String videoPath;
		Common::Util::getResourcePath(Ogre::String("Videos"),
				Ogre::String(VIDEO_STATE_LIST[mVideoIndex]), videoPath);
		if(mVpapi->load(videoPath.c_str()) != VideoPlayerAPI::VIDEO_OK)
		{
			return ERROR;
		}

		if(mVpapi->play() != VideoPlayerAPI::VIDEO_OK)
		{
			return ERROR;
		}
	}else{
		// No more videos to play
		debugRED("NO more videos\n");
		return DONE;
	}

	return OK;
}

////////////////////////////////////////////////////////////////////////////////
MainMachineEvent VideoState::update(MainMachineInfo &info)
{

	Ogre::Timer timer;
	float timeStamp = 0;
	Ogre::Real frameTime = 0;

	while(1){

		timeStamp = timer.getMilliseconds();

		int ret = this->checkInput();
		if(ret == ERROR){
			return MME_ERROR;
		}else if(ret == DONE){
			return MME_DONE;
		}

//		debugRAUL("is playing %d\n", mVpapi.isPlaying());
//		debugRAUL("is visible %d\n", mVpapi.isVisible());
//		debugRAUL("TSLF %lf\n",frameTime);

		ret = mVpapi->update(frameTime);
		if (ret == VideoPlayerAPI::VIDEO_ERROR){
			debugERROR("Error while updating the videoPlayerApi in "
					"video state\n");
			return MME_ERROR;
		}else if( ret == VideoPlayerAPI::VIDEO_ENDED){
			ret = this->nextVideo();
			if(ret == DONE){
				return MME_DONE;
			}else if(ret == ERROR){
				return MME_ERROR;
			}
		}

		// render the frame
		if(!GLOBAL_ROOT->renderOneFrame()){
			return MME_DONE; //TODO: poner un erro real aca
		}
		// This must be called when we use the renderOneFrame approach
		Ogre::WindowEventUtilities::messagePump();

		frameTime = (timer.getMilliseconds() - timeStamp) * 0.001;

	}

}




////////////////////////////////////////////////////////////////////////////////
void VideoState::exit(void)
{
	//borrar cosas

	if(mVpapi){
		delete mVpapi;
	}
}


////////////////////////////////////////////////////////////////////////////////
void VideoState::getResources(ResourcesInfoVec &resourcesList)
{
}
