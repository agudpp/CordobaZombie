#include "FfmpegVideoPlayer.h"
#include "GlobalObjects.h"
#include "inttypes.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

#define SC(t,x) static_cast<t>(x)
const double EPS = 10e-12;



// COLOR PRINT DEFINITIONS
#define lightGreenPrint(format, ...) \
						printf("\33[01;32m" format "\33[0m", ## __VA_ARGS__)
#define cyanPrint(format, ...) \
						printf("\33[22;36m" format "\33[0m", ## __VA_ARGS__)
// END COLOR PRINT DEFINITIONS


/*
 * TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
 *
 * 	1. Arreglar el tema de los pixel format para poder usar memcopy y que sea
 *  mas eficiente.
 *
 *  2. Mostrar la pantalla del video al hacer play y sacarla al hacer stop.
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
 *	13. Ver que pasa si se termina de reproducir la lista y pongo repeat y pido
 *	seguir reproduciendo.
 *
 *	14. Quitar los parametros tslf de todas los metodos que no los necesitan
 *
 *	15. Limpiar los buffers de sonido para que no haga ruido de basura al
 *	darle play a un nuevo video.
 *
 *	16. Buscar una mejor solucion para no usar viejos audio packets al hacer
 *	get_playing_time luego de un seek. Usar la variable apnvfts es muy McCaco.
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
 * FIXME 4.
 * 	Hacer defines asi al aire puede ser peligroso porque se pueden
 *	sobreescribir otros o no? que puedo hacer para evitarlo.
 *
 * FIXME 5.
 * 	Flush audio when seeking: por ahora no hay drama porque los buffers de
 * 	sonido son muy chicos, pero si se cambian va a ser un drama. Con esto se
 * 	desencolan los buffers buenos y se reemplazan por buffers nulos: (es la
 * 	forma que encontre de hacer flush):
 * 	alSourceStop(source); //Openal needs to stop the source before flushing it's buffers
 *	alSourcei(source, AL_BUFFER, 0); //flush openal source from it's buffers
 *
 */

////////////////////////////////////////////////////////////////////////////////

VideoPlayer::VideoPlayer(Ogre::Real left, Ogre::Real top,
						 Ogre::Real right, Ogre::Real bottom):
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
	mMiniScreen(0),
	miniScreenNode(0),
	rtt_texture(0),
	renderMaterial(0),
	mplayingtime(0.0f),
	vtbasenum(0),
	vtbaseden(0),
	atbasenum(0),
	atbaseden(0),
	mVideoLength(0.0f),
	dev(0),
	ctx(0),
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
	apnvfts(0)
{

#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
    debugRED("OGRE SAYS BIGENDIAN\n");
#else
    debugRED("OGRE SAYS LITTLENDIAN\n");
#endif
#if AV_HAVE_BIGENDIAN
    debugRED("AV SAYS BIGENDIAN\n");
#else
    debugRED("AV SAYS LITTLENDIAN\n");
#endif

    // The screen
    mMiniScreen = new Ogre::Rectangle2D(true);
    mMiniScreen->setCorners(left, top, right, bottom);
    mMiniScreen->setBoundingBox(Ogre::AxisAlignedBox(
    		-100000.0f * Ogre::Vector3::UNIT_SCALE,
    		100000.0f * Ogre::Vector3::UNIT_SCALE));

    ASSERT(GLOBAL_SCN_MNGR);
	miniScreenNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

//	GLOBAL_SCN_MNGR->getRootSceneNode()->setVisible(true, true);

    miniScreenNode->attachObject(mMiniScreen);

    // Material for the screen

    static Ogre::MaterialPtr rendmat =
    		Ogre::MaterialManager::getSingleton().create("RttMat",
    		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    renderMaterial = rendmat;
    ASSERT(renderMaterial.get());

    Ogre::Technique* matTechnique = renderMaterial->createTechnique();
    matTechnique->createPass();
    renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(true);

    //A temporary texture for the screen before something is loaded
    static Ogre::TexturePtr rtttex =
    		Ogre::TextureManager::getSingleton().createManual("RttTex",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D, defaultwidth,
			defaultheight, 0,
			Ogre::PF_X8R8G8B8, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

    rtt_texture = rtttex;
    ASSERT(rtt_texture.get());

    //set material texture
	renderMaterial->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(
    		"RttTex");
    //set screen material
    mMiniScreen->setMaterial("RttMat");

    // Register all formats and codecs
    av_register_all();

    // remove all the logs
    av_log_set_level(AV_LOG_QUIET);


}


////////////////////////////////////////////////////////////////////////////////

VideoPlayer::~VideoPlayer(){
	//close and free some stuff
	if(isLoaded){ unload(); }

    // Free frames
	if(pFrame){	av_free(pFrame); pFrame = 0; }

    // Free conversion context
	if(pImgConvertCtx){	sws_freeContext(pImgConvertCtx); pImgConvertCtx = 0; }

    // Free the codec contexts (should be closed to free them)
	if(pCodecCtx){ avcodec_close(pCodecCtx); pCodecCtx = 0;	}

    if(audioStream != -1){
    	if(audio_decoded_frame){
    		av_free(audio_decoded_frame); audio_decoded_frame = 0;
    	}
    	if(aCodecCtx){ avcodec_close(aCodecCtx); aCodecCtx = 0;	}

		// In case there is a packet in audio_decoding_pkt
		if(audio_decoding_pkt){
			av_free_packet(audio_decoding_pkt);
			delete audio_decoding_pkt;
			audio_decoding_pkt = 0;
		}

    }
    //
    if(mMiniScreen){ delete mMiniScreen; mMiniScreen = 0; }
}


////////////////////////////////////////////////////////////////////////////////

void VideoPlayer::play(void)
{
	if(isLoaded){
		if(isPlaying){
			debugWARNING("Warning: attempting to play video while already "
					"playing\n");
		}else{

			//fetch some packets before start
			while(get_more_data() != VIDEO_ENDED and
					vDataQue.size() < VIDEO_QUEUE_MAX_SIZE and
					aDataDque.size() < AUDIO_QUEUE_MAX_SIZE)
			{
			}

			mplayingtime = 0;
			isPlaying = true;
		}
	}else{
		debugWARNING("Warning: attempting to play video while not loaded\n");
	}
}


////////////////////////////////////////////////////////////////////////////////

int VideoPlayer::load(const char *fileName){

	if(isLoaded){
		debugERROR("Tried to load video when video player already loaded.\n");
		return VIDEO_ERROR;
	}

    // Allocation (if first time)
    pFormatCtx = avformat_alloc_context();

    // Allocate video frame
    if(!pFrame){
    	pFrame=avcodec_alloc_frame();
    }
    if(pFrame==0){
    	debugERROR("Could not alloc video frame\n"); return VIDEO_ERROR;
    }

    // Open video file
    if(avformat_open_input(&pFormatCtx, fileName, NULL, 0)!=0){
    	debugERROR("Couldn't open file %s\n", fileName);
        return VIDEO_ERROR;
    }

    // Retrieve stream information
    if(avformat_find_stream_info( pFormatCtx, 0)<0){
    	debugERROR("Couldn't find stream information\n");
    	return VIDEO_ERROR;
    }

    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, fileName, false);

    // Find the first video and the first audio stream
    videoStream = -1;
    audioStream = -1;
    for(int i=0; i<pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO &&
        		videoStream < 0)
        {
            videoStream=i;
        }
    	if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO &&
    			audioStream < 0)
    	{
    		audioStream=i;
    	}
	}
    if(videoStream==-1){
    	debugERROR("Didn't find a video stream\n"); return VIDEO_ERROR;
    }else{
    	debugGREEN("Video Stream Number %d\n", videoStream);
    }

    // Get a pointer to the codec context for the video stream
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL){
    	debugERROR("Video codec not found\n"); return VIDEO_ERROR;
    }

    // Inform the codec that we can handle truncated bitstreams -- i.e.,
    // bitstreams where frame boundaries can fall in the middle of packets
    if(pCodec->capabilities & CODEC_CAP_TRUNCATED)
        pCodecCtx->flags|=CODEC_FLAG_TRUNCATED;

    // Open codec
    if(avcodec_open2(pCodecCtx, pCodec, 0)<0){
    	debugERROR("Could not open video codec"); return VIDEO_ERROR;
    }

    // Allocate an AVFrame structure for converted frame
    pFrameRGB=avcodec_alloc_frame();
    if(pFrameRGB==NULL){
    	debugERROR("Could not alloc RGBframe\n"); return VIDEO_ERROR;
    }
    // Determine required buffer size and allocate buffer
    int numBytes=avpicture_get_size(dst_pix_fmt, pCodecCtx->width,
        pCodecCtx->height);
    buffer=new uint8_t[numBytes];
    // Assign appropriate parts of buffer to image planes in pFrameRGB
    avpicture_fill((AVPicture *)pFrameRGB, buffer, dst_pix_fmt,
        pCodecCtx->width, pCodecCtx->height);

    // Configure convertion context
    int w = pCodecCtx->width;
    int h = pCodecCtx->height;

    //Allocate the conversion context
    pImgConvertCtx = sws_getCachedContext(	pImgConvertCtx,w, h,
											pCodecCtx->pix_fmt,
											w, h, dst_pix_fmt, SWS_BICUBIC,
											NULL, NULL, NULL);

    if(pImgConvertCtx == NULL) {
        debugERROR("Cannot initialize the conversion context!\n");
        return VIDEO_ERROR;
    }


    set_screen_texture();

    //set some values
	vtbasenum = pFormatCtx->streams[videoStream]->time_base.num;
	vtbaseden = pFormatCtx->streams[videoStream]->time_base.den;

	mVideoLength = static_cast<double>(pFormatCtx->duration)/
				   static_cast<double>(AV_TIME_BASE);

	isPlaying = false;
    isLoaded = true;
    mplayingtime = 0;


    //For audio:
    if(audioStream==-1){

    	debugGREEN("Didn't find an audio stream. Playing only video.\n");

    }else{
    	debugGREEN("Audio Stream Number %d\n", audioStream);

        // Allocate audio frame
        if(!audio_decoded_frame){
        	audio_decoded_frame = avcodec_alloc_frame();
        }

        if(audio_decoded_frame==0){
        	debugERROR("Could not alloc Audio frame\n"); return VIDEO_ERROR;
        }

    	aCodecCtx = pFormatCtx->streams[audioStream]->codec;

    	aCodec=avcodec_find_decoder(aCodecCtx->codec_id);
		if(aCodec==NULL){
			debugERROR("Audio codec not found\n"); return VIDEO_ERROR;
		}

	    if(avcodec_open2(aCodecCtx, aCodec, 0)<0){
	        debugERROR("Could not open audio codec"); return VIDEO_ERROR;
	    }

		atbasenum = pFormatCtx->streams[audioStream]->time_base.num;
		atbaseden = pFormatCtx->streams[audioStream]->time_base.den;

	    if(aCodecCtx->sample_fmt != AV_SAMPLE_FMT_S16){
	    	debugERROR("Unsuported audio format :s\n");
	    	return VIDEO_ERROR;
	    }else{
	    	create_al_audio_player();
	    }

	    if(preload_audio() == VIDEO_ERROR){
	    	debugERROR("Couldn't pre-load audio :s\n");
	    	return VIDEO_ERROR;
	    }

    }

    //print_video_info();

    return VIDEO_OK;
}


