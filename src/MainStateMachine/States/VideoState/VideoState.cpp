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

//TODO Cargar la lista de videos desde un xml

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
	mVpapi(0),
	mVideoIndex(0),
	keyPress(false)
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
		Ogre::Vector4 screensize(-1.0f, 1.0f, 1.0f, -1.0f);
		mVpapi = new VideoPlayerAPI(&screensize);
	}

	ASSERT(mVpapi);

	ASSERT(VIDEO_STATE_LIST_SIZE > 0)

	// If got videos, try to load one
	for(int i = 0; i < VIDEO_STATE_LIST_SIZE; i++, mVideoIndex++){
		Ogre::String videoPath;
		Common::Util::getResourcePath(Ogre::String("Videos"),
				Ogre::String(VIDEO_STATE_LIST[i]), videoPath);

		if(VideoPlayerAPI::VIDEO_OK == mVpapi->load(videoPath.c_str())){
			if( VideoPlayerAPI::VIDEO_OK == mVpapi->play()){
				mVpapi->setRepeat(false);
				mVpapi->setVisible(true);
				break;
			}else{
				debugERROR("Can't play %s an VideoState :S\n",
						VIDEO_STATE_LIST[i]);
			}
		}else{
			debugERROR("Can't load %s at VideoState :S.\n",
					VIDEO_STATE_LIST[i]);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////

int VideoState::checkInput(void)
{

	GLOBAL_KEYBOARD->capture();
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_ESCAPE)){
		if(!keyPress){
			keyPress = true;
			return this->nextVideo();
		}
	}else{
		keyPress = false;
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

		// Check input
		int ret = this->checkInput();
		if(ret == ERROR){
			debugERROR("Problem checking input at VideoState\n");
			return MME_DONE;
		}else if(ret == DONE){
			return MME_DONE;
		}

		// Update Video
		ret = mVpapi->update(frameTime);
		if (ret == VideoPlayerAPI::VIDEO_ERROR){
			debugERROR("Error while updating the videoPlayerApi in "
					"VideoState\n");
			return MME_DONE;
		}else if( ret == VideoPlayerAPI::VIDEO_ENDED){
			// Get next video to play
			ret = this->nextVideo();
			if(ret == DONE){
				return MME_DONE;
			}else if(ret == ERROR){
				debugERROR("Problem getting next video in VideoState\n");
				return MME_DONE;
			}
		}

		// render the frame
		if(!GLOBAL_ROOT->renderOneFrame()){
			debugERROR("Problem while rendering frame at VideoState\n");
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
void VideoState::getResources(ResourcesInfoVec &resourcesList) const
{
}
