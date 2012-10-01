/*
 * FfmpegVideoPLayerAPI.cpp
 * Raul Monti
 * 30.09.2012
 */

#include "FfmpegVideoPlayerAPI.h"




//Constructor

VideoPlayerAPI::VideoPlayerAPI(Ogre::Vector4 * screensize):
	mVideoPlayer(0),
	mPlaylistIndex(0),
	mPlaylistModeOn(false),
	mPlaylistRepeatAll(false),
	mPlaylistRepeatVideo(false),
	mIsplaying(false),
	mPlaylistStart(0),
	mPlaylistEnd(0)
{

	Ogre::Vector4 *ss  = screensize;
	if(ss){
		mVideoPlayer = new VideoPlayer(ss->x, ss->y, ss->z,ss->w);
	}else{
		mVideoPlayer = new VideoPlayer;
	}
	mVideo.filename = 0;
	mVideo.end = 0;
	mVideo.start = 0;

}

//Destroyer

VideoPlayerAPI::~VideoPlayerAPI(){

	delete mVideoPlayer;

	while(!mPlaylist.empty()){
		if(mPlaylist.back() != 0){
			free(mPlaylist.back());
		}
		mPlaylist.pop_back();
	}
}




/*
 * Queue a video into the video play list. Can't queue if it's playing. The
 * video will be played from time 'from' to time 'to', both in seconds, if
 * specified. Return the index of the queued video inside the playlist on
 * success. VIDEO_ERROR if something goes wrong.
 */
int VideoPlayerAPI::queueVideo(const char * filename, double from, double to){

	if(mIsplaying){
		debugERROR("Attempt to queue while playing.\n");
		return VIDEO_ERROR;
	}

	Video v = (Video) malloc(sizeof(struct sVideo));

	v->filename = filename;
	v->start = from;
	v->end = to;

	mPlaylist.push_back(v);

	debugRAUL("Queuing video %s\n", mPlaylist.back()->filename);

	return mPlaylist.size()-1;
}

/*
 * Plays from the playlist
 */

int VideoPlayerAPI::playList( int from, int to){

	if(mPlaylist.empty()){
		debugERROR("Empty playlist, can't play.\n");
		return PLAYLIST_EMPTY;
	}

	// reset playlist limits
	if(from >= 0 && from < mPlaylist.size()){
		mPlaylistStart = from;
	}else{
		mPlaylistStart = 0;
	}

	if(to >= 0 && to <= mPlaylist.size()){
		mPlaylistEnd = to;
	}else{
		mPlaylistEnd = mPlaylist.size();
	}

	// reset playlist index
	mPlaylistIndex = 0;

	//Unload player if loaded
	if(mVideoPlayer->is_loaded()){
		mVideoPlayer->unload();
	}

	// Set mVideo as the first video in the play
	// list and load it into the video player.
	mVideo = *(mPlaylist[mPlaylistStart]);
	this->load();

	//
	mPlaylistModeOn = true;

	//
	mVideoPlayer->play();

	//
	mIsplaying = true;

	debugBLUE("Playing from playlist\n");

	return VIDEO_OK;
}


/**
 * Plays the vindex video loaded to the API.
 * vindex goes from 0 to playlist size -1.
 */
int VideoPlayerAPI::playFromPlaylist( int vindex, double from, double to)
{

	if(vindex < 0 || vindex >= mPlaylist.size()){
		return VIDEO_ERROR;
	}

	if(mVideoPlayer->is_loaded()){
		mVideoPlayer->unload();
	}

	mVideo = (*mPlaylist[vindex]);
	this->load();

	mPlaylistModeOn = false;

	mVideoPlayer->play();

	mIsplaying = true;

	return VIDEO_OK;

}


int
VideoPlayerAPI::playSingleVideo(const char * filename, double from, double to)
{
	if(mVideoPlayer->is_loaded()){
		mVideoPlayer->unload();
	}

	mVideo.filename = filename;
	mVideo.start = from;
	mVideo.end = to;

	if( VIDEO_ERROR == this->load()){
		return VIDEO_ERROR;
	}

	mPlaylistModeOn = false;
	mIsplaying = true;

	mVideoPlayer->play();

	return VIDEO_OK;
}




int VideoPlayerAPI::load(void){

	if(mVideoPlayer->is_loaded()){
		return VIDEO_ERROR;
	}

	// Load the video
	if(mVideo.filename != 0){
		if(mVideoPlayer->load(mVideo.filename) == VIDEO_ERROR){
			return VIDEO_ERROR;
		}
	}

	// Seek to the video start point
	if(mVideo.start != NO_VALUE){
		if(mVideoPlayer->seek_time_stamp(mVideo.start) == VIDEO_ERROR){
			return VIDEO_ERROR;
		}
	}

	return VIDEO_OK;
}



int VideoPlayerAPI::update(double tslf){

	if(!mIsplaying){
		// Not playing
		return VIDEO_OK;
	}

	ASSERT(mVideoPlayer->is_loaded());

	int err = VIDEO_OK;
	double pt = 0.0f;
	mVideoPlayer->get_playing_time_in_secs(pt);
	bool ended = mVideo.end != NO_VALUE && pt >= mVideo.end;

	if(!ended){
		err = mVideoPlayer->update(tslf);

		if(err == VIDEO_ERROR){
			debugERROR("Problem updating the video :S\n");
			return VIDEO_ERROR;
		}
	}

	if(ended || err == VIDEO_ENDED){
		if(mPlaylistModeOn){
			this->next();
		}else{
			mIsplaying = false;
		}
		return VIDEO_ENDED;
	}

	return VIDEO_OK;
}


/*
 * Play next video in queue. Stops actual playing video if there is one,
 * doesn't matter if there is a next video or not.
 */

int VideoPlayerAPI::next(void){

	if(!mPlaylistModeOn){
		return VIDEO_ERROR;
	}

	if(mPlaylist.empty()){
		return PLAYLIST_EMPTY;
	}

	if(mVideoPlayer->is_loaded()){
		mVideoPlayer->unload();
	}

	mVideoPlayer->paint_black_screen();

	if(!mPlaylistRepeatVideo){
		// Move to next video if repeat video isn't activated.
		mPlaylistIndex = (mPlaylistIndex + 1) % mPlaylist.size();

	}else{
		mVideo = *(mPlaylist[mPlaylistIndex]);
		this->load();
		mVideoPlayer->play();
		return VIDEO_OK;
	}

	// Jump to start if we reached the last video and repeat all is activated.
	if(mPlaylistIndex == mPlaylistEnd){
		if(!mPlaylistRepeatAll){
			mIsplaying = false;
			return VIDEO_ENDED;
		}else{
			mPlaylistIndex = mPlaylistStart;
		}
	}

	// Get next video and load it
	mVideo = *(mPlaylist[mPlaylistIndex]);
	this->load();
	mVideoPlayer->play();

	return VIDEO_OK;
}

