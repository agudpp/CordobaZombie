/*
 * Module: VideoPlayer
 * Version: 2.0
 * Wed 11/12/2013
 * Author: Raul
 */

//#############################################################################/

#include <inttypes.h>
#include <string.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

#include <openal_handler/OpenALHandler.h>

#include "VideoPlayer.h"


/*
 * NOTE: This is a fix to support old versions of ffmpeg. Looks like this macro
 * was undefined in version 2.1 ...
 */
#ifndef AVCODEC_MAX_AUDIO_FRAME_SIZE
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000
#endif

namespace mm {

///////////////////////////////////////////////////////////////////////////////
// DEFINITIONS AND GLOBAL CONSTANTS

#define SC(t,x) static_cast<t>(x)

const double EPS = 10e-12; // For double precision comparisons


///////////////////////////////////////////////////////////////////////////////
// DEBUGING COLOR PRINT DEFINITIONS
#ifdef DEBUG
#define lightGreenPrint(format, ...) \
						printf("\33[01;32m" format "\33[0m", ## __VA_ARGS__)
#define cyanPrint(format, ...) \
						printf("\33[22;36m" format "\33[0m", ## __VA_ARGS__)
#else
#define lightGreenPrint(format, ...)
#define cyanPrint(format, ...)
#endif

#ifdef VIDEO_PLAYER_DEBUG__
#define debugVIDEO(format, ...) \
    {fprintf(stderr, "\33[22;36m[%s, %s, %d]: ", \
     __FILE__, __FUNCTION__, __LINE__); \
     fprintf(stderr, format "\33[0m" , ## __VA_ARGS__);}

#else
#define debugVIDEO(format, ...)
#endif


///////////////////////////////////////////////////////////////////////////////
/*
 * TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
 *
 * 	1. Arreglar el tema de los pixel format para poder usar memcopy y que sea
 *  mas eficiente.
 *
 *  3. Leaks
 *
 *	10. Cambiar double por float que tiene hasta 7 decimales de presicion y es
 *		mas que suficiente y supongo trae menos problemas con las cuentas.
 *
 *	11. Liberar recursos de openal en unload y ~VideoPlayer. Mejorar el load
 *	para que no tenga que hacer todo de nuevo lo de openal sino que use lo que
 *	ya viene del ultimo video.
 *
 *	12. Reinicializar todas las variables (o al menos todas las necesarias)
 *	cada vez que se haga load, para evitar sorpresas como las que ya tuve :D.
 *
 *	15. Limpiar los buffers de sonido para que no haga ruido de basura al
 *	darle play a un nuevo video.
 *
 *	16. Buscar una mejor solucion para no usar viejos audio packets al hacer
 *	get_playing_time luego de un seek. Usar la variable apnvfts es muy McCaco.
 *
 *	17. Función para crear el alplayer una sola vez, funcion para tomar datos
 *	de audio packet. Usar estos datos para cargar correctamente el audio player.
 *	Asegurarse de que no estamos tirando mil threads :S.
 *
 *	18. Choose a much better and "scientific" way to decide the max size and
 *	critical size of the queues length.
 *
 *	19. mNumAPacks y mNumVPacks no deberian ser menor a 0 nunca !!!
 *
 * TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
 */

/*FIXME'S
 *
 * NOTA:
 * La linea correspondiente a cada FIXME se encuentra maracada con un FIXME#N
 * en el codigo, donde N es el numero correspondiente.
 *
 *
 * FIXME 1.
 * 	Si bien no necesita ser tan preciso este chequeo, notar que puede
 * 	haber problemas de presicion. Recordar la comparacion entre doubles
 * 	(usar un epsilon o delta). Quizas sea mas seguro castear todas las vars a
 * 	booleanos.
 *
 *
 * FIXME 2.
 *  Esto es cacaso, pero es la mejor forma que encontre de hacerlo. Creo que
 *  debe estar leakeando toneladas :S.
 *
 *
 * FIXME 3.
 * 	Quizas no se deba tomar como error un frame incompleto. Simeplemente se
 * 	omite y se espera al proximo update de video.
 *
 *
 *
 * FIXME 5.
 * 	Flush audio when seeking: por ahora no hay drama porque los buffers de
 * 	sonido son muy chicos, pero si se cambian va a ser un drama. Con esto se
 * 	desencolan los buffers buenos y se reemplazan por buffers nulos: (es la
 * 	forma que encontre de hacer flush):
 * 	alSourceStop(source);
 * 		//Openal needs to stop the source before flushing it's buffers
 *	alSourcei(source, AL_BUFFER, 0);
 *		//flush openal source from it's buffers
 *
 */

///////////////////////////////////////////////////////////////////////////////
VideoPlayer::VideoPlayer(VideoBuffer *screen) :
    pFormatCtx(0),
    pCodecCtx(0),
    aCodecCtx(0),
    pCodec(0),
    aCodec(0),
    pFrame(0),
    pFrameRGB(0),
    buffer(0),
    isPlaying(false),
    isLoaded(false),
    pImgConvertCtx(0),
    dst_pix_fmt(PIX_FMT_BGR24),
    mScreen(screen),
    mplayingtime(0.0f),
    vtbasenum(0),
    vtbaseden(0),
    atbasenum(0),
    atbaseden(0),
    mVideoLength(0.0f),
    source(0),
    al_frequency(0),
    al_format(0),
    audio_decoded_frame(0),
    audio_decoding_pkt(0),
    decoded_frame_data_ptr(0),
    decoded_data_size(0),
    decoding_pkt_size(0),
    decoding_pkt_data(0),
    synchroPTS(0),
    buffLenInSec(0),
    lastPts(0),
    apnvfts(0),
    mVDataQueue(std::deque<AVPacket*> (VIDEO_QUEUE_DEFAULT_SIZE)),
    mADataQueue(std::deque<AVPacket*> (AUDIO_QUEUE_DEFAULT_SIZE)),
    mNumVPacks(0),
    mNumAPacks(0),
    mALHandler(0)

{

#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
    debugGREEN("OGRE SAYS BIGENDIAN\n");
#else
    debugGREEN("OGRE SAYS LITTLENDIAN\n");
#endif
#if AV_HAVE_BIGENDIAN
    debugGREEN("AV SAYS BIGENDIAN\n");
#else
    debugGREEN("AV SAYS LITTLENDIAN\n");
#endif

    ASSERT(mScreen);

    // Point video and audio data queue to the avpacket list
    for (int i = 0; i < VIDEO_QUEUE_DEFAULT_SIZE; i++) {
        mVDataQueue[i] = new AVPacket;
    }
    for (int i = 0; i < AUDIO_QUEUE_DEFAULT_SIZE; i++) {
        mADataQueue[i] = new AVPacket;
    }

    // Register all formats and codecs for video
    av_register_all();

    // remove all the logs, so they don't appear in the console
    av_log_set_level( AV_LOG_QUIET);

}

///////////////////////////////////////////////////////////////////////////////

VideoPlayer::~VideoPlayer()
{
    //close and free some stuff
    if (isLoaded) {
        unload();
    }

    // Free frames
    if (pFrame) {
        av_free(pFrame);
        pFrame = 0;
    }

    // Free conversion context
    if (pImgConvertCtx) {
        sws_freeContext(pImgConvertCtx);
        pImgConvertCtx = 0;
    }

    // Free the codec contexts (should be closed to free them)
    if (pCodecCtx) {
        avcodec_close(pCodecCtx);
        pCodecCtx = 0;
    }


    // Free and close audio devices, buffers and packets
    if (audio_decoded_frame) {
        av_free(audio_decoded_frame);
        audio_decoded_frame = 0;
    }
    if (aCodecCtx) {
        avcodec_close(aCodecCtx);
        aCodecCtx = 0;
    }

    // In case there is a packet in audio_decoding_pkt
    if (audio_decoding_pkt) {
        av_free_packet(audio_decoding_pkt);
        delete audio_decoding_pkt;
        audio_decoding_pkt = 0;
    }


    // delete AVPackets:
    for(int i = 0; i < mVDataQueue.size();i++){
        delete mVDataQueue[i];
    }

    for(int i = 0; i < mADataQueue.size();i++){
        delete mADataQueue[i];
    }


}
//#############################################################################

//#############################################################################
///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::load(const char *fileName)
{

    if (isLoaded) {
        debugERROR("Tried to load video when video player already loaded.\n");
        return VIDEO_ERROR;
    }

    // Allocation (if first time)
    pFormatCtx = avformat_alloc_context();

    // Allocate video frame
    if (!pFrame) {
        pFrame = avcodec_alloc_frame();
    }
    if (!pFrame) {
        debugERROR("Could not allocate video frame\n");
        return VIDEO_ERROR;
    }

    // Open video file
    if (avformat_open_input(&pFormatCtx, fileName, NULL, 0)) {
        debugERROR("Couldn't open file %s\n", fileName);
        return VIDEO_ERROR;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(pFormatCtx, 0) < 0) {
        debugERROR("Couldn't find stream information\n");
        return VIDEO_ERROR;
    }

    // Dump information about file into standard error
    av_dump_format(pFormatCtx, 0, fileName, false);

    // Find the first video and the first audio stream
    videoStream = -1;
    audioStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams && (videoStream == -1
        || audioStream == -1); i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO
            && videoStream < 0) {
            videoStream = i;
        }
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO
            && audioStream < 0) {
            audioStream = i;
        }
    }
    if (videoStream == -1) {
        debugERROR("Didn't find a video stream\n");
        return VIDEO_ERROR;
    } else {
        debugGREEN("Video Stream Number %d\n", videoStream);
    }

    // Get a pointer to the codec context for the video stream
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (!pCodec) {
        debugERROR("Video codec not found\n");
        return VIDEO_ERROR;
    }

    // Inform the codec that we can handle truncated bitstreams -- i.e.,
    // bitstreams where frame boundaries can fall in the middle of packets
    if (pCodec->capabilities & CODEC_CAP_TRUNCATED)
        pCodecCtx->flags |= CODEC_FLAG_TRUNCATED;

    // Open codec
    if (avcodec_open2(pCodecCtx, pCodec, 0) < 0) {
        debugERROR("Could not open video codec");
        return VIDEO_ERROR;
    }

    // Allocate an AVFrame structure for converted frame
    pFrameRGB = avcodec_alloc_frame();
    if (pFrameRGB == NULL) {
        debugERROR("Could not alloc RGBframe\n");
        return VIDEO_ERROR;
    }

    // Determine required buffer size and allocate buffer
    int numBytes = avpicture_get_size(dst_pix_fmt,
                                      pCodecCtx->width,
                                      pCodecCtx->height);
    buffer = new uint8_t[numBytes];

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    avpicture_fill((AVPicture *) pFrameRGB,
                   buffer,
                   dst_pix_fmt,
                   pCodecCtx->width,
                   pCodecCtx->height);

    // Configure convertion context
    int w = pCodecCtx->width;
    int h = pCodecCtx->height;

    //Allocate the conversion context
    pImgConvertCtx = sws_getCachedContext(pImgConvertCtx,
                                          w,
                                          h,
                                          pCodecCtx->pix_fmt,
                                          w,
                                          h,
                                          dst_pix_fmt,
                                          SWS_BICUBIC,
                                          NULL,
                                          NULL,
                                          NULL);

    if (!pImgConvertCtx) {
        debugERROR("Cannot initialize the conversion context!\n");
        return VIDEO_ERROR;
    }

    //set some values
    vtbasenum = pFormatCtx->streams[videoStream]->time_base.num;
    vtbaseden = pFormatCtx->streams[videoStream]->time_base.den;

    mVideoLength = static_cast<double> (pFormatCtx->duration)
        / static_cast<double> (AV_TIME_BASE);

    isPlaying = false;
    isLoaded = true;
    mplayingtime = 0.;

    //Set things for audio:
    if (audioStream == -1) {

        debugGREEN("Didn't find an audio stream. Playing only video.\n");

    } else if(!mALHandler){

        debugWARNING("You are loading a video with sound stream, but you "
            "haven't set the OpenALHandler for this video player, so "
            "you won't be able to listen anything.\n");
        // Make sure we don't play any audio
        audioStream = -1;

    }else{
        debugGREEN("Audio Stream Number %d\n", audioStream);

        // Allocate audio frame
        if (!audio_decoded_frame) {
            audio_decoded_frame = avcodec_alloc_frame();
        }

        if (audio_decoded_frame == 0) {
            debugERROR("Could not allocate Audio frame\n");
            return VIDEO_ERROR;
        }

        aCodecCtx = pFormatCtx->streams[audioStream]->codec;

        aCodec = avcodec_find_decoder(aCodecCtx->codec_id);
        if (aCodec == NULL) {
            debugERROR("Audio codec not found\n");
            return VIDEO_ERROR;
        }

        if (avcodec_open2(aCodecCtx, aCodec, 0) < 0) {
            debugERROR("Could not open audio codec");
            return VIDEO_ERROR;
        }

        atbasenum = pFormatCtx->streams[audioStream]->time_base.num;
        atbaseden = pFormatCtx->streams[audioStream]->time_base.den;

        if (aCodecCtx->sample_fmt != AV_SAMPLE_FMT_S16) {
            debugERROR("Unsupported audio format :s\n");
            return VIDEO_ERROR;
        } else {
            get_al_audio_player();
        }

        ASSERT(mNumAPacks == 0);
        ASSERT(mNumVPacks == 0);

//        if (preload_audio() == VIDEO_ERROR) {
//            debugERROR("Couldn't pre-load audio :s\n");
//            return VIDEO_ERROR;
//        }

    }


    //print_video_info();

    return VIDEO_OK;
}

///////////////////////////////////////////////////////////////////////////////

void
VideoPlayer::play(void)
{
    if (isLoaded) {
        if (isPlaying) {
            debugWARNING("Warning: attempting to play video while already "
                "playing\n");
        } else {

            //fetch some packets before start
            while (get_more_data(true, true) != VIDEO_ENDED && (mNumVPacks
                < VIDEO_QUEUE_CRITICAL_LOW_SIZE || (mNumAPacks
                < AUDIO_QUEUE_CRITICAL_LOW_SIZE && audioStream != -1))) {
            }

            mplayingtime = 0.;
            isPlaying = true;
        }
    } else {
        debugWARNING("Warning: attempting to play video while not loaded\n");
    }
}

///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::unload(void)
{
    if (!isLoaded) {
        debugWARNING("Attempt to unload when nothing was loaded\n");
        return VIDEO_OK;
    }

    isPlaying = false;

    // Free the RGB image
    delete[] buffer;
    av_free(pFrameRGB);

    // Close the codecsvideoStream (but don't free them)
    avcodec_close(pCodecCtx);

    // Clear the packet queues
    empty_data_queues();


    // Clean audio buffers:

    // remove the source first
    if (alGetError() != AL_NO_ERROR) {
        debugERROR("We already have an error when trying to delete"
            " the video openal stuff?\n");
    }

    // delete the sources
    if (alIsSource(source)) {
        alDeleteSources(1, &source);
        source = 0;
    }

    // delete the allocated buffers, checking one by one
    for (unsigned int i = 0; i < NUM_BUFFERS; ++i) {
        if (alIsBuffer(buffers[i])) {
            // delete this one
            alDeleteBuffers(1, &(buffers[i]));
        }
    }

    if (alGetError() != AL_NO_ERROR) {
        debugERROR("Some error occur when deleting the openal sources or buffers "
            "in the video player\n");
    }


    // In case we where decoding something.
    if (audio_decoding_pkt) {
        av_free_packet(audio_decoding_pkt);
        audio_decoding_pkt = 0;
    }
    decoded_data_size = 0;
    decoding_pkt_data = 0;
    decoding_pkt_size = 0;

    // destroy contexts and devices from the audio player
    if (audioStream != -1) {
        avcodec_close(aCodecCtx);
    }

    // Close the video file
    avformat_close_input(&pFormatCtx);
    isLoaded = false;

    return VIDEO_OK;
}

///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::seek_time_stamp(double ts)
{

    // Out of range?
    if (ts + EPS < 0 || ts > mVideoLength + EPS) {
        debugERROR("Tried to seek out of range\n");
        return VIDEO_ERROR;
    }

    // Need to seek forward or backward?
    double t = static_cast<double> (lastPts)
        / static_cast<double> (AV_TIME_BASE);
    int flag = ts < t ? AVSEEK_FLAG_BACKWARD : 0;

    // Get the target time
    int stream_index = -1;
    int64_t seek_target = SC(int64_t,(ts*AV_TIME_BASE));

    // Choose an available stream to seek
    if (videoStream >= 0) {
        stream_index = videoStream;
    } else if (audioStream >= 0) {
        stream_index = audioStream;
    }

    // Change target time base if stream_index requires it
    if (stream_index >= 0) {
        seek_target
            = av_rescale_q(seek_target,
                           AV_TIME_BASE_Q,
                           pFormatCtx->streams[stream_index]->time_base);
    }

    // Seek now
    debugGREEN("Seeking for %ld(%lf in seconds) in %s\n", seek_target, ts,
               pFormatCtx->filename);
    if (av_seek_frame(pFormatCtx, stream_index, seek_target, flag) < 0) {
        // Didn't finid it forward/backward try the other way
        flag = !flag ? AVSEEK_FLAG_BACKWARD : 0;
        if (av_seek_frame(pFormatCtx, stream_index, seek_target, flag) < 0) {
            debugERROR("Error while seeking in %s target %ld\n",
                pFormatCtx->filename, seek_target);
            return VIDEO_ERROR;

        }
    }

    mplayingtime = ts;

    //clear queues to prepare for new data
    empty_data_queues();

    if (audio_decoding_pkt) {
        av_free_packet(audio_decoding_pkt);
        audio_decoding_pkt = 0;
    }
    decoded_data_size = 0;
    decoding_pkt_data = 0;
    decoding_pkt_size = 0;

    apnvfts = true;

    return VIDEO_OK;
}

///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::empty_data_queues(void)
{
//TODO watch out for deletes that don't correspond

    //TODO check if this is correct
    while (mNumAPacks) {
        av_free_packet(mADataQueue.front());
        mADataQueue.push_back(mADataQueue.front());
        mADataQueue.pop_front();
        mNumAPacks--;
    }

    //TODO check if this is correct
    while (mNumVPacks) {
        av_free_packet(mVDataQueue.front());
        mVDataQueue.push_back(mVDataQueue.front());
        mVDataQueue.pop_front();
        mNumVPacks--;
    }

    ASSERT(mNumAPacks == 0);
    ASSERT(mNumVPacks == 0);

    return VIDEO_OK;
}

///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::get_al_audio_player(void)
{

    if (!mALHandler->hasDevice()) {
        ASSERT(false && "No device was set for the OpenALHandler.\n");
        return VIDEO_ERROR;
    }

    if (!mALHandler->hasContext()) {
        ASSERT(false && "No context was set for the OpenALHandler.\n");
        return VIDEO_ERROR;
    }

    // Generate buffers and source
    alGenBuffers(NUM_BUFFERS, buffers);

    if(!source){
        alGenSources(1, &source);
    }
    if (alGetError() != AL_NO_ERROR) {
        debugERROR("Problem with me and openal.\n");
        return VIDEO_ERROR;
    }

    // silence
    alListenerf(AL_GAIN, 0.0f);
    alSourcef(source, AL_GAIN, 0.0f);

    // rewind and clear queue
    alSourceRewind(source);
    alSourcei(source, AL_BUFFER, 0);

    // Load buffer with silence queue them into the source and wait till
    // it ends playing them.
    unsigned char *buf[BUFFER_SIZE];
    memset(buf, 0, BUFFER_SIZE);
    al_frequency = aCodecCtx->sample_rate;
    al_format = AL_FORMAT_STEREO16;

    for (int i = 0; i < NUM_BUFFERS; i++) {
        alBufferData(buffers[i], al_format, buf, BUFFER_SIZE, al_frequency);
    }
    alSourceQueueBuffers(source, NUM_BUFFERS, buffers);
    alSourcePlay(source);
    if (alGetError() != AL_NO_ERROR) {
        debugRED("Algo pasa con el reproductor de openal y no inicia\n");
        return VIDEO_ERROR;
    }

    // Reading silenced buffers
    int val = 0;
    do {
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &val);
    } while (val < NUM_BUFFERS);


    //get size of buffers in seconds
    ALint sizeInBytes = 0;
    ALint channels = 0;
    ALint bits = 0;

    alGetBufferi(buffers[0], AL_SIZE, &sizeInBytes);
    alGetBufferi(buffers[0], AL_CHANNELS, &channels);
    alGetBufferi(buffers[0], AL_BITS, &bits);

    fxcxb = (static_cast<double> (channels) * static_cast<double> (bits)
        * static_cast<double> (al_frequency));

    buffLenInSec = (static_cast<double> (sizeInBytes) * 8.0f) / fxcxb;
//    debugGREEN("Buffers size in seconds: %lf %lf %lf %lf %lf\n",
//    static_cast<double>(sizeInBytes), static_cast<double>(channels),
//    static_cast<double>(bits), static_cast<double>(al_frequency),
//    buffLenInSec);

    // Sound up
    alListenerf(AL_GAIN, 1.0f);
    alSourcef(source, AL_GAIN, 1.0f);

    return VIDEO_OK;
}

///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::preload_audio(void)
{
    ALuint buffer = 0;
    ALint val = 0;
    int g_audio_ret = 0;

    // check if buffers need to be filled
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &val);
    if (val <= 0) {

        return VIDEO_OK;

    } else {

        //get data for each buffer that is ready
        for (int i = 0; i < val; i++) {

            g_audio_ret = get_audio_data(al_buf, BUFFER_SIZE);

            if (g_audio_ret == VIDEO_OK) {

                alSourceUnqueueBuffers(source, 1, &buffer);
                alBufferData(buffer,
                             al_format,
                             al_buf,
                             BUFFER_SIZE,
                             al_frequency);
                alSourceQueueBuffers(source, 1, &buffer);
                if (alGetError() != AL_NO_ERROR) {
                    debugERROR("Error buffering :(\n");
                    return VIDEO_ERROR;
                }

            } else if (g_audio_ret == VIDEO_ENDED) {

                alGetSourcei(source, AL_SOURCE_STATE, &val);
                if (val != AL_PLAYING) {
                    // No more info and al_player finished reading his buffers.
                    debugVIDEO("Audio says that the video ended.\n");
                    return VIDEO_ENDED;
                }
                // No more useful info coming from get_audio_for_al (only
                // silence).
                break;

            } else if (g_audio_ret == VIDEO_ERROR) {

                return VIDEO_ERROR;

            } else {

                ASSERT(false);
            }
        }
    }

