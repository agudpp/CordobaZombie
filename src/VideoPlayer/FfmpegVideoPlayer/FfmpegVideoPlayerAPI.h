

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

	static constexpr double NO_VALUE = -1.0f;
	static const int VIDEO_ERROR = VideoPlayer::VIDEO_ERROR;
	static const int VIDEO_ENDED = VideoPlayer::VIDEO_ENDED;
	static const int VIDEO_OK = VideoPlayer::VIDEO_OK;

public:

	VideoPlayerAPI(Ogre::Vector4 * screensize = 0);

	virtual	~VideoPlayerAPI();

	int queue_video(const char * filename);

	int queue_video(const char * filename, double from, double to);

	int play(void);

	inline int pause(void);

	int next(void);

	int play(int vindex);

//	int play(int vindex, double from, double to);

	int update(double tslf);

	inline void set_repeat(bool v);

	inline void setRepeatAll(bool v);

	inline bool is_playing(void);

	inline void set_visible(bool visible);

	inline bool is_visible(void);

protected:


protected:

	VideoPlayer					*videoplayer;
	std::vector<Video>			playlist;
	int							actualvideo;
	bool						isplaying;
	bool						mRepeat;
	bool						mRepeatAll;

};



inline bool VideoPlayerAPI::is_playing(void){
	return isplaying;
}

inline void VideoPlayerAPI::set_repeat(bool v){
	mRepeat = v;
}

inline void VideoPlayerAPI::setRepeatAll(bool v){
	mRepeatAll = v;
}

inline int VideoPlayerAPI::pause(void){
	isplaying = false;
}

inline void VideoPlayerAPI::set_visible(bool visible){
	videoplayer->set_visible(visible);
}


inline bool VideoPlayerAPI::is_visible(void){
	return videoplayer->is_visible();

}

#endif //FFMPEG_VIDEO_PLAYER_API_H
