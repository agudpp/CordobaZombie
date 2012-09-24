/*
 * LSoundSource.cpp
 *
 *	LoadedSoundSources
 *	------------------
 *  Abstract interface for OpenAL source manipulation.
 *  This class implements the abstract class SoundSource.
 *  >>> PLAYS BUFFERS ENTIRELY LOADED INTO MEMORY <<<
 *
 *  Created on: Jun 11, 2012
 *     Company: estrategia201Z
 *      Author: Budde, Carlos Esteban.
 */


#include "LSoundSource.h"
#include "DebugUtil.h"

////////////////////////////////////////////////////////////////////////////////
LSoundSource::LSoundSource() : SoundSource()
{ /* Inherited constructor suffices. */ }


////////////////////////////////////////////////////////////////////////////////
LSoundSource::~LSoundSource()
{
	ALint st;

	ASSERT(alGetError() == AL_NO_ERROR);
	ASSERT(alIsSource(mSource));

	alGetSourcei(mSource, AL_SOURCE_STATE, &st);
	if (st == AL_PAUSED || st == AL_PLAYING) {
		debugRED("%s","Source killed during playback\n");
		alSourceStop(mSource);
	}
	alDeleteSources(1,&mSource);
	ASSERT(alGetError() == AL_NO_ERROR);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
LSoundSource::play(SoundBuffer* buf,
				   const Ogre::Vector3& pos,
				   bool  repeat)
{
	ALint st(0);

	ASSERT(alGetError() == AL_NO_ERROR);
	ASSERT(alIsSource(mSource));
	alGetSourcei(mSource, AL_SOURCE_STATE, &st);

	/* Void call? */
	if (!buf) {
		if (!mBuffer) {
			debug("Invalid buffer, cannot start playback.\n");
			return SSerror::SS_NO_BUFFER;

		} else if (st == AL_PAUSED) {
			alSourcePlay(mSource);
			return SSerror::SS_NO_ERROR;

		} else if (st == AL_PLAYING) {
			return SSerror::SS_NO_ERROR;

		} else {
			debugRED("Invalid state, cannot start playback.\n");
			return SSerror::SS_INTERNAL_ERROR;
		}

	/* Resume paused playback? */
	} else if (buf == mBuffer) {
		if (st == AL_PAUSED) {
			alSourcePlay(mSource);
		}
		return SSerror::SS_NO_ERROR;

	/* New buffer passed. */
	}  else if (!alIsBuffer(buf->buffer)
				|| buf->type != SSbuftype::SS_BUF_LOADED) {
		return SSerror::SS_NO_BUFFER;
	}

	/* Stop any previous playback. */
	if (st == AL_PLAYING || st == AL_PAUSED) {
		alSourceStop(mSource);
	}

	/* Attach new buffer */
	alSourcei(mSource, AL_BUFFER, buf->buffer);
	ASSERT(alGetError() == AL_NO_ERROR);
	mBuffer = buf;
	mRepeat = repeat;
#ifdef DEBUG
	{
		ALint st(0);
		alGetSourcei(mSource, AL_SOURCE_TYPE, &st);
		ASSERT(st==AL_STATIC);
	}
#endif

	/* Set current position and repeat */
	alSource3f(mSource, AL_POSITION, pos.x, pos.y, pos.z);
	alSourcei(mSource, AL_LOOPING, repeat);
	/* Play modefoke, PLAY! */
	alSourcePlay(mSource);

	ASSERT(alGetError() == AL_NO_ERROR);
	return SSerror::SS_NO_ERROR;
}


////////////////////////////////////////////////////////////////////////////////
SSplayback
LSoundSource::update(const Ogre::Vector3& pos)
{
	SSplayback result = SSplayback::SS_FINISHED;
	ALint st(AL_NONE);

	ASSERT(alGetError() == AL_NO_ERROR);
	ASSERT(alIsSource(mSource));

	alGetSourcei(mSource, AL_SOURCE_STATE, &st);
	if (st == AL_PLAYING) {
		/* Update current position */
		alGetSourcei(mSource, AL_SOURCE_RELATIVE, &st);
		if (!st) {
			alSource3f(mSource, AL_POSITION, pos.x, pos.y, pos.z);
		}  // else: source relative to listener, no position update needed
		result = SSplayback::SS_PLAYING;

	} else if (st == AL_PAUSED) {
		result = SSplayback::SS_PAUSED;

	} else {
		/* Must be a "finished" source. Detach buffer. */
		ASSERT(st == AL_STOPPED);
		ASSERT(!mRepeat);
		alSourcei(mSource, AL_BUFFER, AL_NONE);
		mBuffer = NULL;
	}

	ASSERT(alGetError() == AL_NO_ERROR);
	return result;
}


////////////////////////////////////////////////////////////////////////////////
void
LSoundSource::pause()
{
	ASSERT(alGetError() == AL_NO_ERROR);
	ASSERT(alIsSource(mSource));
	alSourcePause(mSource);
#ifdef DEBUG
	{
		ALint st(0);
		alGetSourcei(mSource, AL_SOURCE_STATE, &st);
		ASSERT(st == AL_PAUSED);
	}
#endif
}


////////////////////////////////////////////////////////////////////////////////
void
LSoundSource::stop()
{
	ASSERT(alGetError() == AL_NO_ERROR);
	ASSERT(alIsSource(mSource));
	alSourceStop(mSource);
#ifdef DEBUG
	{
		ALint st(AL_NONE);
		alGetSourcei(mSource, AL_SOURCE_STATE, &st);
		ASSERT(st == AL_STOPPED);
	}
#endif
	/* Detach buffer */
	alSourcei(mSource, AL_BUFFER, AL_NONE);
	mBuffer = 0;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
LSoundSource::restart(const Ogre::Vector3& pos)
{
	ALint st(AL_NONE);
	SSerror result = SSerror::SS_INTERNAL_ERROR;

	ASSERT(alGetError() == AL_NO_ERROR);
	ASSERT(alIsSource(mSource));

	if (!mBuffer) {
		result = SSerror::SS_NO_BUFFER;

	} else {
		alGetSourcei(mSource, AL_SOURCE_RELATIVE, &st);
		if (!st) {
			alSource3f(mSource, AL_POSITION, pos.x, pos.y, pos.z);
		}  // else: source relative to listener, no position update needed
		alGetSourcei(mSource, AL_SOURCE_STATE, &st);
		if (st == AL_PLAYING) {
			alSourcePlay(mSource);
			result = SSerror::SS_NO_ERROR;

		} else if (st == AL_PAUSED) {
			alSourcePlay(mSource);
			alSourcePlay(mSource);
			result = SSerror::SS_NO_ERROR;

		} else {  // st == AL_STOPPED || st == AL_INITIAL
			debugRED("Invalid state, cannot restart playback.\n");
			result = SSerror::SS_INTERNAL_ERROR;
		}
	}

	ASSERT(alGetError() == AL_NO_ERROR);
	return result;
}