    return VIDEO_OK;
}

///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::update(double timesincelastframe)
{

    int video_ret = VIDEO_OK, audio_ret = VIDEO_OK;

    // Don't do nothing if its not playing
    if (!isPlaying) {
        return VIDEO_OK;
    }

    mplayingtime += timesincelastframe;

    // if one of the queues is running out of packets
    if (mNumVPacks <= VIDEO_QUEUE_CRITICAL_LOW_SIZE || (mNumAPacks
        <= AUDIO_QUEUE_CRITICAL_LOW_SIZE && audioStream != -1)) {
        get_more_data(true, true);
    }

    //if we have audio
    if (audioStream != -1) {
        audio_ret = update_audio();
    } else {
        audio_ret = VIDEO_ENDED;
    }

    video_ret = update_video();

    if (audio_ret == VIDEO_ERROR or video_ret == VIDEO_ERROR) {
        debugERROR("Video error\n");
        return VIDEO_ERROR;
    }

    // FIXME if ((audioStream == -1 || audio_ret == VIDEO_ENDED) && video_ret
    if (audio_ret == VIDEO_ENDED  && video_ret == VIDEO_ENDED) {
        ASSERT(!mNumAPacks);
        ASSERT(!mNumVPacks);
        debugGREEN("Video of length %lf ended at time %lf\n",
            mVideoLength, mplayingtime);
        if(abs(mVideoLength-mplayingtime)<0.1){
            debugERROR("Ending too soon or too late? ended=%lf, "
                "should have been=%lf:S\n", mplayingtime, mVideoLength);
        }
        isPlaying = false;
        return VIDEO_ENDED;
    }

    return VIDEO_OK;
}