////////////////////////////////////////////////////////////////////////////////

int VideoPlayer::unload(void){
	if(!isLoaded){
		debugWARNING("Attempt to unload when nothing was loaded\n");
		return VIDEO_OK;
	}

	isPlaying = false;

    // Free the RGB image
    delete [] buffer;
    av_free(pFrameRGB);

    // Close the codecsvideoStream (but not free them)
    avcodec_close(pCodecCtx);

    // Clear the packet queues
    empty_data_queues();

    // destroy contexts and devices from the audio player
    if(audioStream != -1){
		avcodec_close(aCodecCtx);
		alcCloseDevice(dev);
		alcDestroyContext(ctx);
		alcCloseDevice(dev);
    }

    // Close the video file
    avformat_close_input(&pFormatCtx);
    isLoaded = false;

}



/**
 * Seek to frame at time stamp 'ts' or as near to it as possible.
 *
 * @warning: packet queues will be cleared on success.
 */

int VideoPlayer::seek_time_stamp(double ts){

	if(ts+EPS < 0 || ts > mVideoLength+EPS){
		debugERROR("Tried to seek out of range\n");
		return VIDEO_ERROR;
	}

	double t = static_cast<double>(lastPts)/static_cast<double>(AV_TIME_BASE);
	int flag = ts < t ? AVSEEK_FLAG_BACKWARD : 0;

	int stream_index= -1;
	int64_t seek_target = SC(int64_t,(ts*AV_TIME_BASE));

	if(videoStream >= 0){
		stream_index = videoStream;
	}else if(audioStream >= 0){
		stream_index = audioStream;
	}

	if(stream_index >= 0){
		seek_target = av_rescale_q(seek_target, AV_TIME_BASE_Q,
	                  	  pFormatCtx->streams[stream_index]->time_base);
	}

//	debugGREEN("Seeking in %s target %ld\n",
//					pFormatCtx->filename, seek_target);
	if(av_seek_frame(pFormatCtx, stream_index, seek_target, flag) < 0) {
		flag = 0 ? AVSEEK_FLAG_BACKWARD : 0;
		if(av_seek_frame(pFormatCtx, stream_index, seek_target, flag) < 0){
			debugERROR("Error while seeking in %s target %ld\n",
				pFormatCtx->filename, seek_target);
			return VIDEO_ERROR;

		}
	}

	mplayingtime = ts;

	//clear queues to prepare for new data
	empty_data_queues();

	apnvfts = true;

	return VIDEO_OK;
}



