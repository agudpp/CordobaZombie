/*
 * SoundBuffer.h
 *
 *	OpenAL buffer wrapper.
 *
 *  Created on: May 22, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef SOUNDBUFFER_H_
#define SOUNDBUFFER_H_


#include <sys/types.h>  // off_t
#include <vector>
#include <iostream>
#include <fstream>
#include <vorbis/vorbisfile.h>  // OGG-vorbis parsing

#include <debug/DebugUtil.h>
#include "SoundEnums.h"

#if defined(_WIN32) || defined(CYGWIN)
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  include <AL/al.h>
#  include <AL/alc.h>
#else
#  error "Unsupported platform, aborting compilation."
#endif
#ifdef DEBUG
#  include <map>
#endif


typedef unsigned long ulong;
typedef unsigned char uchar;
typedef ALuint ALBuffer;

namespace mm {


// Generic sound buffer
struct SoundBuffer
{
	SSbuftype type;		 	// Buffer type: SS_BUF_LOADED vs. SS_BUF_STREAM
	ALBuffer  buffer;	 	// OpenAL buffer identifier.
	bool	  loaded;	 	// Audio data loaded, buffer ready to be played.
	ALenum    format;		// Audio format (Mono8, Mono16, Stereo, etc)
	float	  duration;	 	// Total playing time, in seconds.
	short	  chan;		 	// Number of channels.
	int		  freq;		 	// Audio frequency (sample rate)

	inline SoundBuffer(SSbuftype buffType = SSbuftype::SS_BUF_LOADED);
	inline virtual ~SoundBuffer();

	// For children internal files manipulation.
	inline virtual long long int
	filler(ALBuffer& buf, size_t size, bool repeat);

	inline virtual void
	restart();

private:
	// Avoid copy and assignment
	SoundBuffer(const SoundBuffer&);
	SoundBuffer& operator=(const SoundBuffer&);
};


// Sound buffer for streaming playback of WAV files.
struct StreamWAVSoundBuffer : public SoundBuffer
{
	std::ifstream* file;		// Pointer to the audio file
	std::streampos dataStart;	// File's audio data start position
	char* pcmData;				// Aux array (here only for efficiency)

	inline StreamWAVSoundBuffer();
	inline virtual ~StreamWAVSoundBuffer();

	/**
	 ** @brief
	 ** Extract 'size' bytes from internal file and store them inside 'buf'
	 **
	 ** @remarks
	 ** Reading starts at internal file's get pointer current position.
	 ** If EOF is reached and 'repeat' is true, continue reading
	 ** from the beginning of the file.
	 **
	 ** @returns
	 ** Number of bytes effectively stored into 'buf'
	 ** -1 on error.
	 **/
	long long int
	filler(ALBuffer& buf, size_t size, bool repeat);

	/**
	 ** @brief
	 ** Seek internal file get pointer to the audio data start position.
	 **/
	inline void
	restart();

private:
	// Avoid copy and assignment
	StreamWAVSoundBuffer(const StreamWAVSoundBuffer&);
	StreamWAVSoundBuffer& operator=(const StreamWAVSoundBuffer&);
};


// Sound buffer for streaming playback of OGG files.
struct StreamOGGSoundBuffer : public SoundBuffer
{
	FILE* file;					// Pointer to the audio file
	OggVorbis_File* oggFile;	// Pointer to the OGG formated audio file
	int bitStreamSection;		// File's audio data current reading position
	std::vector<char> pcmData;	// Aux array (here only for efficiency)

	inline StreamOGGSoundBuffer();
	inline virtual ~StreamOGGSoundBuffer();

	/**
	 ** @brief
	 ** Extract 'size' bytes from internal file and store them inside 'buf'
	 **
	 ** @remarks
	 ** Reading starts at internal file's get pointer current position.
	 ** If EOF is reached and 'repeat' is true, continue reading
	 ** from the beginning of the file.
	 **
	 ** @returns
	 ** Number of bytes effectively stored into 'buf'
	 ** -1 on error
	 **/
	long long int
	filler(ALBuffer& buf, size_t size, bool repeat);

