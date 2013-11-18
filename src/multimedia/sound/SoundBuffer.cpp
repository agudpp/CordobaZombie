/*
 * SoundBuffer.cpp
 *
 *  Created on: Jul 12, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */


#include <map>
#include <vector>
#include <fstream>
#include <cstdio>   // FILE* family
#include <vorbis/vorbisfile.h>  // OGG-vorbis parsing

#include <debug/DebugUtil.h>
#include "SoundBuffer.h"


namespace mm {

////////////////////////////////////////////////////////////////////////////////
long long int
StreamWAVSoundBuffer::filler(ALBuffer& buf,
								 size_t    size,
								 bool     repeat,
								 bool*    finish)
{
	std::streamsize readSize(0);
	size = size > pcmData.max_size() ? pcmData.max_size() : size;

	// Try to read 'size' bytes into pcmData
	pcmData.clear();
	file->read(pcmData.end(), size);
	readSize = file->gcount(); // EOF?
	pcmData.resize(readSize);
	debug("readSize: %ld\tfile->tellg(): %ld\n", readSize, file->tellg());

	if (readSize >= 0 && readSize < size && repeat) {
		// End of audio data, and must repeat: relocate file get pointer.
		debugRED("Relocating file pointer.\n");
		restart();
		debug("readSize: %ld\tfile->tellg(): %ld\n", readSize, file->tellg());

		file->read(pcmData.end(), size-readSize);
		if (readSize + file->gcount() < size) {
			debug("Couldn't extract %zu bytes from file.\n", size);
		}

		if (file->gcount() < 0) {
			debugERROR("Problems reading data from file.\n");
			return -1ll;
		} else {
			readSize += file->gcount();
			pcmData.resize(readSize);
		}

	} else if (readSize == 0) {
		// End of audio data, but no repeat.
		debugRED("End of audio data.\n");
		*finish = true;
		return 0ll;

	} else if (readSize < 0) {
		debugERROR("Problems reading data from file.\n");
		return -1ll;
	}

	// Fill OpenAL buffer with fetched audio data.
	ASSERT(alIsBuffer(buf));
	ASSERT(pcmData.size() <= SS_SIZE_INT_BUFFERS);
	alBufferData(buf, format, pcmData.begin(), pcmData.size(), freq);

	ALenum alErr = alGetError();
	if (alErr != AL_NO_ERROR) {
		std::map<int, const char*> strErr =
			{{AL_OUT_OF_MEMORY,"AL_OUT_OF_MEMORY"},
			 {AL_INVALID_NAME,"AL_INVALID_NAME"},
			 {AL_INVALID_ENUM,"AL_INVALID_ENUM"},
			 {AL_INVALID_VALUE,"AL_INVALID_VALUE"},
			 {AL_INVALID_OPERATION,"AL_INVALID_OPERATION"}};
		debugERROR("OpenAL error: %s\n", strErr[alErr]);
		return -1ll;
	}

	return ((long long int)pcmData.size());
}


////////////////////////////////////////////////////////////////////////////////
long long int
StreamOGGSoundBuffer::filler(ALBuffer& buf,
								 size_t    size,
								 bool     repeat,
								 bool*    finish)
{
	const int endianness(0);  // 0 LittleEndian, 1 BigEndian. Usually 0.
	const int sign(1);        // Signed(1) or unsigned(0) data. Usually 1.
	const short BPS(16);	  // Bits per sample. We default to 16.
	int chunk(size < OGG_BUFF_SIZE ? ((int)size) : OGG_BUFF_SIZE);
	int read(0);

	// Read 'size' bytes into pcmData.
	pcmData.clear();
	do {
		// Read 'chunk' bytes from Ogg file into pcmData.
		read = ov_read(oggFile, pcmData.end(), chunk, endianness, BPS/8, sign,
						&bitStreamSection);

		if (read > 0) {
			// Successful read from OGG file.
			pcmData.resize(pcmData.size() + read);
			size -= read;
			chunk = size < OGG_BUFF_SIZE ? ((int)size) : OGG_BUFF_SIZE;

		} else if (read == 0 && repeat) {
			// End of audio data, and must repeat: relocate file get pointer.
			debugRED("Relocating file pointer.\n");
			restart();
			read = 1;  // We must repeat, continue iteration

		} else if (read == 0) {
			// End of audio data, but no repeat.
			debugRED("End of audio data.\n");
			*finish = true;
		}

	} while (read > 0 && chunk > 0);

	if (read < 0) {
		// Error fetching audio data from Ogg file.
		std::map<int, const char*> strErr =
			{{OV_HOLE, "OV_HOLE"},
			 {OV_EINVAL, "OV_EINVAL"},
			 {OV_EBADLINK, "OV_EBADLINK"}};
		debugERROR("Can't read OGG file. Error: %s\n", strErr[read]);
		return -1ll;

	} else {
		// Fill OpenAL buffer with fetched audio data.
		ASSERT(alIsBuffer(buf));
		ASSERT(pcmData.size() <= SS_SIZE_INT_BUFFERS);
		alBufferData(buf, format, pcmData.begin(), pcmData.size(), freq);
	}

	ALenum alErr = alGetError();
	if (alErr != AL_NO_ERROR) {
		std::map<int, const char*> strErr =
			{{AL_OUT_OF_MEMORY,"AL_OUT_OF_MEMORY"},
			 {AL_INVALID_NAME,"AL_INVALID_NAME"},
			 {AL_INVALID_ENUM,"AL_INVALID_ENUM"},
			 {AL_INVALID_VALUE,"AL_INVALID_VALUE"},
			 {AL_INVALID_OPERATION,"AL_INVALID_OPERATION"}};
		debugERROR("OpenAL error: %s\n", strErr[alErr]);
		return -1ll;
	}

	return ((long long int)pcmData.size());
}

}