///////////////////////////////////////////////////////////////////////////////


int
VideoPlayer::update_video(void)
{

    if (!mNumVPacks) {
        //if need data
        if (get_more_data(false, true) == VIDEO_ENDED) {
            if (!mNumVPacks) {
                // if couldn't get more data
                return VIDEO_ENDED;
            }
        }
    }

//    debugGREEN("Number of video packets: %i, of audio: %i\n", mNumVPacks, mNumAPacks);

    ASSERT(mNumVPacks);
    AVPacket * pkt = mVDataQueue.front();

    ASSERT( &(*pkt) == mVDataQueue.front());
    ASSERT(pkt->stream_index == videoStream && \
           "Se mezclaron los packetes en la cola :S\n");
    ASSERT(pkt);
    ASSERT(pkt->data);

    double t = 0.;
    if (VIDEO_ERROR == get_playing_time(t)) {
        // we don't have the time, then we make sure to present the frame
        t = SC(double,pkt->pts) * SC(double,vtbasenum);
    }

    if (SC(double,pkt->pts) * SC(double,vtbasenum) <=
        t * SC(double,vtbaseden)) { //FIXME#1

        int frameFinished = 0;
        avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, pkt);

        if (!frameFinished) { //FIXME#3
            debugVIDEO("Incomplete video frame\n");
            return VIDEO_OK;
        }

        // Pop the packet and free it
        av_free_packet(pkt);
        mVDataQueue.pop_front();
        mVDataQueue.push_back(pkt); // TODO check if this works as it should
        pkt = 0; //being careful
        mNumVPacks--;

        //convert frame to rgb image
        sws_scale(pImgConvertCtx,
                  pFrame->data,
                  pFrame->linesize,
                  0,
                  pCodecCtx->height,
                  pFrameRGB->data,
                  pFrameRGB->linesize);

        //Actualizar la textura de la pantalla
        unsigned char * img = pFrameRGB->data[0];

        mScreen->fillBuffer(img,
                            VideoBuffer::Format::RGB,
                            pCodecCtx->width * pCodecCtx->height * 3);
    }

    return VIDEO_OK;
}