int VideoPlayer::empty_data_queues(void){

	while(!aDataDque.empty()){
		av_free_packet(aDataDque.front());
		delete aDataDque.front();
		aDataDque.pop_front();
	}

	while(!vDataQue.empty()){
		av_free_packet(vDataQue.front());
		delete vDataQue.front();
		vDataQue.pop();
	}

	return VIDEO_OK;
}





/*
 * Usamos OpenAL para reproducir el sonido de nuestro video. Esta funcion crea
 * el dispositivo de sonido y realiza las configuraciones iniciales.
 */
int VideoPlayer::create_al_audio_player(void){

     al_frequency = aCodecCtx->sample_rate;
     al_format = AL_FORMAT_STEREO16 ;

	 dev = alcOpenDevice(NULL);
	 if(!dev)
	 {
		 debugERROR("");
		 return VIDEO_ERROR;
	 }

	 ctx = alcCreateContext(dev, NULL);
	 alcMakeContextCurrent(ctx);
	 if(!ctx)
	 {
		 debugERROR("");
		 return VIDEO_ERROR;
	 }
	 alListenerf(AL_GAIN, 1.0f);


     alGenBuffers(NUM_BUFFERS, buffers);
     alGenSources(1, &source);
     if(alGetError() != AL_NO_ERROR)
     {
    	 debugERROR("");
    	 return VIDEO_ERROR;
     }
	 alSourcef(source, AL_GAIN, 1.0f);

     // Load buffer with silence queue them into the source and wait till
     // it ends playing them.
     unsigned char *buf;
     buf = (unsigned char *)calloc(1,BUFFER_SIZE);
     for(int i = 0; i < NUM_BUFFERS; i++){
    	 alBufferData(buffers[i], al_format, buf, BUFFER_SIZE, al_frequency);
     }
     alSourceQueueBuffers(source, NUM_BUFFERS, buffers);
     alSourcePlay(source);
     if(alGetError() != AL_NO_ERROR)
     {
    	 debugRED("Algo pasa con el reproductor de openal y no inicia\n");
    	 return VIDEO_ERROR;
     }
     int val = 0;

     // Reading silenced buffers
     do{
    	 alGetSourcei(source, AL_BUFFERS_PROCESSED, &val);
     }while(val < NUM_BUFFERS);

     // Free memory
     free(buf);

     //get size of buffers in seconds
     ALint sizeInBytes;
     ALint channels;
     ALint bits;

     alGetBufferi(buffers[0], AL_SIZE, &sizeInBytes);
     alGetBufferi(buffers[0], AL_CHANNELS, &channels);
     alGetBufferi(buffers[0], AL_BITS, &bits);

     fxcxb = (static_cast<double>(channels) * static_cast<double>(bits) *
			 static_cast<double>(al_frequency));

     buffLenInSec = (static_cast<double>(sizeInBytes) * 8.0f) / fxcxb;
     //debugRAUL("Buffers size in seconds: %lf %lf %lf %lf %lf\n", static_cast<double>(sizeInBytes), static_cast<double>(channels), static_cast<double>(bits), static_cast<double>(al_frequency), buffLenInSec);


     return VIDEO_OK;
}


