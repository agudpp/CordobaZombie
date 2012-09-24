
#include "FfmpegVideoPlayerAPI.h"



/*
 * TODO Method play(int videoindex, float from, float to)
 * 		plays the i-th video from the time 'from' to the time 'to'
 *
 * TODO	Method queue_video and play, with option to define start and end
 * 		in frames metric instead of seconds.
 */



//Constructor

VideoPlayerAPI::VideoPlayerAPI(Ogre::Vector4 * screensize):
	videoplayer(0),
	actualvideo(0),
	isplaying(false),
	mRepeat(false),
	mRepeatAll(false)
{

	Ogre::Vector4 *ss  = screensize;
	if(ss){
		videoplayer = new VideoPlayer(ss->x, ss->y, ss->z,ss->w);
	}else{
		videoplayer = new VideoPlayer;
	}


}

//Destroyer

VideoPlayerAPI::~VideoPlayerAPI(){

	delete videoplayer;

	while(!playlist.empty()){

		free(playlist.back());
		playlist.pop_back();

	}
}



/*
 * Queue a video into the video play list. Can't queue if it's playing.
 */

int VideoPlayerAPI::queue_video(const char * filename){

	if(isplaying){
		debugERROR("Attempt to queue while playing.\n");
		return VIDEO_ERROR;
	}

	Video v = (Video) malloc(sizeof(struct sVideo));

	v->filename = filename;
	v->start = NO_VALUE;
	v->end = NO_VALUE;

	playlist.push_back(v);

	debugRAUL("Video name %s\n", playlist.back()->filename);

	return VIDEO_OK;
}

/*
 * Queue a video into the video play list. Can't queue if it's playing. The
 * video will be played from time 'from' to time 'to', both in seconds.
 */
int VideoPlayerAPI::queue_video(const char * filename, double from, double to){

	if(isplaying){
		debugERROR("Attempt to queue while playing.\n");
		return VIDEO_ERROR;
	}

	Video v = (Video) malloc(sizeof(struct sVideo));

	v->filename = filename;
	v->start = from;
	v->end = to;

	playlist.push_back(v);

	debugRAUL("Video name %s\n", playlist[0]->filename);

	return VIDEO_OK;
}

/*
 * Play :P
 */

int VideoPlayerAPI::play(void){

	if(playlist.empty()){
		debugERROR("Empty playlist, can't play.\n");
		return VIDEO_ERROR;
	}

	if(!isplaying){
		isplaying = true;
	}

	debugBLUE("Video PLAY\n");

	return VIDEO_OK;
}


/**
 * Plays the vindex video loaded to the API.
 * vindex goes from 0 to playlist size -1.
 */
int VideoPlayerAPI::play(int vindex){

	if(vindex < 0 || vindex >= playlist.size()){
		return VIDEO_ERROR;
	}

	videoplayer->unload();

	actualvideo = vindex;

	play();

	return VIDEO_OK;

}


int VideoPlayerAPI::update(double tslf){

	if(!isplaying){
		//debugBLUE("SKIP UPDATE (NOT PLAYING)\n");
		return VIDEO_OK;
	}

	ASSERT(!playlist.empty());

	// si no esta cargado entonces intentamos cargar el siguiente video
	if(!videoplayer->is_loaded()){

		if(actualvideo >= playlist.size()){
			// no more videos to play
			isplaying = false;
			debugGREEN("Out of videos, then STOP\n");
			return VIDEO_ENDED;
		}
		//else load video
		if(videoplayer->load(playlist[actualvideo]->filename) == VIDEO_ERROR){
			debugERROR("Can't load video player :S \n");
			return VIDEO_ERROR;
		}
		if(playlist[actualvideo]->start != NO_VALUE){
			// if want to start somewhere else
			if (videoplayer->seek_time_stamp(playlist[actualvideo]->start)
					== VIDEO_ERROR)
			{
				debugERROR("Can't seek in video :S \n");
				return VIDEO_ERROR;
			}
		}
		videoplayer->play();
	}

	int err = videoplayer->update(tslf);
	if(err == VIDEO_ERROR){
		debugERROR("VideoPlayer error while updating :S \n");
		return VIDEO_ERROR;
	}

	if(err == VIDEO_ENDED){
		debugBLUE("Video ended :S \n");
	}

	double pt = 0;
	videoplayer->get_playing_time_in_secs(pt);
	Video video = playlist[actualvideo];
	bool flag = video->end != NO_VALUE;

	if(err == VIDEO_ENDED || (flag && pt >= video->end)){
		debugGREEN("Ended because reached video->end %lf %lf\n", pt, video->end);
		videoplayer->unload();
		if(!mRepeat){
			actualvideo += 1;
		}
		if(actualvideo == playlist.size()){
			if(mRepeatAll){
				actualvideo = 0;
			}
		}
	}
	return VIDEO_OK;
}


/*
 * Play next video in queue. Stops actual playing video if there is one,
 * doesn't matter if there is a next video or not.
 */

int VideoPlayerAPI::next(void){

	if(playlist.empty()){
		return VIDEO_ERROR;
	}

	videoplayer->unload();

	videoplayer->paint_black_screen();

	if(!mRepeat){
		actualvideo += 1;
	}
	if(actualvideo >= playlist.size()){
		if(mRepeatAll){
			actualvideo = 0;
		}
	}

	return VIDEO_OK;
}