///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::update_audio(void)
{

    ALuint buffer = 0;
    ALint val = 0;
    int g_audio_ret = 0;

    // check if buffers need to be filled
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &val);
    if (val > 0) {

        //get data for each buffer that is ready
        for (int i = 0; i < val; i++) {

            g_audio_ret = get_audio_data(al_buf, BUFFER_SIZE);

            if (g_audio_ret == VIDEO_OK) {

                alSourceUnqueueBuffers(source, 1, &buffer);
                alBufferData(buffer,
                             al_format,
                             al_buf,
                             BUFFER_SIZE,
                             al_frequency);
                alSourceQueueBuffers(source, 1, &buffer);
                if (alGetError() != AL_NO_ERROR) {
                    debugERROR("Error buffering :(\n");
                    return VIDEO_ERROR;
                }

            } else if (g_audio_ret == VIDEO_ENDED) {

                alGetSourcei(source, AL_SOURCE_STATE, &val);
                if (val != AL_PLAYING) {
                    // No more info and al_player finished reading his buffers.
                    debugVIDEO("AUDIO SAYS: VIDEO ENDED\n");
                    return VIDEO_ENDED;
                }
                // No more useful info coming from get_audio_for_al (only
                // silence).
                break;

            } else if (g_audio_ret == VIDEO_ERROR) {

                return VIDEO_ERROR;

            } else {

                ASSERT(false);
            }
        }
    }

    // In case the player has stopped.
    alGetSourcei(source, AL_SOURCE_STATE, &val);
    if (val != AL_PLAYING) {
        debugVIDEO("OpenAl player had stopped\n");
        alSourcePlay(source);
    }

    return VIDEO_OK;
}