/**
 * Preload buffer to avoid problems with synchronization. Does the same thing
 * as audio_update but doesn't start to play the audio.
 */

int VideoPlayer::preload_audio(void){
	ALuint buffer = 0;
    ALint val = 0;
	int g_audio_ret = 0;

	// check if buffers need to be filled
	alGetSourcei(source, AL_BUFFERS_PROCESSED, &val);
	if(val <= 0){

		return VIDEO_OK;

	}else{

		//get data for each buffer that is ready
		for(int i = 0; i < val; i++){

			g_audio_ret = get_audio_data(al_buf, BUFFER_SIZE);

			if(g_audio_ret == VIDEO_OK){

				alSourceUnqueueBuffers(source, 1, &buffer);
				alBufferData(buffer,al_format,al_buf,BUFFER_SIZE,al_frequency);
				alSourceQueueBuffers(source, 1, &buffer);
				if(alGetError() != AL_NO_ERROR)
				{
					debugERROR("Error buffering :(\n");
					return VIDEO_ERROR;
				}

			}else if(g_audio_ret == VIDEO_ENDED){

				alGetSourcei(source, AL_SOURCE_STATE, &val);
				if(val != AL_PLAYING){
					// No more info and al_player finished reading his buffers.
					debugGREEN("AUDIO SAYS: VIDEO ENDED\n");
					return VIDEO_ENDED;
				}
				// No more useful info coming from get_audio_for_al (only
				// silence).
				break;

			}else if(g_audio_ret == VIDEO_ERROR){

				return VIDEO_ERROR;

			}else{

				ASSERT(false);
			}
		}
	}

	return VIDEO_OK;
}


