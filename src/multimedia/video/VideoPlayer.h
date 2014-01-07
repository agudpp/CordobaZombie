/*
 * Module: VideoPlayer
 * Version: 2.0
 * Wed 11/12/2013
 * Author: Raul
 */

////////////////////////////////////////////////////////////////////////////////

#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H


// c++ stdlib includes
#include <iostream>
#include <cstdio>
#include <queue>
//-----------------------------------------------------------------------------


// FFMPEG includes
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}
//-----------------------------------------------------------------------------

// OPEN_AL includes
#if defined(_WIN32) || defined(CYGWIN) || defined(MINGW)
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  include <AL/al.h>
#  include <AL/alc.h>
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif
//-----------------------------------------------------------------------------


// Project includes

#include <openal_handler/OpenALHandler.h>
#include <debug/DebugUtil.h>
#include "VideoBuffer.h"
//-----------------------------------------------------------------------------



////////////////////////////////////////////////////////////////////////////////

namespace mm {

class VideoPlayer
{

private:

    static const int NUM_BUFFERS = 3; //for the openal sound player
    static const int BUFFER_SIZE = 8192; //for the openal sound player

    static const int VIDEO_QUEUE_DEFAULT_SIZE = 150;
    static const int AUDIO_QUEUE_DEFAULT_SIZE = 150;

    static const int VIDEO_QUEUE_CRITICAL_HIGH_SIZE = 250;
    static const int AUDIO_QUEUE_CRITICAL_HIGH_SIZE = 250;

    static const int VIDEO_QUEUE_CRITICAL_LOW_SIZE = 10;
    static const int AUDIO_QUEUE_CRITICAL_LOW_SIZE = 30;

    static const int DEFAULT_SCREEN_WIDTH = 1024; //for the screen
    static const int DEFAULT_SCREEN_HEIGHT = 768; //for the screen


public:

    enum
    {
        VIDEO_ERROR = -10,
        VIDEO_WARNING,
        VIDEO_ENDED,
        VIDEO_NEED_DATA,
        VIDEO_OK = 0
    };

private:

    AVFormatContext *pFormatCtx;
    int videoStream;
    int audioStream;
    AVCodecContext *pCodecCtx;
    AVCodecContext *aCodecCtx;
    AVCodec *pCodec;
    AVCodec *aCodec;
    AVFrame *pFrame;
    AVFrame *pFrameRGB;
    uint8_t *buffer;
    bool isPlaying;
    bool isLoaded;
    struct SwsContext *pImgConvertCtx;
    PixelFormat dst_pix_fmt;
    VideoBuffer *mScreen;
    double mplayingtime;
    long int vtbasenum;
    long int vtbaseden;
    long int atbasenum;
    long int atbaseden;
    double mVideoLength;
    std::deque<AVPacket*> mVDataQueue;  // queue of ptr's to the video packets.
    std::deque<AVPacket*> mADataQueue;  // queue of ptr's to the audio packets.
    int mNumVPacks;
    int mNumAPacks;

    // For alplayer
    const OpenALHandler *mALHandler;
    ALuint source, buffers[NUM_BUFFERS];
    ALuint al_frequency;
    ALenum al_format;
    uint8_t al_buf[BUFFER_SIZE];

    // For decode_frame()
    AVFrame *audio_decoded_frame;
    AVPacket *audio_decoding_pkt;
    uint8_t *decoded_frame_data_ptr;
    int decoded_data_size;
    int decoding_pkt_size;
    unsigned char *decoding_pkt_data;

    //for timing and synchronization
    double synchroPTS;
    double buffLenInSec;
    double fxcxb; // audio frequency*channel*bits

    //
    int64_t lastPts;
    // audio packet not valid for time stamp
    bool apnvfts;

public:

    /*
     * Video player constructor.
     */
    VideoPlayer(VideoBuffer *screen);


    /*
     * Video player destroyer
     */
    virtual
    ~VideoPlayer(void);


    /*
     * Select video 'fileName' to load into the player for decoding.
     */
    int
    load(const char *fileName);


    /*
     * Unload the video player, clean and free its buffers.
     */
    int
    unload(void);


    /*
     * Check if there is a video loaded into the video player.
     */
    inline bool
    is_loaded(void);


    /*
     * Start playing the loaded video.
     */
    void
    play(void);


    /*
     * Check if the video player is playing something.
     */
    inline bool
    is_playing(void);


    /**
     * @VideoPlayer::update
     *		Call this every new frame for updating the video.
     *
     * @Return:
     * 		VIDEO_ERROR if the video ends at least one second before its
     * 		supposed to end, meaning that the file is broken. Else VIDEO_OK.
     * 		TODO: one second is too much REPAIR!!!
     */
    int
    update(double timesincelastframe);


    /*
     * Seek to frame at time stamp 'ts' or as close to it as possible.
     * @warning: packet queues will be cleared on success.
     */
    int
    seek_time_stamp(double ts);


    /*
     *
     */
    inline int
    get_video_length(double & l);


    /*
     *
     */
    inline int
    get_playing_time_in_secs(double & s);


    /*
     *
     */
    inline const char *
    get_video_name(void);


    /*
     *
     */
    int
    paint_screen(unsigned char R, unsigned char G, unsigned char B);


    /*
     * Get height and width of the loaded video in pixels.
     */
    inline int
    getSizes(unsigned int &h, unsigned int &w);


    /*
     * Paint it black.
     */
    int
    paint_black_screen(void);



