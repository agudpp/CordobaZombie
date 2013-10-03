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
StreamWAVSoundBuffer::filler(ALBuffer& buf, size_t size, bool repeat)
{
	std::streamsize readSize(0);

	/* Try to read 'size' bytes into temp. array */
	if (file->eof()) {
		if (repeat) {
			file->seekg((long)dataStart, std::ifstream::beg);
			ASSERT(file->good() && !file->eof());
		} else {
			return 0ll;
		}
	}
	file->read(pcmData, size);

	/* EOF? */
	readSize = file->gcount();
	if (readSize >= 0 && readSize < size && repeat) {

		/* Must repeat: continue loading from the beginning. */
		file->seekg((long)dataStart, std::ifstream::beg);
		ASSERT(file->good() && !file->eof());

		file->read(&pcmData[readSize], size-readSize);
		if (readSize + file->gcount() < size) {
			debug("Couldn't extract %zu bytes from file.\n", size);
		}

		if (file->gcount() < 0) {
			debugERROR("Problems reading data from file.%s", "\n");
			return -1ll;
		} else {
			readSize += file->gcount();
		}
	} else if (readSize < 0) {
		debugERROR("Problems reading data from file.%s", "\n");
		return -1ll;
	}

	alBufferData(buf, format, pcmData, readSize, freq);

	if (alGetError() != AL_NO_ERROR) {
		return -1ll;
	}
	return ((long long int)readSize);
}


////////////////////////////////////////////////////////////////////////////////
long long int
StreamOGGSoundBuffer::filler(ALBuffer& buf, size_t size, bool repeat)
{
	const int endianness(0);  // 0 LittleEndian, 1 BigEndian. Usually 0
	const int sign(1);        // Signed(1) or unsigned(0) data. Usually 1.
	const short BPS(16);	  // Bits per sample. We default to 16
	int chunk(size < OGG_BUFF_SIZE ? ((int)size) : OGG_BUFF_SIZE);
	char* array = new char[chunk]();
	long read(0);
	int err(0);
	ALenum alErr(AL_NO_ERROR);
	SSerror error(SSerror::SS_NO_ERROR);

	/* Try to read 'size' bytes into pcmData field. */
	pcmData.clear();
	do {
		/* Read from Ogg file into temporal array */
		read = ov_read(oggFile, array, chunk, endianness, BPS/8, sign, &bitStreamSection);

		if (read > 0) {
			/* Store fetched audio data */
			pcmData.insert(pcmData.end(), array, &array[read]);
			size -= read;
			chunk = size < OGG_BUFF_SIZE ? ((int)size) : OGG_BUFF_SIZE;

		} else if (read == 0 && repeat) {
			/* End of audio data, must repeat. */
			read = ov_pcm_seek(oggFile, 0);
			bitStreamSection = 0;
			ASSERT(read==0);
			read = 1;  // continue iteration
		}

	} while (read > 0 && chunk > 0);

	if (read < 0) {
		/* Error fetching audio data from Ogg file */
		std::map<int, const char*> strErr = {{OV_HOLE, "OV_HOLE"},
											{OV_EBADLINK, "OV_EBADLINK"},
											{OV_EINVAL, "OV_EINVAL"}};
		debug("Can't read OGG file. Error: %s\n", strErr[read]);
		delete[] array;
		return -1ll;

	} else {
		/* Fill OpenAL buffer with fetched audio data */
		alBufferData(buf, format, &pcmData[0], pcmData.size(), freq);
	}

	delete[] array;
	if (alGetError() != AL_NO_ERROR) {
		return -1ll;
	}

	return ((long long int)pcmData.size());
}

}