/**
 * @VideoPlayer::update
 *		Call this every new frame for updating the video.
 *
 * @Return:
 * 		VIDEO_ERROR if the video ends at least one second before its
 * 		supposed to end, meaning that the file is broken. Else VIDEO_OK.
 */

int VideoPlayer::update(double timesincelastframe){

	int video_ret = VIDEO_OK, audio_ret = VIDEO_OK;

	// Don't do nothing if its not playing
	if(!isPlaying){
		return VIDEO_OK;
	}

	mplayingtime += timesincelastframe;

	// if nether of the queues are full we get new packets
	if(	vDataQue.size() < VIDEO_QUEUE_MAX_SIZE and
		aDataDque.size() < AUDIO_QUEUE_MAX_SIZE)
	{
		get_more_data();
	}

	//if we have audio
	if(audioStream != -1){
		audio_ret = update_audio(timesincelastframe);
	}else{
		audio_ret = VIDEO_ENDED;
	}

	video_ret = update_video(timesincelastframe);

	if(audio_ret == VIDEO_ERROR or video_ret == VIDEO_ERROR)
	{
		debugERROR("Video error\n");
		return VIDEO_ERROR;
	}

	if( (audioStream == -1 || audio_ret == VIDEO_ENDED)  &&
	    video_ret == VIDEO_ENDED )
	{
		ASSERT(vDataQue.empty());
		ASSERT(aDataDque.empty());
		debugGREEN("Video of length %lf ended at time %lf\n",
					mVideoLength, mplayingtime);
		isPlaying = false;
		return VIDEO_ENDED;
	}

	return VIDEO_OK;
}


////////////////////////////////////////////////////////////////////////////////

/**
 * Update the video texture (the screen where the video is being played) if
 * its time to do it. Gets packets from the video queue decodes them and copies
 * the frame into the video texture.
 *
 * @return
 * 		VIDEO_OK if everything goes right.
 * 		VIDEO_ERROR if something goes wrong.
 * 		VIDEO_ENDED if can't get more packets to decode frames and present them.
 */
int VideoPlayer::update_video(double tslf){ // FIXME al dope el tslf

	if(vDataQue.empty()){
		//if need data
		if(get_more_data() == VIDEO_ENDED){
			if(vDataQue.empty()){
				// if couldn't get more data
				return VIDEO_ENDED;
			}
		}
	}

	ASSERT(!vDataQue.empty());

	AVPacket * pkt  = vDataQue.front();

	double t = 0;
	if(VIDEO_ERROR == get_playing_time(t)){
		// we don't have a the time, then we make sure to present the frame
		t = SC(double,pkt->pts) * SC(double,vtbasenum);
	}

	if(SC(double,pkt->pts) * SC(double,vtbasenum) <= t * SC(double,vtbaseden)){    //FIXME#1

		int frameFinished = 0;
		avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, pkt);

		if(!frameFinished){ //FIXME#3
			debugRED("Incomplete video frame\n");
			return VIDEO_OK;
		}

		// Pop the packet and free it
		vDataQue.pop();
		av_free_packet(pkt);
		delete pkt;

		//convert frame to rgb image
		sws_scale(pImgConvertCtx,
				  pFrame->data,
				  pFrame->linesize, 0,
				  pCodecCtx->height,
				  pFrameRGB->data,
				  pFrameRGB->linesize);

		//Actualizar la textura de la pantalla
		Ogre::HardwarePixelBufferSharedPtr PixelBuffer =
			rtt_texture->getBuffer();

		PixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

		unsigned char * pDest =
			static_cast<unsigned char*>(PixelBuffer->getCurrentLock().data);

		unsigned char * img = pFrameRGB->data[0];

//		#pragma omp parallel for
		for (size_t i = 0; i < pCodecCtx->width*pCodecCtx->height*4; i+=4)
		{
			pDest[i] 		= img[0];
			pDest[i + 1] 	= img[0 + 1];
			pDest[i + 2]    = img[0 + 2];
			img += 3;
		}

		//SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, 1);

		PixelBuffer->unlock();
	}

	return VIDEO_OK;
}


