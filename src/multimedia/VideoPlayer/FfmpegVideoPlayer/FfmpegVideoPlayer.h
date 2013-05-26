#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <iostream>
#include <cstdio>
#include "DebugUtil.h"
#include <Ogre.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}



#if defined(_WIN32) || defined(CYGWIN)
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  include <AL/al.h>
#  include <AL/alc.h>
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif

namespace mm {

//FIXME#4
//#define NUM_BUFFERS 			3
//#define BUFFER_SIZE 			8192 //4096
//#define VIDEO_QUEUE_MAX_SIZE	50
//#define AUDIO_QUEUE_MAX_SIZE	50

class VideoPlayer
{


private:

	static const int NUM_BUFFERS = 3;		//for the openal sound player
	static const int BUFFER_SIZE = 8192;    //for the openal sound player

	static const int VIDEO_QUEUE_MAX_SIZE = 50;
	static const int AUDIO_QUEUE_MAX_SIZE = 50;

	static const int defaultwidth = 1024;	//for the screen texture
	static const int defaultheight = 768;   //for the screen texture


public:

	enum{
		VIDEO_ERROR = -10,
		VIDEO_WARNING,
		VIDEO_ENDED,
		VIDEO_NEED_DATA,
		VIDEO_OK = 0
	};

private:

	AVFormatContext 			*pFormatCtx;
	int             			videoStream;
	int 						audioStream;
	AVCodecContext  			*pCodecCtx;
	AVCodecContext 				*aCodecCtx;
	AVCodec         			*pCodec;
	AVCodec						*aCodec;
	AVFrame         			*pFrame;
	AVFrame         			*pFrameRGB;
	uint8_t         			*buffer;
	bool						isPlaying;
	bool						isLoaded;
    struct SwsContext 			*pImgConvertCtx;
    PixelFormat 				dst_pix_fmt;
    Ogre::Rectangle2D 			*mMiniScreen;
    Ogre::SceneNode 			*miniScreenNode;
    Ogre::TexturePtr 			rtt_texture;
    Ogre::MaterialPtr 			renderMaterial;
    double						mplayingtime;
    long int 					vtbasenum;
    long int 					vtbaseden;
    long int 					atbasenum;
    long int 					atbaseden;
    double						mVideoLength;
    std::queue<AVPacket*>		vDataQue;
    std::deque<AVPacket*> 		aDataDque;

    // For alplayer
	ALCdevice 					*dev;
	ALCcontext 					*ctx;
    ALuint 						source, buffers[NUM_BUFFERS];
    ALuint 						al_frequency;
    ALenum 						al_format;
    uint8_t 					al_buf[BUFFER_SIZE];

    // For decode_frame()
    AVFrame 					*audio_decoded_frame;
    AVPacket 					*audio_decoding_pkt;
    uint8_t						*decoded_frame_data_ptr;
    int 						decoded_data_size;
    int 						decoding_pkt_size;
    unsigned char				*decoding_pkt_data;

    //for timing and synchronization
    double						synchroPTS;
    double						buffLenInSec;
    double						fxcxb;  // audio frequency*channel*bits

    //
    int64_t						lastPts;
    // audio packet not valid for time stamp
    bool						apnvfts;

public:

					VideoPlayer(Ogre::Real left = -1.0f, Ogre::Real top = 1.0f,
							Ogre::Real right = 1.0f, Ogre::Real bottom = -1.0f);

	virtual 		~VideoPlayer();

	int 			load(const char *fileName);

	int 			unload(void);

	int 			update(double timesincelastframe);

	void		 	play(void);

	inline bool 	is_playing(void);

	inline bool		is_loaded(void);

	int				seek_time_stamp(double ts);

	inline int 		get_video_length(double & l);

	inline int		get_playing_time_in_secs(double & s);

	inline const char *	get_video_name(void);

	int				paint_screen(unsigned char R,
								 unsigned char G,
								 unsigned char B);

	inline int		paint_black_screen(void);

	inline int		paint_white_screen(void);

	inline void		set_visible(bool b);

	inline bool		is_visible(void);

protected:

	int 		create_al_audio_player(void);

	int 		get_more_data(void);

	int			update_video(double tslf);

	int			update_audio(double tslf);

	int 		audio_decode_frame(uint8_t **buffer, int buffer_size);

	int			get_audio_data(uint8_t *strm, int len);

	void 		print_video_info(void);

	int			set_screen_texture(void);

	int 		preload_audio(void);

	int 		get_playing_time(double & t);

	int 		empty_data_queues(void);


	// Useful as debug
	void 		SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);

};



inline bool VideoPlayer::is_playing(void)
{
	return isPlaying;
}

inline bool	VideoPlayer::is_loaded(void)
{
	return isLoaded;
}


inline int VideoPlayer::get_video_length(double & l){

	if(isLoaded){
		l = mVideoLength;
		return VIDEO_OK;
	}else{
		debugWARNING("Asked for length of video while no video is loaded\n");
		return VIDEO_WARNING;
	}
}


inline int VideoPlayer::get_playing_time_in_secs(double & s){

	if(isLoaded){
		return get_playing_time(s);
	}else{
		debugWARNING("Attempt to get playing time when no video loaded\n");
		return VIDEO_WARNING;
	}
}


//TODO Revisar esta mierda de codigo!!!!
inline const char *	VideoPlayer::get_video_name(void){

	if(isLoaded){
		return pFormatCtx->filename;
	}else{
		debugWARNING("Attempt to get video name when no video loaded\n");
		return "No video is beeing played";
	}
}


inline int VideoPlayer::paint_black_screen(void){
	return paint_screen(0,0,0);
}

inline int VideoPlayer::paint_white_screen(void){
	return paint_screen(255,255,255);
}

inline void VideoPlayer::set_visible(bool b){
	mMiniScreen->setVisible(b);
	miniScreenNode->setVisible(b);
}

inline bool VideoPlayer::is_visible(void){
	return mMiniScreen->isVisible();

}

}
#endif
