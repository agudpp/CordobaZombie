/*
 * BufferBuilder.cpp
 *
 *  Abstraction of a SoundBuffer creation from an audio file.
 *
 *  Created on: May 22, 2012
 *     Company: estrategia201Z
 *      Author: Budde, Carlos Esteban.
 */

#include <map>
#include <vector>
#include <fstream>
#include <sys/stat.h>	// stat()
#include <cstdio>		// FILE* family
#include <cstring>		// strcmp
#include <strings.h>	// strcasecmp
#include <vorbis/vorbisfile.h>  // OGG-vorbis parsing

#include <debug/DebugUtil.h>
#include "SoundEnums.h"
#include "SoundBuffer.h"
#include "BufferBuilder.h"

#if defined(_WIN32) || defined(CYGWIN) || defined(MINGW)
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  include <AL/al.h>
#  include <AL/alc.h>
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif

namespace mm {

////////////////////////////////////////////////////////////////////////////////
SSerror
BufferBuilder::bufferFromFile(const std::string& sName,
								SoundBuffer** buffer,
								SSformat  format,
								SSbuftype type)
{
	SSerror err(SSerror::SS_NO_ERROR);
	std::ifstream file;
	FILE* filep(0);

	if (*buffer) {
		delete *buffer;
	}

	if (format == SSformat::SS_NOTHING) {
		// Filetype not specified, checking file extension.
		size_t ext_idx = sName.find_last_of(".") + 1;
		std::string ext = sName.substr(ext_idx, sName.size()-ext_idx);
		if (strcasecmp(ext.c_str(),"WAV") == 0) {
			format = SSformat::SS_WAV;
		} else if (strcasecmp(ext.c_str(),"OGG") == 0) {
			format = SSformat::SS_OGG;
		}
	}

	switch(format)
	{
	case SSformat::SS_WAV:
		if (type==SSbuftype::SS_BUF_LOADED) {
			*buffer = new SoundBuffer();
			file.open(sName.c_str(), std::ifstream::in | std::ifstream::binary);
			if (!file.good() || !file.is_open()) {
				delete *buffer;
				goto bad_file;
			}
			err = loadedBufferFromWAV(file, *buffer);

		} else {
			ASSERT(type==SSbuftype::SS_BUF_STREAM_WAV);
			*buffer = new StreamWAVSoundBuffer();
			std::ifstream* file = ((StreamWAVSoundBuffer*)*buffer)->file;
			file->open(sName.c_str(), std::ifstream::in | std::ifstream::binary);
			if (!file->good() || !file->is_open()) {
				delete *buffer;
				goto bad_file;
			}
			err = streamBufferFromWAV((StreamWAVSoundBuffer*) *buffer);
		}
		break;

	case SSformat::SS_OGG:
		// Fucking OGG library can't handle std::ifstream
		if (type==SSbuftype::SS_BUF_LOADED) {
			*buffer = new SoundBuffer();
			filep = fopen(sName.c_str(), "rb");
			if (!filep || fileno(filep) < 0) {
				delete *buffer;
				goto bad_file;
			}
			err = loadedBufferFromOGG(filep, *buffer);

		} else {
			ASSERT(type==SSbuftype::SS_BUF_STREAM_OGG);
			*buffer = new StreamOGGSoundBuffer();
			((StreamOGGSoundBuffer*)*buffer)->file = fopen(sName.c_str(), "rb");
			filep = ((StreamOGGSoundBuffer*)*buffer)->file;
			if (!filep || fileno(filep) < 0) {
				delete *buffer;
				goto bad_file;
			}
			err = streamBufferFromOGG((StreamOGGSoundBuffer*) *buffer);
		}
		break;

	default:
		return SSerror::SS_INVALID_FILE;
		break;
	}

	if (err != SSerror::SS_NO_ERROR) {
		delete *buffer;
	}

//	success:
	return err;

	bad_file:
		debug("Can't open binary file %s for reading.\n", sName.c_str());
		return SSerror::SS_FILE_NOT_FOUND;
}


////////////////////////////////////////////////////////////////////////////////
SSbuftype
BufferBuilder::bestBufferType(const std::string& sName)
{
	SSbuftype type(SSbuftype::SS_BUF_NONE);
	SSformat format(SSformat::SS_NOTHING);
	struct stat fileStat;
	int err;

	// Checking file extension.
	size_t ext_idx = sName.find_last_of(".") + 1;
	std::string ext = sName.substr(ext_idx, sName.size()-ext_idx);
	if (strcasecmp(ext.c_str(),"WAV") == 0) {
		format = SSformat::SS_WAV;
	} else if (strcasecmp(ext.c_str(),"OGG") == 0) {
		format = SSformat::SS_OGG;
	}

	// Reading file information.
	err = stat(sName.c_str(), &fileStat);
	if (err < 0) {
		debugERROR("Failed reading file \"%s\" information.\n", sName.c_str());
		return type;
	}

	// Deciding best buffer type.
	switch (format) {
	case SSformat::SS_WAV:
		if (fileStat.st_size < MAX_WAV_FILE_SIZE) {
			type = SSbuftype::SS_BUF_LOADED;
		} else {
			type = SSbuftype::SS_BUF_STREAM_WAV;
		}
		break;

	case SSformat::SS_OGG:
		if (fileStat.st_size < MAX_OGG_FILE_SIZE) {
			type = SSbuftype::SS_BUF_LOADED;
		} else {
			type = SSbuftype::SS_BUF_STREAM_OGG;
		}
		break;

	case SSformat::SS_NOTHING:
		debugERROR("File \"%s\": unsupported sound encoding.\n", sName.c_str());
		break;

	default:
		debugERROR("WTF just happened?\n");
		break;
	}

	return type;
}


#ifdef _MSC_VER
   typedef __uint32 uint32_t
#else
   #include <stdint.h>
#endif
static union {
	uint32_t i;
	char c[4];
} bint = {0x01020304};


////////////////////////////////////////////////////////////////////////////////
// Returns the next 16 bytes from a buffer as an unsigned integer
static inline short
readByte16(const unsigned char buffer[2])
{
	if(bint.c[0] == 1)	// BigEndian
		return (buffer[0] << 8) + buffer[1];
	else	// LittleEndian
		return (buffer[1] << 8) + buffer[0];
}


////////////////////////////////////////////////////////////////////////////////
// Returns the next 32 bytes from a buffer as an unsigned integer
static inline int
readByte32(const unsigned char buffer[4])
{
	if(bint.c[0] == 1)	// BigEndian
		return (buffer[0] << 24) + (buffer[1] << 16)
				+ (buffer[2] << 8) + buffer[3];
	else	// LittleEndian
		return (buffer[3] << 24) + (buffer[2] << 16)
				+ (buffer[1] << 8) + buffer[0];
}


////////////////////////////////////////////////////////////////////////////////
// Read number of channels and bits per sample to determine playing format.
static ALenum
computePlayingFormat(short channels, short BPS)
{
	switch(channels)
	{
	case 1:
		if(BPS == 8) {
			return AL_FORMAT_MONO8;
		} else {
			return AL_FORMAT_MONO16;
		}
		break;
	case 2:
		return AL_FORMAT_STEREO16;
		break;
	case 4:
		return alGetEnumValue("AL_FORMAT_QUAD16");
		break;
	case 6:
		return alGetEnumValue("AL_FORMAT_51CHN16");
		break;
	case 7:
		return alGetEnumValue("AL_FORMAT_61CHN16");
		break;
	case 8:
		return alGetEnumValue("AL_FORMAT_71CHN16");
		break;
	default:
		// Can't determine buffer format
		debug("%s", "Could not determine buffer format, defaulting to MONO\n");
		return AL_FORMAT_MONO16;
		break;
	}
	return AL_FORMAT_MONO16;
}


////////////////////////////////////////////////////////////////////////////////
// Free the SoundBuffer, releasing all memory and reseting all values.
static void
clearSoundBuffer(SoundBuffer* buffer)
{
	if(!buffer) return;
	if (alcGetCurrentContext() != NULL && alIsBuffer(buffer->buffer)) {
		alDeleteBuffers(1,&(buffer->buffer));
	}
	buffer->duration = 0.0;
	buffer->chan     = 0;
	buffer->freq     = 0;
	buffer->loaded   = false;
	return;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
BufferBuilder::loadedBufferFromWAV(std::ifstream& file, SoundBuffer *buffer)
{
	// Auxiliary buffers.
	uchar* pcmData(0);  // Raw PCM audio data
	char magic[5];
	unsigned char buffer32[4];
	unsigned char buffer16[2];
	short bitsPerSample = 0;
	short audioFormat = 0;
	int audioDataSize = 0;
	int byteRate = 0;
	ALenum alErr = AL_NO_ERROR;

	// Preconditions checking and stuff.
	ASSERT(file.is_open() && file.good());
	ASSERT(buffer != NULL);
	ASSERT(buffer->type == SSbuftype::SS_BUF_LOADED);
	if (alcGetCurrentContext() == NULL) {
		debugERROR("NULL OpenAL context! Aborting.\n");
		file.close();
		return SSerror::SS_INTERNAL_ERROR;
	}
	clearSoundBuffer(buffer);
	alGenBuffers(1, &(buffer->buffer));
	alErr = alGetError();
	if (alErr != AL_NO_ERROR) {
		file.close();
		if (alErr == AL_OUT_OF_MEMORY) {
			return SSerror::SS_NO_MEMORY;
		} else {
			return SSerror::SS_INTERNAL_ERROR;
		}
	}
	ASSERT(alIsBuffer(buffer->buffer));

	// Parse file header.
	magic[4] = '\0';
	if (!file.read(magic,4) || strcmp(magic,"RIFF") != 0) {
		debug("%s","No RIFF header.\n");
		file.close();
		return SSerror::SS_INVALID_FILE;
	}
	file.ignore(4);  // Skipping field "chunk size"
	if (!file.read(magic,4) || strcmp(magic,"WAVE") != 0 ||
	    !file.read(magic,4) || strcmp(magic,"fmt ") != 0 ) {
		debug("%s","Not \"WAVEfmt \" format line.\n");
		file.close();
		return SSerror::SS_INVALID_FILE;
	}
	file.ignore(4);  // Skipping field "size of sub-chunk 1"
	file.read((char*)buffer16, 2);
	ASSERT(file.good());
	audioFormat = readByte16(buffer16);

	file.read((char*)buffer16, 2);
	ASSERT(file.good());
	buffer->chan = readByte16(buffer16);

	file.read((char*)buffer32, 4);
	ASSERT(file.good());
	buffer->freq = readByte32(buffer32);

	file.read((char*)buffer32, 4);
	ASSERT(file.good());
	byteRate = readByte32(buffer32);

	file.ignore(2);  // Skipping field "block-align"
	file.read((char*)buffer16, 2);
	ASSERT(file.good());
	bitsPerSample = readByte16(buffer16);
	buffer->format = computePlayingFormat(buffer->chan, bitsPerSample);

	// Approaching data section.
	if (!file.read(magic,4) || strcmp(magic,"data") != 0) {
		if (strcmp(magic,"fact") == 0) {
			file.ignore(8);  // Skip extra options
			file.read(magic,4);
		}
		if (strcmp(magic,"data") != 0) {
			debug("\"%s\": no data section!?.\n",magic);
			clearSoundBuffer(buffer);
			file.close();
			return SSerror::SS_INVALID_FILE;
		}
	}

	// Finally, store audio data.
	file.read((char*)buffer32, 4);
	ASSERT(file.good());
	audioDataSize  = readByte32(buffer32);
	pcmData = new unsigned char[audioDataSize]();

	// WARNING: next read may be HUGE.
	file.read((char*)pcmData, audioDataSize);
	if(!file.good()) {
		debug("%s","Unable to load audio data section into memory\n");
		clearSoundBuffer(buffer);
		delete[] pcmData;
		file.close();
		return SSerror::SS_NO_MEMORY;
	}
	buffer->duration = ((float)audioDataSize) / byteRate;

	// Now create OpenAL buffer from audio data.
	alBufferData(buffer->buffer,
		     buffer->format,
		     pcmData,
		     audioDataSize,
		     buffer->freq);

	buffer->loaded = true;
	delete[] pcmData;
	file.close();

	return SSerror::SS_NO_ERROR;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
BufferBuilder::streamBufferFromWAV(StreamWAVSoundBuffer* buffer)
{
	// Auxiliary buffers.
	char magic[5];
	unsigned char buffer32[4];
	unsigned char buffer16[2];
	short bitsPerSample = 0;
	short audioFormat = 0;
	int audioDataSize = 0;
	int byteRate = 0;
	ALenum alErr = AL_NO_ERROR;
	std::ifstream* file = buffer->file;

	// Preconditions checking and stuff.
	ASSERT(file->good() && file->is_open());
	ASSERT(buffer != NULL);
	ASSERT(buffer->type == SSbuftype::SS_BUF_STREAM_WAV);
	if (alcGetCurrentContext() == NULL) {
		debugERROR("NULL OpenAL context! Aborting.\n");
		file->close();
		return SSerror::SS_INTERNAL_ERROR;
	}
	clearSoundBuffer(buffer);
	buffer->buffer = 0;

	// Parse file header.
	magic[4] = '\0';
	if (!file->read(magic,4) || strcmp(magic,"RIFF") != 0) {
		debug("%s","No RIFF header.\n");
		file->close();
		return SSerror::SS_INVALID_FILE;
	}
	file->ignore(4);  // Skipping field "chunk size"
	if (!file->read(magic,4) || strcmp(magic,"WAVE") != 0 ||
	    !file->read(magic,4) || strcmp(magic,"fmt ") != 0 ) {
		debug("%s","Not \"WAVEfmt \" format line.\n");
		file->close();
		return SSerror::SS_INVALID_FILE;
	}
	file->ignore(4);  // Skipping field "size of sub-chunk 1"
	file->read((char*)buffer16, 2);
	ASSERT(file->good());
	audioFormat = readByte16(buffer16);

	file->read((char*)buffer16, 2);
	ASSERT(file->good());
	buffer->chan = readByte16(buffer16);

	file->read((char*)buffer32, 4);
	ASSERT(file->good());
	buffer->freq = readByte32(buffer32);

	file->read((char*)buffer32, 4);
	ASSERT(file->good());
	byteRate = readByte32(buffer32);

	file->ignore(2);  // Skipping field "block-align"
	file->read((char*)buffer16, 2);
	ASSERT(file->good());
	bitsPerSample = readByte16(buffer16);
	buffer->format = computePlayingFormat(buffer->chan, bitsPerSample);

	// Approaching data section.
	if (!file->read(magic,4) || strcmp(magic,"data") != 0) {
		if (strcmp(magic,"fact") == 0) {
			file->ignore(8);  // Skip extra options
			file->read(magic,4);
		}
		if (strcmp(magic,"data") != 0) {
			debug("\"%s\": no data section!?.\n",magic);
			clearSoundBuffer(buffer);
			file->close();
			return SSerror::SS_INVALID_FILE;
		}
	}

	// Check if file size within bounds.
	file->read((char*)buffer32, 4);
	ASSERT(file->good());
	audioDataSize = readByte32(buffer32);
	if (audioDataSize < SS_MIN_STREAM_LEN) {
		debug("\nAudio data size: %d. Must be >= %d for streaming. Failing.\n",
				audioDataSize, SS_MIN_STREAM_LEN);
		clearSoundBuffer(buffer);
		file->close();
		return SSerror::SS_FILE_TOO_SMALL;
	}

	// Reference file and audio data and exit.
	buffer->duration  = ((float)audioDataSize) / byteRate;
	buffer->dataStart = file->tellg();  // Remember data starting position
	buffer->loaded = true;

	// Postconditions checking.
	ASSERT(buffer->file->good());
	ASSERT(buffer->file->is_open());
	ASSERT(!buffer->file->eof());

	return SSerror::SS_NO_ERROR;
}



////////////////////////////////////////////////////////////////////////////////
SSerror
BufferBuilder::loadedBufferFromOGG(FILE* file, SoundBuffer *buffer)
{
	const int endianness(0);  // 0 LittleEndian, 1 BigEndian. Usually 0
	const int sign(1);        // Signed(1) or unsigned(0) data. Usually 1.
	const short BPS(16);      // Bits per sample. We default to 16
	int bitStreamSection(0);
	SSerror error(SSerror::SS_INTERNAL_ERROR);
	int err(0);
	long read(0);
	char array[OGG_BUFF_SIZE];
	ALenum alErr(AL_NO_ERROR);
	vorbis_info *fileInfo(0);
	OggVorbis_File oggFile;
	std::vector<char> pcmData;

	// Preconditions checking and stuff.
	ASSERT(file);
	ASSERT(fileno(file) >= 0);
	ASSERT(buffer);
	ASSERT(buffer->type == SSbuftype::SS_BUF_LOADED);
	if (alcGetCurrentContext() == NULL) {
		debugERROR("NULL OpenAL context! Aborting.\n");
		return SSerror::SS_INTERNAL_ERROR;
	}
	clearSoundBuffer(buffer);
	alGenBuffers(1, &(buffer->buffer));
	alErr = alGetError();
	if (alErr != AL_NO_ERROR) {
		if (alErr == AL_OUT_OF_MEMORY) {
			return SSerror::SS_NO_MEMORY;
		} else {
			return SSerror::SS_INTERNAL_ERROR;
		}
	}
	ASSERT(alIsBuffer(buffer->buffer));

	// Open OGG_file from file.
	// DON'T CALL fclose(file) AFTERWARDS, ov_clear() takes care of it.
	err = ov_open_callbacks(file, &oggFile, NULL, 0l, OV_CALLBACKS_DEFAULT);
	if(err) {
		std::map<int, const char*> strErr = {{OV_EREAD, "OV_EREAD"},
											{OV_ENOTVORBIS, "OV_ENOTVORBIS"},
											{OV_EVERSION, "OV_EVERSION"},
											{OV_EBADHEADER, "OV_EBADHEADER"},
											{OV_EFAULT, "OV_EFAULT"}};
		debug("\nCan't open OGG file. Error: %s\n", strErr[err]);
		return SSerror::SS_INVALID_FILE;
	}

	// Update buffer's fields with fileInfo.
	fileInfo = ov_info(&oggFile, -1);
	buffer->chan = fileInfo->channels;
	buffer->freq = fileInfo->rate;
	buffer->format = computePlayingFormat(fileInfo->channels, BPS);
	buffer->duration = (float) ov_time_total(&oggFile, -1);

	// Extract audio data.
	read = ov_read(&oggFile, array, OGG_BUFF_SIZE, endianness, BPS/8, sign,
					&bitStreamSection);
	while (read > 0) {
		pcmData.insert(pcmData.end(), array, &array[read]);
		read = ov_read(&oggFile, array, OGG_BUFF_SIZE, endianness, BPS/8, sign,
						&bitStreamSection);
	}

	if (read < 0) {
		// Error fetching audio data from Ogg file.
		std::map<int, const char*> strErr = {{OV_HOLE, "OV_HOLE"},
											{OV_EBADLINK, "OV_EBADLINK"},
											{OV_EINVAL, "OV_EINVAL"}};
		debug("Can't read OGG file. Error: %s\n", strErr[read]);
		error = SSerror::SS_INTERNAL_ERROR;

	} else {
		// Create OpenAL buffer from audio data.
		alBufferData(buffer->buffer,
			     buffer->format,
			     &pcmData[0],
			     pcmData.size(),
			     buffer->freq);
		buffer->loaded = true;
		buffer->type   = SSbuftype::SS_BUF_LOADED;
		error = SSerror::SS_NO_ERROR;
	}

	pcmData.clear();
	ov_clear(&oggFile);  // Closes both file and oggFile
	return error;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
BufferBuilder::streamBufferFromOGG(StreamOGGSoundBuffer* buffer)
{
	int err(0);
	const short BPS(16);  // Bits per sample. We default to 16
	vorbis_info *fileInfo(0);
	FILE* file = buffer->file;

	// Preconditions checking and stuff
	ASSERT(file);
	ASSERT(fileno(file) >= 0);
	ASSERT(buffer);
	ASSERT(buffer->type == SSbuftype::SS_BUF_STREAM_OGG);
	if (alcGetCurrentContext() == NULL) {
		debugERROR("NULL OpenAL context! Aborting.\n");
		// Don' t close file here, buffer destructor will do it.
		return SSerror::SS_INTERNAL_ERROR;
	}
	clearSoundBuffer(buffer);
	buffer->buffer = 0;

	// Open OGG_file from file.
	// DON'T CALL fclose(file) AFTERWARDS, ov_clear() takes care of it.
	err = ov_open_callbacks(file, buffer->oggFile, NULL, 0l, OV_CALLBACKS_DEFAULT);
	ASSERT(!err);
	if(err) {
		std::map<int, const char*> strErr = {{OV_EREAD, "OV_EREAD"},
											{OV_ENOTVORBIS, "OV_ENOTVORBIS"},
											{OV_EVERSION, "OV_EVERSION"},
											{OV_EBADHEADER, "OV_EBADHEADER"},
											{OV_EFAULT, "OV_EFAULT"}};
		debug("\nCan't open OGG file. Error: %s\n", strErr[err]);
		return SSerror::SS_INVALID_FILE;
	}

	// Update buffer's fields with fileInfo.
	fileInfo = ov_info(buffer->oggFile, -1);
	buffer->chan = fileInfo->channels;
	buffer->freq = fileInfo->rate;
	buffer->format = computePlayingFormat(fileInfo->channels, BPS);
	buffer->duration = (float) ov_time_total(buffer->oggFile, -1);

	return SSerror::SS_NO_ERROR;
}

}