////////////////////////////////////////////////////////////////////////////////

/**
 * Update the video player audio. It checks if al player needs data in its
 * buffers and fills them up.
 *
 * @return
 * 		On success it returns VIDEO_OK. If an error occurs while filling the
 * 		buffers, it returns VIDEO_ERROR. If can't get any more info to fill in
 * 		the buffers and the al player has already red all its buffers then it
 * 		returns VIDEO_ENDED.
 */

int VideoPlayer::update_audio(double tslf){ // FIXME tslf esta al dope

	ALuint buffer = 0;
    ALint val = 0;
	int g_audio_ret = 0;

	// check if buffers need to be filled
	alGetSourcei(source, AL_BUFFERS_PROCESSED, &val);
	if(val > 0){

		//get data for each buffer that is ready
		for(int i = 0; i < val; i++){

			g_audio_ret = get_audio_data(al_buf, BUFFER_SIZE);

			if(g_audio_ret == VIDEO_OK){

				alSourceUnqueueBuffers(source, 1, &buffer);
				alBufferData(buffer,al_format,al_buf,BUFFER_SIZE,al_frequency);
				alSourceQueueBuffers(source, 1, &buffer);
				if(alGetError() != AL_NO_ERROR)
				{
					debugERROR("Error buffering :(\n");
					return VIDEO_ERROR;
				}

			}else if(g_audio_ret == VIDEO_ENDED){

				alGetSourcei(source, AL_SOURCE_STATE, &val);
				if(val != AL_PLAYING){
					// No more info and al_player finished reading his buffers.
					debugGREEN("AUDIO SAYS: VIDEO ENDED\n");
					return VIDEO_ENDED;
				}
				// No more useful info coming from get_audio_for_al (only
				// silence).
				break;

			}else if(g_audio_ret == VIDEO_ERROR){

				return VIDEO_ERROR;

			}else{

				ASSERT(false);
			}
		}
	}

	// In case the player has stopped.
	alGetSourcei(source, AL_SOURCE_STATE, &val);
	if(val != AL_PLAYING){
		debugRED("OpenAl player had stopped\n");
		alSourcePlay(source);
	}

	return VIDEO_OK;
}


////////////////////////////////////////////////////////////////////////////////

/**
 *	When called assures to fill stream with len bytes of audio data.
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

int VideoPlayer::get_audio_data(uint8_t *strm, int len){

	ASSERT(strm != 0);

	int d_frame_res = 0;
	uint8_t	*buf = 0;
	int len2retrive = len;

	while(len2retrive > 0){

		// decode audio data
		d_frame_res = audio_decode_frame(&buf, len2retrive);

		if(d_frame_res > 0){
			// got some data
			ASSERT(buf != 0);
			memcpy(strm, buf, d_frame_res);
			strm += d_frame_res;
			len2retrive -= d_frame_res;

		}else if(d_frame_res == VIDEO_NEED_DATA){

			//need Packets
			if(get_more_data() == VIDEO_ENDED && aDataDque.size() == 0){

				// fill rest of the buffer with silence
				memset(strm, 0, len2retrive);

				if(len2retrive == len){
					// No useful info is being return (only silence).
					return VIDEO_ENDED;
				}else{
					// Partly useful data is being returned, possibly
					// followed by silence.
					return VIDEO_OK;
				}
			}

		}else{

			// On error send silence
			debugERROR("Error while getting data for alplayer\n");
			memset(strm, 0, len2retrive);
			return VIDEO_ERROR;

		}
	}

	return VIDEO_OK;
}






/*
 *
 *	Decodifica frames desde los paquetes que obtiene de la cola de paquetes de
 *	audio. Busca obtener hasta buffer_size bytes de datos de audio. Al salir
 *	apunta *buffer a memoria con datos para ser copiados desde afuera. La
 *	cantidad de bytes de datos reales descomprimidos son devueltos como salida.
 *	Solo se garantiza que esa cantidad de datos son validos para ser copiados
 *	desde *buffer.
 *
 *	@return
 *		On success return number of bytes decoded an pointed by buffer.
 *		If can't fetch packets from aDataDque then it returns VIDEO_NEED_DATA.
 *		On error returns VIDEO_ERROR.
 *
 *  @param buffer
 *  	Buffer pointer to be pointed to the decoded data.
 *
 *  @param buffer_size
 *      Max size of the decoded data to be returned (must be > 0).
 */

