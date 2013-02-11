/*
 * FfmpegVideoPLayerAPI.h
 * Raul Monti
 * 30.09.2012
 */

#ifndef FFMPEG_VIDEO_PLAYER_API_H
#define FFMPEG_VIDEO_PLAYER_API_H

#include "FfmpegVideoPlayer.h"


#define VIDEOS_RESOURCE_GROUP "Videos"


typedef struct sVideo{
	char *path;
	float start;
	float end;

} * Video;


class VideoPlayerAPI{

public:
	enum{
		PLAYLIST_EMPTY,
	};

	static const float NO_VALUE;
	static const int VIDEO_ERROR = VideoPlayer::VIDEO_ERROR;
	static const int VIDEO_ENDED = VideoPlayer::VIDEO_ENDED;
	static const int VIDEO_OK = VideoPlayer::VIDEO_OK;

public:

	/*
	 * screensize defines the size of the video in the screen, using top left
	 * bottom right coordinates, going from -1 to 1 each. Default values will
	 * make the video cover all the screen.
	 */
	VideoPlayerAPI(Ogre::Vector4 * screensize = 0);

	/*
	 * Destroyer
	 */
	virtual	~VideoPlayerAPI();

	/*
	 * Load video at 'path'. Video will be played from second 'from' to second
	 * 'to' when calling play method. If no 'from' is specified then it will
	 * play it from the start; if no 'to' is defined then it will play it till
	 * it ends. If path isn't defined then it will change actual video
	 * parameters to 'from' and 'to'.
	 * @Note: if there is no video loaded then not passing path argument will
	 * 	return VIDEO_ERROR.
	 * @Note: the player stops when asked to load, so you need to call play
	 * 	method after loading to continue playing.
	 */
	int load(const char *path = 0,
			 float from = NO_VALUE,
			 float to = NO_VALUE);

	/*
	 * Play previously loaded video.
	 */
	int play(void);

	/*
	 * Pauses the video player.
	 */
	inline int pause(void);

	/*
	 * Need to call this method every frame to update the videos.
	 */
	int update(double tslf);

	/*
	 * Get the video position in seconds.
	 */
	float getVideoTime(void);

	/*
	 * Set repeat video to 'b'
	 */
	inline void setRepeat(bool b);

	inline bool isPlaying(void);

	inline void setVisible(bool b);

	inline bool isVisible(void);


private:

	VideoPlayer					*mVideoPlayer;
	bool						mRepeat;
	bool						mIsplaying;
	struct sVideo				mVideo;

};



inline bool VideoPlayerAPI::isPlaying(void){
	return mIsplaying;
}

inline void VideoPlayerAPI::setRepeat(bool b){
	mRepeat = b;
}

inline int VideoPlayerAPI::pause(void){
	mIsplaying = false;
}

inline void VideoPlayerAPI::setVisible(bool b){
	mVideoPlayer->set_visible(b);
}

inline bool VideoPlayerAPI::isVisible(void){
	return mVideoPlayer->is_visible();

}


#endif //FFMPEG_VIDEO_PLAYER_API_H