    /*
     *
     */
    inline void
    setOpenALHandler(const OpenALHandler *oalh);


protected:

    /*
     * Usamos OpenAL para reproducir el sonido de nuestro video. Esta funcion
     * crea el dispositivo de sonido y realiza las configuraciones iniciales.
     *
     */
    int
    get_al_audio_player(void);

    /*
     * 	If audio queue size is < AUDIO_QUEUE_DEFAULT_SIZE or video queue size
     * 	is < VIDEO_QUEUE_DEFAULT_SIZE, it gets at least one audio packet and one
     * 	video frame unless no more packets can be fetched from the file.
     *
     * @return
     * 		On success returns VIDEO_OK. If can't fetch more packets, then it
     * 		returns VIDEO_ENDED. On error returns VIDEO_ERROR.
     */
    int
    get_more_data(bool audio, bool video);


    /*
     * Update the video (the screen where the video is being played) if
     * its time to do it. Gets packets from the video queue decodes them and
     * copies the frame into the screen.
     *
     * @return
     * 		VIDEO_OK if everything goes right.
     * 		VIDEO_ERROR if something goes wrong.
     * 		VIDEO_ENDED if can't get more packets to decode frames and present
     * 		them.
     */
    int
    update_video(void);

    /*
     * Update the video player audio. It checks if al player needs data in its
     * buffers and fills them up.
     *
     * @return
     * 		On success it returns VIDEO_OK. If an error occurs while filling the
     * 		buffers, it returns VIDEO_ERROR. If can't get any more info to fill
     * 		in the buffers and the al player has already red all its buffers
     * 		then it returns VIDEO_ENDED.
     */
    int
    update_audio(void);

    /*
     *
     *	Decodifica frames desde los paquetes que obtiene de la cola de paquetes
     *	de audio. Busca obtener hasta buffer_size bytes de datos de audio. Al
     *	salir apunta *buffer a memoria con datos para ser copiados desde afuera.
     *	La cantidad de bytes de datos reales descomprimidos son devueltos como
     *	salida. Solo se garantiza que esa cantidad de datos son validos para
     *	ser copiados desde buffer.
     *
     *	@return
     *		On success return number of bytes decoded an pointed by buffer.
     *		If can't fetch packets from aDataDque then it returns
     *		VIDEO_NEED_DATA.
     *		On error returns VIDEO_ERROR.
     *
     *  @param buffer
     *  	Buffer pointer to be pointed to the decoded data.
     *
     *  @param buffer_size
     *      Max size of the decoded data to be returned (must be > 0).
     */
    int
    audio_decode_frame(uint8_t **buffer, int buffer_size);

    /*
     *	Ensures to fill stream with 'len' bytes of audio data.
     *
     *	@return
     *		On error returns VIDEO_ERROR.
     *		If can't get useful data for strm, return VIDEO_ENDED.
     *		Else it returns VIDEO_OK
     *	@param
     *		strm:
     *		stream to fill up with data (must be len bytes size or more)
     *	@param
     *		len:
     *		number of data bytes to be copied to strm
     */
    int
    get_audio_data(uint8_t *strm, int len);

    void
    print_video_info(void);

    /*
     * Preload buffer to avoid problems with synchronization. Does the same
     * thing as audio_update but doesn't start to play the audio.
     */
    int
    preload_audio(void);


    /*
     * From the audio track (can be different from the video time, when
     * synchronization is not working right).
     */
    int
    get_playing_time(double & t);


    /*
     *
     */
    int
    empty_data_queues(void);

public: // Extra methods

    /*
     * Useful as debug
     */
    void
    SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);

};


///////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
inline bool
VideoPlayer::is_playing(void)
{
    return isPlaying;
}


//-----------------------------------------------------------------------------
inline bool
VideoPlayer::is_loaded(void)
{
    return isLoaded;
}


//-----------------------------------------------------------------------------
inline int
VideoPlayer::get_video_length(double & l)
{

    if (isLoaded) {
        l = mVideoLength;
        return VIDEO_OK;
    } else {
        debugWARNING("Asked for length of video while no video is loaded\n");
        return VIDEO_WARNING;
    }
}


//-----------------------------------------------------------------------------
inline int
VideoPlayer::get_playing_time_in_secs(double & s)
{

    if (isLoaded) {
        return get_playing_time(s);
    } else {
        debugWARNING("Attempt to get playing time when no video loaded\n");
        return VIDEO_WARNING;
    }
}


//-----------------------------------------------------------------------------
//TODO Revisar esta mierda de codigo!!!!
inline const char *
VideoPlayer::get_video_name(void)
{

    if (isLoaded) {
        return pFormatCtx->filename;
    } else {
        debugWARNING("Attempt to get video name when no video loaded\n");
        return "No video is being played";
    }
}


//-----------------------------------------------------------------------------
inline int
VideoPlayer::getSizes(unsigned int &h, unsigned int &w)
{
    if (isLoaded) {
        h = pCodecCtx->height;
        w = pCodecCtx->width;
        return VIDEO_OK;
    } else {
        debugERROR("Can't get sizes as no video is loaded.\n");
        return VIDEO_ERROR;
    }
}

//-----------------------------------------------------------------------------

inline void
VideoPlayer::setOpenALHandler(const OpenALHandler *oalh)
{
    ASSERT(oalh);
    mALHandler = oalh;
}



} //namespace mm

#endif //VIDEO_PLAYER_H
