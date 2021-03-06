/*
 * SoundEnums.h
 *
 *  Created on: May 3, 2012
 *     Company: estrategia201Z
 *      Author: Budde, Carlos Esteban.
 */

#ifndef SOUNDENUMS_H_
#define SOUNDENUMS_H_


/* Ogre's resource manager name for sound files. */
#define  SOUNDS_RESOURCE_GROUP_NAME  "Sounds"

/* Number of internal buffers for each SSoundSource. */
#define  SS_NUM_INT_BUFFERS	3

/* Maximum data size of each internal buffer in a SSoundSource. */
#define  SS_SIZE_INT_BUFFERS	(1<<14)  // 16 KB

/* Minimum buffer length for streaming.
 * Audio files smaller than this may NOT use streaming. */
#define	 SS_MIN_STREAM_LEN  SS_SIZE_INT_BUFFERS

/* Static's array size for OGG files parsing. Usually 4096 */
#define  OGG_BUFF_SIZE  (1<<12)  // 4 K


/* Supported audio compression formats */
typedef enum {
	SS_NONE,
	SS_WAV,
	SS_OGG
} SSformat;


/* SoundBuffer type */
typedef enum {
	SS_BUF_LOADED,  	// Buffer loaded entirely into memory.
	SS_BUF_STREAM_WAV,	// Buffer for streaming WAV files.
	SS_BUF_STREAM_OGG	// Buffer for streaming OGG files.
} SSbuftype;


/* SoundSource type */
typedef enum {
	SS_SRC_LOADED,  // Plays buffers entirely loaded into memory.
	SS_SRC_STREAM   // Uses streaming mechanisms.
} SSsrctype;


/* SoundSystem error codes */
typedef enum {
	SS_NO_ERROR,
	SS_NO_MEMORY,
	SS_NO_SOURCES,			/* No free sources */
	SS_NO_BUFFER,           /* NULL or invalid buffer parameter */
	SS_INVALID_FILE,		/* Unsupported/erroneous file audio format */
	SS_FILE_TOO_SMALL,		/* File size insufficient for streaming playback */
	SS_FILE_NOT_FOUND,		/* Can't load sound, inexistent filename */
	SS_NO_BUFFER_ATTACHED,	/* Can't play, no buffers attached to source */
	SS_UNINITIALIZED_SOURCE,/* Source not yet registered in AL system */
	SS_INTERNAL_ERROR
} SSerror;


/* SoundSource playback state */
typedef enum {
	SS_PLAYING,
	SS_PAUSED,
	SS_FINISHED
} SSplayback;


#endif /* SOUNSRETURNCODES_H_ */