	/**
	 ** @brief
	 ** Seek internal file's get pointer to the audio data start position.
	 **/
	inline void
	restart();

private:
	// Avoid copy and assignment
	StreamOGGSoundBuffer(const StreamOGGSoundBuffer&);
	StreamOGGSoundBuffer& operator=(const StreamOGGSoundBuffer&);
};


/******************************************************************************/
/****************************     INLINES     *********************************/

////////////////////////////////////////////////////////////////////////////////
inline
SoundBuffer::SoundBuffer(SSbuftype buffType) :
	type(buffType),
	buffer(0),
	loaded(false),
	format(AL_FORMAT_MONO16),
	duration(0.0),
	chan(0),
	freq(0)
{ /* Default constructor suffices. */ }


////////////////////////////////////////////////////////////////////////////////
inline
SoundBuffer::~SoundBuffer()
{
	if (alcGetCurrentContext() != NULL && alIsBuffer(this->buffer)) {
		alDeleteBuffers(1,&(this->buffer));
	}
}


////////////////////////////////////////////////////////////////////////////////
inline long long int
SoundBuffer::filler(ALBuffer& buf, size_t size, bool repeat)
{
	debugERROR("\nfiller() called from SoundBuffer base class. This is WRONG!%s", "\n");
	return -1ll;
}


////////////////////////////////////////////////////////////////////////////////
inline void
SoundBuffer::restart()
{
	debugERROR("\nBuffer restart() called from SoundBuffer base class. "
				"This is WRONG!%s", "\n");
}


////////////////////////////////////////////////////////////////////////////////
inline
StreamWAVSoundBuffer::StreamWAVSoundBuffer() :
	SoundBuffer(SSbuftype::SS_BUF_STREAM_WAV),
	dataStart(0),
	file(new std::ifstream),
	pcmData(new char[SS_SIZE_INT_BUFFERS]())
{ /* Default constructor suffices. */ }


////////////////////////////////////////////////////////////////////////////////
inline
StreamWAVSoundBuffer::~StreamWAVSoundBuffer()
{
	if(file && file->is_open()) {
		file->close();
	}
	delete file;
	delete[] pcmData;
}


////////////////////////////////////////////////////////////////////////////////
inline void
StreamWAVSoundBuffer::restart()
{
	if(file && file->is_open()) {
		file->seekg((long)dataStart, std::ifstream::beg);
	}
}


////////////////////////////////////////////////////////////////////////////////
inline
StreamOGGSoundBuffer::StreamOGGSoundBuffer() :
	SoundBuffer(SSbuftype::SS_BUF_STREAM_OGG),
	bitStreamSection(0),
	oggFile(new OggVorbis_File),
	file(0)
{ pcmData.reserve(SS_SIZE_INT_BUFFERS); }


////////////////////////////////////////////////////////////////////////////////
inline
StreamOGGSoundBuffer::~StreamOGGSoundBuffer()
{
#if defined(_WIN32) || defined(CYGWIN)
	if (!ov_test_callbacks(file, oggFile, 0, 0, OV_CALLBACKS_DEFAULT))
#else
	if (!ov_test(file, oggFile, 0, 0))
#endif
		ov_clear(oggFile);  // Closes both file and oggFile
	else
		fclose(file);
	delete oggFile;
	pcmData.clear();
}


////////////////////////////////////////////////////////////////////////////////
inline void
StreamOGGSoundBuffer::restart()
{
	if(file && fileno(file) >= 0 && oggFile && oggFile->seekable) {
		int err(0);
		err = ov_pcm_seek(oggFile, 0);
#ifdef DEBUG
		if (err != 0) {
			std::map<int,const char*> errMap = {{OV_ENOSEEK, "OV_ENOSEEK"},
												 {OV_EINVAL, "OV_EINVAL"},
												 {OV_EREAD, "OV_EREAD"},
												 {OV_EFAULT, "OV_EFAULT"},
												 {OV_EBADLINK, "OV_EBADLINK"}};
			debugERROR("Restart failed: %s\n", errMap[err]);
		}
#endif
	}
}

}

#endif /* SOUNDBUFFER_H_ */