///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::get_audio_data(uint8_t *strm, int len)
{

    ASSERT(strm != 0);

    int d_frame_res = 0;
    uint8_t *buf = 0;
    int len2retrive = len;

    while (len2retrive > 0) {

        // decode audio data
        d_frame_res = audio_decode_frame(&buf, len2retrive);

        if (d_frame_res > 0) {
            // got some data
            ASSERT(buf != 0);
            memcpy(strm, buf, d_frame_res);
            strm += d_frame_res;
            len2retrive -= d_frame_res;

        } else if (d_frame_res == VIDEO_NEED_DATA) {

            //need Packets
            if (get_more_data(true, false) == VIDEO_ENDED && mNumAPacks == 0) {

                // fill rest of the buffer with silence
                memset(strm, 0, len2retrive);

                if (len2retrive == len) {
                    // No useful info is being return (only silence).
                    return VIDEO_ENDED;
                } else {
                    // Partly useful data is being returned, possibly
                    // followed by silence.
                    return VIDEO_OK;
                }
            }

        } else {

            // On error send silence
            debugERROR("Error while getting data for alplayer\n");
            memset(strm, 0, len2retrive);
            return VIDEO_ERROR;

        }
    }

    return VIDEO_OK;
}

///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::audio_decode_frame(uint8_t **buffer, int buffer_size)
{

    ASSERT(buffer_size > 0);

    // FIXME: justificar por qué uso un paquete auxiliar y ver si es correcto
    // hacerlo.
    AVPacket auxpack;
    av_init_packet(&auxpack);

    while (1) { // Packet dequeuing loop

        while (1) { // Frame decoding loop

            if (!audio_decoding_pkt) {
                // Need a packet to decode
                break;
            }

            // Si ya envie toda la data de la ultima frame busco uno nuevo.
            if (decoded_data_size <= 0) {

                ASSERT(audio_decoding_pkt);
                ASSERT(audio_decoded_frame);

                if (decoding_pkt_size <= 0) {
                    // Get new Packet
                    break;
                }

                auxpack.data = decoding_pkt_data;
                auxpack.size = decoding_pkt_size;

                int decode_error = 0, got_frame = 0;
                decode_error = avcodec_decode_audio4(aCodecCtx,
                                                     audio_decoded_frame,
                                                     &got_frame,
                                                     &auxpack);

                if (decode_error < 0) {
                    debugGREEN("Error decoding audio frame. "
                        "Skiping to next frame\n");
                    decoding_pkt_data += auxpack.size;
                    decoding_pkt_size -= auxpack.size;
                    continue;
                }

                if (got_frame) {

                    // got frame
                    decoded_data_size
                        = av_samples_get_buffer_size(NULL,
                                             aCodecCtx->channels,
                                             audio_decoded_frame->nb_samples,
                                             aCodecCtx->sample_fmt,
                                             1);

                    if (decoded_data_size < 0) {
                        debugERROR("bad size %d\n", decoded_data_size);
                        return VIDEO_ERROR;
                    }

                    ASSERT(audio_decoded_frame->data[0]);
                    decoded_frame_data_ptr = audio_decoded_frame->data[0];
                }
                decoding_pkt_data += decode_error;
                decoding_pkt_size -= decode_error;
                continue;
            }

            //si todavia tengo datos de un frame viejo o acabo de conseguir
            //un nuevo frame entonces los mando.

            ASSERT(decoded_data_size > 0);
            ASSERT(decoded_frame_data_ptr != 0)

            //Apunto buffer al inicio de los datos decodificados
            *buffer = decoded_frame_data_ptr;

            if (decoded_data_size > buffer_size) {

                // Decodifique mas de lo que se necesitaba
                decoded_data_size -= buffer_size;
                decoded_frame_data_ptr += buffer_size;
                return buffer_size;

            } else {

                //Quizas no tengo tanta info como quiero todavia. Asi que el de
                //afuerda debera copiar estos datos en algun lado y llamarnos
                //para pedir mas.
                int ret = decoded_data_size;
                decoded_data_size = 0;
                return ret;
            }
        }

        ASSERT(decoded_data_size == 0);

        // Free old packet before getting new one, we don't have one the
        // first time, thats the reason for the if clause.
        if (audio_decoding_pkt) {
            // FIXME#2
            //ASSERT(audio_decoding_pkt->data);
            av_free_packet(audio_decoding_pkt);
            audio_decoding_pkt = 0; // being careful
            mADataQueue.push_back(mADataQueue.front());
            mADataQueue.pop_front();
            mNumAPacks--;
            ASSERT(mNumAPacks>=0);
        }

        if (mNumAPacks > 0) {

            apnvfts = false;

            audio_decoding_pkt = mADataQueue.front();

            ASSERT(audio_decoding_pkt->stream_index == audioStream && \
                   "Se mezclaron los packetes en la cola :S\n");

            //ASSERT(audio_decoding_pkt && "Aca la cagamos antes");
            //ASSERT(audio_decoding_pkt->data && "Aca la cagamos");
            if (audio_decoding_pkt->pts != AV_NOPTS_VALUE) {
                synchroPTS = static_cast<double> (audio_decoding_pkt->pts);
                //debugGREEN("synchroPTS %lf audio... %ld\n", synchroPTS,
                //            audio_decoding_pkt->pts);
            }
            // save for restoring later
            decoding_pkt_size = audio_decoding_pkt->size;
            decoding_pkt_data = audio_decoding_pkt->data;
            // take out of the queue
        } else {
            // can't get packets because queue is empty
            return VIDEO_NEED_DATA;

        }
    }

    // should never come out here
    ASSERT(false);
}

