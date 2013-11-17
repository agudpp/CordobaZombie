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

	// Check if we're at EOF
	if (file->eof()) {
		if (repeat) {
			file->seekg((long)dataStart, std::ifstream::beg);
			ASSERT(file->good() && !file->eof());
		} else {
			*finish = true;
			return 0ll;
		}
	}

	// Try to read 'size' bytes into temp. array
	size = size > SS_SIZE_INT_BUFFERS ? SS_SIZE_INT_BUFFERS : size;
	file->read(pcmData.begin(), size);
	readSize = file->gcount(); // EOF?

	if (readSize >= 0 && readSize < size && repeat) {
		// Must repeat: continue loading from the beginning.
		file->seekg((long)dataStart, std::ifstream::beg);
		ASSERT(file->good() && !file->eof());

		file->read(&pcmData[readSize], size-readSize);
		if (readSize + file->gcount() < size) {
			debug("Couldn't extract %zu bytes from file.\n", size);
		}

		if (file->gcount() < 0) {
			debugERROR("Problems reading data from file.\n");
			return -1ll;
		} else {
			readSize += file->gcount();
		}

	} else if (readSize < 0) {
		debugERROR("Problems reading data from file.\n");
		return -1ll;

	} else if (readSize == 0) {
		*finish = true;
	}

	alBufferData(buf, format, pcmData.begin(), readSize, freq);

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

	return ((long long int)readSize);
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

	ASSERT(alIsBuffer(buf));

	// Try to read 'size' bytes into pcmData field.
	pcmData.clear();
	do {
		// Read from Ogg file into temporal array.
		read = ov_read(oggFile, pcmData.end(), chunk, endianness, BPS/8, sign,
						&bitStreamSection);
//		TODO: erase old way (with std::vector<char>)
//		read = ov_read(oggFile, array, chunk, endianness, BPS/8, sign, &bitStreamSection);

		if (read > 0) {
			// Successful read from OGG file.
			pcmData.resize(pcmData.size() + read);
			size -= read;
			chunk = size < OGG_BUFF_SIZE ? ((int)size) : OGG_BUFF_SIZE;
//			TODO: erase old way (with std::vector<char>)
//			pcmData.insert(pcmData.end(), array, &array[read]);

		} else if (read == 0 && repeat) {
			// End of audio data, and must repeat: relocate file get pointer.
			restart();
			debugRED("Rellocating file pointer.\n");
			read = 1;  // We must repeat, continue iteration

		} else if (read == 0) {
			// End of audio data, but no repeat.
			debugRED("Audio file ended playback.\n");
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
