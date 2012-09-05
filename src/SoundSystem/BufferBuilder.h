/*
 * BufferBuilder.h
 *
 *  Helper module.
 *  Abstraction of a SoundBuffer creation from an audio file.
 *
 *  Created on: May 22, 2012
 *     Company: estrategia201Z
 *      Author: Budde, Carlos Esteban.
 */

#ifndef BUFFERBUILDER_H_
#define BUFFERBUILDER_H_

#include <fstream>
#include "SoundBuffer.h"
#include "SoundEnums.h"

class BufferBuilder
{
public:
	/**
	 ** @brief
	 ** Opens audio file named "sName" and fills "buffer" with the content.
	 **
	 ** @params
	 **  sName: name of the audio file (absolut path in filesystem)
	 ** buffer: SoundBuffer to create and fill. MUST POINT TO NULL.
	 ** format: file's audio compression format (WAV, OGG, MP3)
	 **   type: buffer's type (streaming vs mem.loaded)
	 **
	 ** @remarks
	 ** An OpenAL context must exist before calling this function.
	 ** sName must be the absolut path of a readable file.
	 ** The filled SoundBuffer is left ready for OpenAL playback.
	 ** If file's compression format (i.e. WAV, OGG, MP3) is not specified,
	 ** it's guessed from file's extension.
	 ** Default buffer type is SS_BUF_LOADED (i.e. load all audio data in mem)
	 **
	 ** @return
	 ** SS_NO_ERROR			Buffer successfully loaded.
	 ** SS_NO_MEMORY		Not enough memory to work with. Go buy some.
	 ** SS_INVALID_FILE		Unsupported/erroneous file audio format.
	 ** SS_FILE_TOO_SMALL	File ain't big enough for streaming.
	 ** SS_FILE_NOT_FOUND	Inexistent file.
	 ** SS_INTERNAL_ERROR	Something went wrong. Does an OpenAL context exist?
	 **/
	static SSerror
	bufferFromFile(const std::string& sName,
					SoundBuffer** buffer,
					SSformat format,
					SSbuftype type);

private:
	/**
	 ** @brief
	 ** Parse "file", which should be a valid (and open) WAV audio file,
	 ** and fill in "buffer" with the extracted data.
	 **
	 ** @remarks
	 ** An OpenAL context must exist before calling this function.
	 ** "file"   must be a valid ifstream built from a WAV audio file.
	 ** "file"   is closed after use, regardless of success.
	 ** "buffer" must not be NULL.
	 ** If "buffer" contained some data, it gets erased.
	 **
	 ** @return
	 ** SS_NO_ERROR			Buffer successfully loaded.
	 ** SS_NO_MEMORY		Not enough memory to work with. Go buy some.
	 ** SS_INVALID_FILE		Erroneous file audio format.
	 ** SS_FILE_NOT_FOUND	Bad file.
	 ** SS_INTERNAL_ERROR	Something went wrong. Does an OpenAL context exist?
	 **/
	static SSerror
	loadedBufferFromWAV(std::ifstream& file, SoundBuffer *buffer);

	/**
	 ** @brief
	 ** Parse "buffer"'s internal file, which should be a valid (and open)
	 ** WAV audio file, and fill "buffer" internal parameters accordingly.
	 **
	 ** @remarks
	 ** An OpenAL context must exist before calling this function.
	 ** The buffer's internal field "file" must point towards a valid ifstream
	 ** built from a WAV audio file.
	 ** Such file remains open on success, and must not be closed externally.
	 ** "buffer" must not be NULL.
	 ** If "buffer" contained some data, it gets erased.
	 **
	 ** @return
	 ** SS_NO_ERROR			Buffer successfully loaded.
	 ** SS_NO_MEMORY		Not enough memory to work with. Go buy some.
	 ** SS_INVALID_FILE		Erroneous file audio format.
	 ** SS_FILE_NOT_FOUND	Bad file.
	 ** SS_INTERNAL_ERROR	Something went wrong. Does an OpenAL context exist?
	 **/
	static SSerror
	streamBufferFromWAV(StreamWAVSoundBuffer *buffer);

	/**
	 ** @brief
	 ** Parse "file", which should be a valid (and open) OGG-vorbis audio file,
	 ** and fill in "buffer" with the extracted data.
	 **
	 ** @remarks
	 ** An OpenAL context must exist before calling this function.
	 ** "file"   must be a valid file pointer to an (open) OGG audio file.
	 ** "file"   is closed after use, regardless of success.
	 ** "buffer" must not be NULL.
	 ** If "buffer" contained some data, it gets erased.
	 **
	 ** @return
	 ** SS_NO_ERROR			Buffer successfully loaded.
	 ** SS_NO_MEMORY		Not enough memory to work with. Go buy some.
	 ** SS_INVALID_FILE		Erroneous file audio format.
	 ** SS_FILE_NOT_FOUND	Bad file pointer.
	 ** SS_INTERNAL_ERROR	Something went wrong. Does an OpenAL context exist?
	 **/
	static SSerror
	loadedBufferFromOGG(FILE* file, SoundBuffer *buffer);

	/**
	 ** @brief
	 ** Parse "buffer"'s internal file, which should be a valid (and open)
	 ** OGG-vorbis audio file, and fill "buffer" internal parameters accordingly
	 **
	 ** @remarks
	 ** An OpenAL context must exist before calling this function.
	 ** The buffer's internal field "file" must be a valid FILE* to an
	 ** (open) OGG audio file.
	 ** Such file remains open on success, and must not be closed externally.
	 ** "buffer" must not be NULL.
	 ** If "buffer" contained some data, it gets erased.
	 **
	 ** @return
	 ** SS_NO_ERROR			Buffer successfully loaded.
	 ** SS_NO_MEMORY		Not enough memory to work with. Go buy some.
	 ** SS_INVALID_FILE		Erroneous file audio format.
	 ** SS_FILE_NOT_FOUND	Bad file pointer.
	 ** SS_INTERNAL_ERROR	Something went wrong. Does an OpenAL context exist?
	 **/
	static SSerror
	streamBufferFromOGG(StreamOGGSoundBuffer *buffer);
};


#endif /* BUFFERBUILDER_H_ */