///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::get_playing_time(double & t)
{

    if (!audio_decoding_pkt || apnvfts || audioStream == -1) {
        // we can't get the synchronized time so we return the real time
        t = mplayingtime;
        return VIDEO_OK;
    }

    ALint val = 0;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &val);
    int nloadedbuffs = NUM_BUFFERS - val;

    ASSERT(static_cast<double>(aCodecCtx->sample_rate) > 0);

    // offset in buffers (sound loaded in buffers but already played)
    ALfloat samples = 0;
    alGetSourcef(source, AL_SAMPLE_OFFSET, &samples);
    //debugRED("samples %f\n", samples);
    double offset = static_cast<double> (samples)
        / static_cast<double> (aCodecCtx->sample_rate);

    //debugRED("Hz %lf\n",static_cast<double>(aCodecCtx->sample_rate));
    //debugRED("Offset %lf\n",offset);

    ASSERT(fxcxb>0);
    ASSERT(audio_decoding_pkt);
    //size in seconds of data loaded from last packet
    double lastpckloadedsize = ((audio_decoding_pkt->size - decoding_pkt_size)
        * 8.0f) / fxcxb;

    //debugRED("lastpckloadedsize %lf\n",lastpckloadedsize );

    ASSERT(SC(double,atbaseden)>0);
    //debugRED("synchroPTS %lf, %lf, %lf, %lf\n",
    //synchroPTS*(SC(double,atbasenum) / SC(double,atbaseden)),synchroPTS,
    //SC(double,atbasenum),SC(double,atbaseden));
    t = synchroPTS * (SC(double,atbasenum) / SC(double,atbaseden))
        - ((SC(double,nloadedbuffs) - 1.0f) * buffLenInSec) + offset
        + lastpckloadedsize;

    return VIDEO_OK;
}

