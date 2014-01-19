/*
 * SoundEnums.h
 *
 *  Created on: May 3, 2012
 *     Company: estrategia201Z
 *      Author: Budde, Carlos Esteban.
 */

#ifndef SOUNDENUMS_H_
#define SOUNDENUMS_H_

namespace mm {


// Ogre's resource manager name for sound files.
//
#define  SOUNDS_RESOURCE_GROUP_NAME  "Sounds"


// Number of internal buffers for each SSoundSource.
//
#define  SS_NUM_INT_BUFFERS	  4


// Maximum data size of each internal buffer in a SSoundSource.
//
#define  SS_SIZE_INT_BUFFERS	(1<<14)  // 16 KB


// Minimum buffer length for streaming.
// Audio files smaller than this may NOT use streaming.
//
#define	 SS_MIN_STREAM_LEN  SS_SIZE_INT_BUFFERS


// Static's array size for OGG files parsing. Usually 4096
// XXX MUST BE <= SS_SIZE_INT_BUFFERS
//
#define  OGG_BUFF_SIZE  (1<<12)  // 4 K


// Default gain values (i.e. sound volume) for environmental and unit sounds.
//
#define  DEFAULT_ENV_GAIN	(0.07f)
#define  DEFAULT_UNIT_GAIN	(1.0) // (10.0f*DEFAULT_ENV_GAIN)


// Maximum (and middle) distance fade off for sounds
// e.g. no sound will be attenuated further away than this units
// See SoundManager::playSound()
//
#define  MAX_ATTENUATION_DISTANCE  800.0f
#define  MID_ATTENUATION_DISTANCE  0.017*MAX_ATTENUATION_DISTANCE


// Range of randomization (in seconds) for random-silence playbacks
// (see SoundHandler::newPlaylist(...)
//
#define  MIN_SILENCE  (2.0f)
#define  MAX_SILENCE  (10.0f*MIN_SILENCE)


// Supported audio compression formats.
typedef enum {
	SS_NOTHING,
	SS_WAV,
	SS_OGG
} SSformat;


// SoundBuffer type.
typedef enum {
	SS_BUF_LOADED,  	// Buffer loaded entirely into memory.
	SS_BUF_STREAM_WAV,	// Buffer for streaming WAV files.
	SS_BUF_STREAM_OGG,	// Buffer for streaming OGG files.
	SS_BUF_NONE			// Error
} SSbuftype;


// SoundSource type.
typedef enum {
	SS_SRC_LOADED,  // Plays buffers entirely loaded into memory.
	SS_SRC_STREAM   // Uses streaming mechanisms.
} SSsrctype;


// SoundSystem error codes.
typedef enum {
	SS_NO_ERROR = 0,
	SS_NO_MEMORY,
	SS_NO_SOURCES,			  // No free sources to start playback
	SS_NO_BUFFER,             // NULL or invalid buffer parameter
    SS_INVALID_HANDLE,        // Handle is not valid in the sound system
    SS_INVALID_FILE,          // Unsupported/erroneous file audio format
	SS_FILE_TOO_SMALL,		  // File size insufficient for streaming playback
	SS_FILE_NOT_FOUND,		  // Inexistent filename (sound not loaded)
	SS_NO_BUFFER_ATTACHED,	  // Can't play, no buffers attached to source
	SS_UNINITIALIZED_SOURCE,  // Source not yet registered in AL system
	SS_ILLEGAL_OPERATION,     // Operation not allowed in current state
	SS_INTERNAL_ERROR=(1<<31) // Unspecified error
} SSerror;


// SoundSource playback state (XXX don't alter order!)
typedef enum {
	SS_NONE			= 0,
	SS_PLAYING		= (1<<0),
	SS_PAUSED		= (1<<1),
	SS_FADING_OUT_AND_PAUSE = (1<<2),
	SS_FADING_OUT	= (1<<3),
	SS_FADING_IN	= (1<<4),
	SS_FINISHED		= (1<<5),
	SS_SILENCE      = (1<<6)
} SSplayback;


// Debug printing
#ifndef DEBUG
#  define  SSenumStr(d)
#else
#  define  SSenumStr(d)  soundEnumToStr(d)
   static inline const char* soundEnumToStr(SSformat f)
   {
	   switch(f)
	   {
	   case SSformat::SS_NOTHING:
		   return "SS_NOTHING";
		   break;
	   case SSformat::SS_OGG:
		   return "SS_OGG";
		   break;
	   case SSformat::SS_WAV:
		   return "SS_WAV";
		   break;
	   default:
		   return "<unrecognized audio format (MP3?)>";
		   break;
	   }
	   return "";
   }
   static inline const char* soundEnumToStr(SSbuftype t)
   {
	   switch(t)
	   {
	   case SSbuftype::SS_BUF_NONE:
		   return "SS_BUF_NONE";
		   break;
	   case SSbuftype::SS_BUF_LOADED:
		   return "SS_BUF_LOADED";
		   break;
	   case SSbuftype::SS_BUF_STREAM_OGG:
		   return "SS_BUF_STREAM_OGG";
		   break;
	   case SSbuftype::SS_BUF_STREAM_WAV:
		   return "SS_BUF_STREAM_WAV";
		   break;
	   default:
		   return "<unrecognized buffer type>";
		   break;
	   }
	   return "";
   }
   static inline const char* soundEnumToStr(SSsrctype t)
   {
	   switch(t)
	   {
	   case SSsrctype::SS_SRC_LOADED:
		   return "SS_SRC_LOADED";
		   break;
	   case SSsrctype::SS_SRC_STREAM:
		   return "SS_SRC_STREAM";
		   break;
	   default:
		   return "<unrecognized source type>";
		   break;
	   }
	   return "";
   }
   static inline const char* soundEnumToStr(SSerror e)
   {
	   switch(e)
	   {
	   case SSerror::SS_NO_ERROR:
		   return "SS_NO_ERROR";
		   break;
	   case SSerror::SS_NO_MEMORY:
		   return "SS_NO_MEMORY";
		   break;
	   case SSerror::SS_NO_SOURCES:
		   return "SS_NO_SOURCES";
		   break;
	   case SSerror::SS_NO_BUFFER:
		   return "SS_NO_BUFFER";
		   break;
	   case SSerror::SS_INVALID_FILE:
		   return "SS_INVALID_FILE";
		   break;
	   case SSerror::SS_FILE_TOO_SMALL:
		   return "SS_FILE_TOO_SMALL";
		   break;
	   case SSerror::SS_FILE_NOT_FOUND:
		   return "SS_FILE_NOT_FOUND";
		   break;
	   case SSerror::SS_NO_BUFFER_ATTACHED:
		   return "SS_NO_BUFFER_ATTACHED";
		   break;
	   case SSerror::SS_UNINITIALIZED_SOURCE:
		   return "SS_UNINITIALIZED_SOURCE";
		   break;
	   case SSerror::SS_ILLEGAL_OPERATION:
		   return "SS_ILLEGAL_OPERATION";
		   break;
	   case SSerror::SS_INTERNAL_ERROR:
		   return "SS_INTERNAL_ERROR";
		   break;
	   default:
		   return "<unrecognized error>";
		   break;
	   }
	   return "";
   }
   static inline const char* soundEnumToStr(SSplayback s)
   {
	   switch(s)
	   {
	   case SSplayback::SS_NONE:
		   return "SS_NONE";
		   break;
	   case SSplayback::SS_PLAYING:
		   return "SS_PLAYING";
		   break;
	   case SSplayback::SS_PAUSED:
		   return "SS_PAUSED";
		   break;
	   case SSplayback::SS_FADING_OUT_AND_PAUSE:
		   return "SS_FADING_OUT_AND_PAUSE";
		   break;
	   case SSplayback::SS_FADING_OUT:
		   return "SS_FADING_OUT";
		   break;
	   case SSplayback::SS_FADING_IN:
		   return "SS_FADING_IN";
		   break;
	   case SSplayback::SS_FINISHED:
		   return "SS_FINISHED";
		   break;
	   default:
		   return "<unrecognized playback state>";
		   break;
	   }
	   return "";
   }
#endif

}

#endif  // SOUNDENUMS_H_
