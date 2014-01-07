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

#include <types/StackVector.h>
#include <debug/DebugUtil.h>
#include "SoundEnums.h"

#if defined(_WIN32) || defined(CYGWIN) || defined(MINGW)
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  include <AL/al.h>
#  include <AL/alc.h>
#else
#  error "Unsupported platform. ABORTING COMPILATION."
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

	/**
	 ** @brief
	 ** Extract 'size' bytes from internal file and store them inside 'buf'
	 **
	 ** @remarks
	 ** Reading starts at the current position of the internal file get pointer.
	 **
	 ** @remarks
	 ** When EOF is reached:
	 ** if (repeat)
	 ** 	continue reading from the beginning of file
	 ** else
	 ** 	fill what we can, set 'finish'=true, leave file get pointer at end
	 **
	 ** @param
	 **    buf: (in/out) OpenAL buffer to fill
	 **   size: (in/   ) number of bytes to read from file into buf
	 ** repeat: (in/   ) shall we repeat the file reading on EOF?
	 ** finish: (  /out) if !repeat, tells whether EOF was reached
	 **
	 ** @returns
	 ** Number of bytes effectively stored into 'buf'
	 ** -1 on error
	 **/
	inline virtual long long int
	filler(ALBuffer& buf, size_t size, bool repeat, bool* finish);

	/**
	 ** @brief
	 ** Seek internal file get pointer to the audio data start position.
	 **/
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
	core::StackVector<char, SS_SIZE_INT_BUFFERS> pcmData;	// Aux array

	inline StreamWAVSoundBuffer();
	inline virtual ~StreamWAVSoundBuffer();

	/**
	 ** @brief
	 ** Check base class method comment for explanation
	 **/
	long long int
	filler(ALBuffer& buf, size_t size, bool repeat, bool *finish);

	/**
	 ** @brief
	 ** Check base class method comment for explanation
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
	core::StackVector<char, SS_SIZE_INT_BUFFERS> pcmData;	// Aux array

	inline StreamOGGSoundBuffer();
	inline virtual ~StreamOGGSoundBuffer();

	/**
	 ** @brief
	 ** Check base class method comment for explanation
	 **/
	long long int
	filler(ALBuffer& buf, size_t size, bool repeat, bool* finish);

	/**
	 ** @brief
	 ** Check base class method comment for explanation
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
SoundBuffer::filler(ALBuffer& buf, size_t size, bool repeat, bool* finish)
{
	debugERROR("filler() called from SoundBuffer base class: WRONG!\n");
	return -1ll;
}


////////////////////////////////////////////////////////////////////////////////
inline void
SoundBuffer::restart()
{
	debugERROR("restart() called from SoundBuffer base class: WRONG!\n");
}


////////////////////////////////////////////////////////////////////////////////
inline
StreamWAVSoundBuffer::StreamWAVSoundBuffer() :
	SoundBuffer(SSbuftype::SS_BUF_STREAM_WAV),
	dataStart(0),
	file(new std::ifstream)
{ /* Default constructor suffices. */ }


////////////////////////////////////////////////////////////////////////////////
inline
StreamWAVSoundBuffer::~StreamWAVSoundBuffer()
{
	if(file && file->is_open()) {
		file->close();
	}
	delete file;
	pcmData.clear();
}


////////////////////////////////////////////////////////////////////////////////
inline void
StreamWAVSoundBuffer::restart()
{
	if(file && file->is_open()) {
		pcmData.clear();
		file->clear();  // Reset file error state flags
		file->seekg(dataStart, std::ifstream::beg);
		ASSERT(file->good());
		ASSERT(!file->eof());
		ASSERT(file->tellg() == dataStart);
	}
}


////////////////////////////////////////////////////////////////////////////////
inline
StreamOGGSoundBuffer::StreamOGGSoundBuffer() :
	SoundBuffer(SSbuftype::SS_BUF_STREAM_OGG),
	bitStreamSection(0),
	oggFile(new OggVorbis_File),
	file(0)
{ /* Default constructor suffices. */ }


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
		int err = ov_pcm_seek(oggFile, 0);
		bitStreamSection = 0;
#ifdef DEBUG
		if (err != 0) {
			std::map<int,const char*> errMap =
				{{OV_ENOSEEK, "OV_ENOSEEK"},
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