///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::get_more_data(bool audio, bool video)
{

    bool got_audio = !audio || audioStream == -1;
    bool got_video = !video;

    while (!got_audio || !got_video) {

        AVPacket* newPacket = 0;
        bool alloced = false;

        //start supposing it's a video packet
        ASSERT(mNumVPacks <= mVDataQueue.size());
        if (mNumVPacks >= mVDataQueue.size()) {
            // if the queue is full we need a new packet
            newPacket = new AVPacket;
            alloced = true;
            debugERROR("HORRIBLE\n");
        } else {
            newPacket = mVDataQueue[mNumVPacks];
        }

        av_init_packet(newPacket);

        if (av_read_frame(pFormatCtx, newPacket) != 0) {
            // No chequeo errores aca :s Simplemente supongo que termino de
            // desempaquetar.
            return VIDEO_ENDED;
        } else {
            ASSERT(newPacket->data);
            lastPts = newPacket->pts;

            if (newPacket->stream_index == videoStream) {
                // it was a video packet as supposed :D
                if (alloced) {
                    mVDataQueue.push_back(newPacket);
                }
                newPacket = 0; // just to be careful
                got_video = true;
                mNumVPacks++;

            } else if (newPacket->stream_index == audioStream) {
                // it was an audio packet, not what we supposed
                //                debugGREEN("%i --- %i\n",mNumAPacks, SC(int,mADataQueue.size()));
                ASSERT(mNumAPacks <= SC(int,mADataQueue.size()));
                // exchange packets between queues
                if (mNumAPacks >= mADataQueue.size()) {
                    mADataQueue.push_back(newPacket);
                    if (!alloced) {
                        mVDataQueue[mNumVPacks] = new AVPacket;
                    }
                } else {
                    // need to save old packet from the audio queue before
                    // it's replaced.
                    if (alloced) {
                        av_init_packet(mADataQueue[mNumAPacks]);
                        mVDataQueue.push_back(mADataQueue[mNumAPacks]);
                    } else {
                        av_init_packet(mADataQueue[mNumAPacks]);
                        mVDataQueue[mNumVPacks] = mADataQueue[mNumAPacks];
                    }
                    mADataQueue[mNumAPacks] = newPacket;
                }

                newPacket = 0;
                got_audio = true;
                mNumAPacks++;

            } else {

                debugVIDEO("Not my stream :s\n");
                av_free_packet(newPacket);

            }
        }
    }

    //    debugGREEN("video: %i, audio: %i\n",mNumVPacks,mNumAPacks);
    //    ASSERT(mADataQueue[mNumAPacks]->data != 0 && "va a causar double free");
    //    ASSERT(mVDataQueue[mNumVPacks]->data != 0 && "va a causar double free");

    return VIDEO_OK;
}

