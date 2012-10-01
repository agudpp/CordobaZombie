/*
 * FfmpegVideoPLayerAPI.h
 * Raul Monti
 * 30.09.2012
 */

#ifndef FFMPEG_VIDEO_PLAYER_API_H
#define FFMPEG_VIDEO_PLAYER_API_H

#include "FfmpegVideoPlayer.h"



typedef struct sVideo{
	const char *filename;
	double start;
	double end;

} * Video;


class VideoPlayerAPI{

public:
	enum{
		PLAYLIST_EMPTY,
	};

	static const double NO_VALUE = -1.0f;
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

	virtual	~VideoPlayerAPI();

	/*
	 * Queue the video at path 'filename' into the API playlist, to be played
	 * from instant 'from' to instant 'to', bought in seconds. Default values
	 * will play the entire video when asked to play.
	 * On success returns the index in the playlist where the video was placed.
	 */
	int queueVideo( const char * filename
				   , double from = NO_VALUE
				   , double to = NO_VALUE
				   );

	/*
	 * Start playing videos from the playlist. The playlist should have been
	 * loaded using the queueVideo method. The playlist will be played in a
	 * circular mode, starting from video at index 'from' and ending at video
	 * on index 'to'.
	 */
	int playList(int from = -1, int to = -1);

	/*
	 * Play video at path 'filename', starting from second 'from' to second 'to'
	 * in it's video stream.
	 */
	int playSingleVideo( const char * filename
					   , double from = NO_VALUE
					   , double to = NO_VALUE
					   );

	/*
	 * Play video at position 'vindex' in the playlist, starting from second
	 * 'from' to second 'to' in it's video stream.
	 */
	int playFromPlaylist( int vindex
						, double from = NO_VALUE
						, double to = NO_VALUE
						);

	/*
	 * If there is a video being played, it pauses or un-pauses it.
	 */
	inline int pause(void);

	/*
	 * Play the next video from the playlist. Should first call playList method
	 * to start playing from playlist. Then you can use this method to force
	 * the player to play the next video in the playlist.
	 */
	int next(void);

	/*
	 * Need to call this method every frame to update the videos.
	 */
	int update(double tslf);


	inline void setRepeatPlaylistVideo(bool v);

	inline void setRepeatPlaylistAll(bool v);

	inline bool isPlaying(void);

	inline void setVisible(bool visible);

	inline bool isVisible(void);

	inline int getQueueSize(void);

	inline int getActualVideoIndex(void);

protected:

	/*
	 * Loads mVideo into mVideoPlayer
	 */
	int load(void);

protected:

	VideoPlayer					*mVideoPlayer;
	std::vector<Video>			mPlaylist;
	int							mPlaylistIndex;
	bool						mPlaylistRepeatVideo;
	bool						mPlaylistRepeatAll;
	bool 						mPlaylistModeOn;
	bool						mIsplaying;
	struct sVideo				mVideo;
	// Playlist plays between mPlaylistStart and mPlaylistEnd -1
	int							mPlaylistStart;
	int							mPlaylistEnd;
};



inline bool VideoPlayerAPI::isPlaying(void){
	return mIsplaying;
}

inline void VideoPlayerAPI::setRepeatPlaylistVideo(bool v){
	mPlaylistRepeatVideo = v;
}

inline void VideoPlayerAPI::setRepeatPlaylistAll(bool v){
	mPlaylistRepeatAll = v;
}

inline int VideoPlayerAPI::pause(void){
	if(mIsplaying){
		mIsplaying = false;
	}
	else if(mVideoPlayer->is_loaded()){
		mIsplaying = true;
	}
}

inline void VideoPlayerAPI::setVisible(bool visible){
	mVideoPlayer->set_visible(visible);
}


inline bool VideoPlayerAPI::isVisible(void){
	return mVideoPlayer->is_visible();

}

inline int VideoPlayerAPI::getQueueSize(void){
	return mPlaylist.size();
}

inline int VideoPlayerAPI::getActualVideoIndex(void){
	return mPlaylistIndex;
}

#endif //FFMPEG_VIDEO_PLAYER_API_H
