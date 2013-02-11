/*
 * FfmpegVideoPLayerAPI.cpp
 * Raul Monti
 * 30.09.2012
 */

#include "FfmpegVideoPlayerAPI.h"
#include "string.h"


const float VideoPlayerAPI::NO_VALUE = -1.0f;


////////////////////////////////////////////////////////////////////////////////

VideoPlayerAPI::VideoPlayerAPI(Ogre::Vector4 * screensize):
	mVideoPlayer(0),
	mIsplaying(false),
	mRepeat(false)
{

	Ogre::Vector4 *ss  = screensize;
	if(ss){
		mVideoPlayer = new VideoPlayer(ss->x, ss->y, ss->z,ss->w);
	}else{
		mVideoPlayer = new VideoPlayer;
	}

	mVideo.path = 0;
	mVideo.end = 0;
	mVideo.start = 0;

}

////////////////////////////////////////////////////////////////////////////////

VideoPlayerAPI::~VideoPlayerAPI(){
	if(mVideoPlayer){
		delete mVideoPlayer;
	}
	if(mVideo.path != 0){
		free(mVideo.path);
	}
}

////////////////////////////////////////////////////////////////////////////////

int VideoPlayerAPI::load(const char* path, float from, float to){

	//stop playing
	mIsplaying = false;

	if(to != NO_VALUE && from > to){
		return VIDEO_ERROR;
	}

	if(path == 0){
		if(!mVideoPlayer->is_loaded()){
			return VIDEO_ERROR;
		}
	}else{
		if(mVideoPlayer->is_loaded()){
			mVideoPlayer->unload();
		}
		if(mVideoPlayer->load(path) == VIDEO_ERROR){
			return VIDEO_ERROR;
		}
	}

	// Seek to the video start point
	if(from != NO_VALUE){
		if(mVideoPlayer->seek_time_stamp(static_cast<double>(from))
				== VIDEO_ERROR)
		{
			ASSERT(false);
			return VIDEO_ERROR;
		}
	}else{
		if(mVideoPlayer->seek_time_stamp(0.0f) == VIDEO_ERROR){
			debugRED("Seek time stamp 0.0f falla\n");
			return VIDEO_ERROR;
		}
	}

	if(!mVideoPlayer->is_playing()){
		mVideoPlayer->play();
	}

	// Update actual video information
	if(path != 0){
		mVideo.path = (char*)realloc(mVideo.path, sizeof(char)*strlen(path)+1);
		strcpy(mVideo.path, path);
	}
	mVideo.start = from;
	mVideo.end = to;

	return VIDEO_OK;
}

////////////////////////////////////////////////////////////////////////////////

int VideoPlayerAPI::play(void){
	if(!mVideoPlayer->is_loaded()){
		return VIDEO_ERROR;
	}else{
		mIsplaying = true;
	}
	return VIDEO_OK;
}


////////////////////////////////////////////////////////////////////////////////

int VideoPlayerAPI::update(double tslf){

	if(!mVideoPlayer->is_loaded())
	{
		return VIDEO_ENDED;
	}
	else if(!mIsplaying)
	{
		return VIDEO_OK;
	}

	ASSERT(mVideoPlayer->is_loaded());

	int err = VIDEO_OK;
	double pt = 0.0f;
	mVideoPlayer->get_playing_time_in_secs(pt);
	bool ended = mVideo.end != NO_VALUE && static_cast<float>(pt) >= mVideo.end;

	if(!ended){
		err = mVideoPlayer->update(tslf);
		if(err == VIDEO_ERROR){
			debugERROR("Problem updating the video :S\n");
			return VIDEO_ERROR;
		}
	}

	if(ended || err == VIDEO_ENDED){
		if(mRepeat){
			this->load( 0, mVideo.start, mVideo.end);
			this->play();
			return VIDEO_OK;
		}else{
			mIsplaying = false;
			mVideoPlayer->paint_white_screen();
			return VIDEO_ENDED;
		}
	}

	return VIDEO_OK;
}

////////////////////////////////////////////////////////////////////////////////

float VideoPlayerAPI::getVideoTime(void){

	if(mVideoPlayer->is_loaded()){
		double t = 0.0f;
		if(VIDEO_ERROR != mVideoPlayer->get_playing_time_in_secs(t)){
			if (static_cast<float>(t) > mVideo.end){
				return mVideo.end;
			}else if (static_cast<float>(t) < mVideo.start){
				return mVideo.start;
			}else{
				return static_cast<float>(t);
			}
		}else{
			return VIDEO_ERROR;
		}
	}else{
		return VIDEO_ERROR;
	}
}

////////////////////////////////////////////////////////////////////////////////