///////////////////////////////////////////////////////////////////////////////

void
VideoPlayer::print_video_info(void)
{

    cyanPrint("VIDEO DEBUG\n");
    lightGreenPrint("Video nb of streams %d\n", pFormatCtx->nb_streams);
    if (audioStream != -1) {
        cyanPrint("AUDIO DEBUG\n");
        const char *format = av_get_sample_fmt_name(aCodecCtx->sample_fmt);
        if (format) {
            lightGreenPrint("Audio sample format %s\n",format );
        }
        lightGreenPrint("Audio nb of channels %d\n",aCodecCtx->channels);
        lightGreenPrint("Audio nb of bits per sample %d\n",
            av_get_bytes_per_sample(aCodecCtx->sample_fmt)*8);
        lightGreenPrint("Audio frequency %dHz\n",aCodecCtx->sample_rate);
    }
    cyanPrint("OTHER DEBUGS\n");
    lightGreenPrint("AVCODEC_MAX_AUDIO_FRAME_SIZE: %d\n",
        AVCODEC_MAX_AUDIO_FRAME_SIZE); cyanPrint("***********\n");

}

///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::paint_screen(unsigned char R, unsigned char G, unsigned char B)
{
    ASSERT(mScreen);
    unsigned char img[pCodecCtx->width * pCodecCtx->height * 3];
    for (int i = 0; i + 2 < pCodecCtx->width * pCodecCtx->height * 3; i += 3) {
        img[i] = R;
        img[i + 1] = G;
        img[i + 2] = B;
    }
    mScreen->fillBuffer(img,
                        VideoBuffer::Format::RGB,
                        pCodecCtx->width * pCodecCtx->height * 3);

    return VIDEO_OK;
}


///////////////////////////////////////////////////////////////////////////////

int
VideoPlayer::paint_black_screen(void)
{
    ASSERT(mScreen);
    int size = 0;
    if(pCodecCtx){
        size =  pCodecCtx->width * pCodecCtx->height * 3;
    }else{
        size = DEFAULT_SCREEN_HEIGHT*DEFAULT_SCREEN_WIDTH*3;
    }
    unsigned char img[size];

    memset(img, 0, size);
    mScreen->fillBuffer(img,
                        VideoBuffer::Format::RGB,
                        size);

    return VIDEO_OK;
}


/*----------------------------------------------------------------------------*/
/*----------------------------- EXTRA FUNCTIONS ------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * @VideoPlayer::SaveFrame
 * Saves an RGB24 (RGBRGBRGB...) (one byte for R, one for G and one for B) to
 * the hard drive in ppm format. We only use it for debug issues.
 */
void
VideoPlayer::SaveFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
    FILE *pFile;
    char szFilename[32];
    int y;

    // Open file
    sprintf(szFilename, "./Frames/frame%d.ppm", iFrame);
    pFile = fopen(szFilename, "wb");
    if (pFile == NULL)
        return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for (y = 0; y < height; y++)
        fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, width * 3, pFile);

    // Close file
    fclose(pFile);
}

} //namespace mm