int VideoPlayer::audio_decode_frame(uint8_t **buffer, int buffer_size){

	ASSERT(buffer_size > 0);

	AVPacket auxpack;
	av_init_packet(&auxpack);

	while(1){ // Packet dequeuing loop
		while(1){ // Frame decoding loop

			if(!audio_decoding_pkt){
				// Need a packet to decode
				break;
			}

			// Si ya envie toda la data de la ultima frame busco uno nuevo.
			if(decoded_data_size <= 0){

				ASSERT(audio_decoding_pkt);
				ASSERT(audio_decoded_frame);

				if(decoding_pkt_size <= 0){
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

				if (decode_error < 0){
					debugRED("Error decoding audio frame. "
							 "Skiping to next frame\n");
					continue;
				}

				if(got_frame){

					// got frame
					decoded_data_size = av_samples_get_buffer_size(NULL,
												aCodecCtx->channels,
												audio_decoded_frame->nb_samples,
	                                            aCodecCtx->sample_fmt, 1);

					if(decoded_data_size < 0){
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

			if(decoded_data_size > buffer_size){

				// Decodifique mas de lo que se necesitaba
				decoded_data_size -= buffer_size;
				decoded_frame_data_ptr += buffer_size;
				return buffer_size;

			}else{

				//Quizas no tengo tanta info como quiero todavia. Asi que el de
				//afuerda debera copiar estos datos en algun lado y llamarnos
				//para pedir mas.
				int ret = decoded_data_size;
				decoded_data_size = 0;
				return ret;
			}
		}

		ASSERT(decoded_data_size == 0);

		// Free old packet before getting new one
		if(audio_decoding_pkt){
			// FIXME#2
			av_free_packet(audio_decoding_pkt);
			delete audio_decoding_pkt;
			audio_decoding_pkt = 0;
		}

		if(aDataDque.size() > 0){

			apnvfts = false;

			audio_decoding_pkt = aDataDque.front();
			if(audio_decoding_pkt->pts != AV_NOPTS_VALUE){
				synchroPTS = static_cast<double>(audio_decoding_pkt->pts);
				//debugGREEN("synchroPTS %lf audio... %ld\n", synchroPTS, audio_decoding_pkt->pts);
			}
			// save for restoring later
			decoding_pkt_size = audio_decoding_pkt->size;
			decoding_pkt_data = audio_decoding_pkt->data;
			// take out of the queue
			aDataDque.pop_front();

		}else{
			// can't get packets because queue is empty
			return VIDEO_NEED_DATA;

		}
	}

	// should never come out here
	ASSERT(false);
}



int VideoPlayer::get_playing_time(double & t){

	if(!audio_decoding_pkt || apnvfts || audioStream == -1){
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
    double offset = static_cast<double>(samples) /
    		static_cast<double>(aCodecCtx->sample_rate);

    //debugRED("Hz %lf\n",static_cast<double>(aCodecCtx->sample_rate));
    //debugRED("Offset %lf\n",offset);

    ASSERT(fxcxb>0);
    ASSERT(audio_decoding_pkt);
    //size in seconds of data loaded from last packet
    double lastpckloadedsize =
    		((audio_decoding_pkt->size - decoding_pkt_size) * 8.0f) / fxcxb;

    //debugRED("lastpckloadedsize %lf\n",lastpckloadedsize );

    ASSERT(SC(double,atbaseden)>0);
    //debugRED("synchroPTS %lf, %lf, %lf, %lf\n", synchroPTS*(SC(double,atbasenum) / SC(double,atbaseden)),synchroPTS, SC(double,atbasenum),SC(double,atbaseden));
	t = synchroPTS*(SC(double,atbasenum) / SC(double,atbaseden))
		- ((SC(double,nloadedbuffs) - 1.0f)*buffLenInSec)
		+ offset + lastpckloadedsize;

	return VIDEO_OK;
}



/**
 * 	If audio queue size is < AUDIO_QUEUE_MAX_SIZE or video queue size is <
 * 	VIDEO_QUEUE_MAX_SIZE, it gets at least one audio packet and one video
 * 	frame unless no more packets can be fetched from the file.
 *
 * @return
 * 		On success returns VIDEO_OK. If can't fetch more packets, then it
 * 		returns VIDEO_ENDED. On error returns VIDEO_ERROR.
 */


int VideoPlayer::get_more_data(void){


	bool got_audio = aDataDque.size() >= AUDIO_QUEUE_MAX_SIZE;
	bool got_video = vDataQue.size() >= VIDEO_QUEUE_MAX_SIZE;

	while(!got_audio || !got_video){

		AVPacket *mPacket = new AVPacket;
		if(!mPacket){
			debugERROR("Can't allocate packet.\n");
			return VIDEO_ERROR;
		}
		av_init_packet(mPacket);

		if(av_read_frame(pFormatCtx, mPacket) != 0){
			// No chequeo errores aca :s Simplemente supongo que termino de
			// desempaquetar.
			//debugGREEN("Ended unpacking at %lf :D\n", mplayingtime);
			av_free_packet(mPacket);
			delete mPacket;
			return VIDEO_ENDED;

		}else{

			lastPts = mPacket->pts;

			if(mPacket->stream_index == videoStream){

				vDataQue.push(mPacket);
				got_video = true;
				mPacket = 0;

			}else if(mPacket->stream_index == audioStream){

				aDataDque.push_back(mPacket);
				got_audio = true;
				mPacket = 0;

			}else{

				debugRED("Not my stream :s\n");
				av_free_packet(mPacket);
				delete mPacket;

			}
		}
	}

	return VIDEO_OK;
}




int VideoPlayer::set_screen_texture(void){
    // Texture for the screen
    Ogre::TextureManager::getSingleton().remove("RttTex");
	rtt_texture =
			Ogre::TextureManager::getSingleton().createManual("RttTex",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D, pCodecCtx->width,
			pCodecCtx->height, 0,
			Ogre::PF_X8R8G8B8, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	//set material texture
	renderMaterial->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(
    		"RttTex");
    //set screen material
    mMiniScreen->setMaterial("RttMat");

    return VIDEO_OK;
}



void VideoPlayer::print_video_info(void){
	// // // // DEBUG INFO

	    cyanPrint("VIDEO DEBUG\n");
	    lightGreenPrint("Video nb of streams %d\n", pFormatCtx->nb_streams);
	    if(audioStream != -1){
			cyanPrint("AUDIO DEBUG\n");
			const char *format = av_get_sample_fmt_name(aCodecCtx->sample_fmt);
			if(format){
				lightGreenPrint("Audio sample format %s\n",format );
			}
			lightGreenPrint("Audio nb of channels %d\n",aCodecCtx->channels);
			lightGreenPrint("Audio nb of bits per sample %d\n",
					av_get_bytes_per_sample(aCodecCtx->sample_fmt)*8);
			lightGreenPrint("Audio frequency %dHz\n",aCodecCtx->sample_rate);
	    }
	    cyanPrint("OTHER DEBUGS\n");
	    lightGreenPrint("AVCODEC_MAX_AUDIO_FRAME_SIZE: %d\n",
	    		AVCODEC_MAX_AUDIO_FRAME_SIZE);
	    cyanPrint("***********\n");
	    // // // //

}





/*----------------------------------------------------------------------------*/
/*----------------------------- EXTRA FUNCTIONS ------------------------------*/
/*----------------------------------------------------------------------------*/


/*
 * Pinta la pantalla de video del color definido por 'R', 'G', 'B'.
 */
int VideoPlayer::paint_screen(unsigned char R, unsigned char G, unsigned char B)
{

	if(!rtt_texture.get()){
		return VIDEO_ERROR;
	}

	//Actualizar la textura de la pantalla
	Ogre::HardwarePixelBufferSharedPtr PixelBuffer =
		rtt_texture->getBuffer();

	PixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

	unsigned char * pDest =
		static_cast<unsigned char*>(PixelBuffer->getCurrentLock().data);

	int width = defaultwidth, height = defaultheight;
	if(pCodecCtx){
		width = pCodecCtx->width;
		height = pCodecCtx->height;
	}

	for (size_t i = 0; i <width*height*4; i+=4)
	{
		pDest[i] 		= R;
		pDest[i + 1] 	= G;
		pDest[i + 2]    = B;
	}

	PixelBuffer->unlock();

	return VIDEO_OK;
}



/*
 * @VideoPlayer::SaveFrame
 * Saves an RGB24 (RGBRGBRGB...) (one byte for R, one for G and one for B) to
 * the hard drive in ppm format. We only use it for debug issues.
 */
void VideoPlayer::SaveFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
    FILE *pFile;
    char szFilename[32];
    int  y;

    // Open file
    sprintf(szFilename, "./Frames/frame%d.ppm", iFrame);
    pFile=fopen(szFilename, "wb");
    if(pFile==NULL)
        return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for(y=0; y<height; y++)
        fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

    // Close file
    fclose(pFile);
}

