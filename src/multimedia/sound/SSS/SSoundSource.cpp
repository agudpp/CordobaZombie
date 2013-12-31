/*
 * SSoundSource.cpp
 *
 *	StreamingSoundSources
 *	---------------------
 *  Abstract interface for OpenAL source manipulation.
 *  This class implements the abstract class SoundSource.
 *  >>> PLAYS BUFFERS USING A STREAMING MECHANISM <<<
 *
 *  Created on: Jun 11, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */


#include <fstream>
#include "SSoundSource.h"
#include <debug/DebugUtil.h>


#ifndef DEBUG
#  define  ALsrcState(s)
#else
#  define  ALsrcState(s)  ALsourceStateToStr(s)
static inline const char*
ALsourceStateToStr(ALint st)
{
	switch (st)
	{
	case AL_SOURCE_STATE:
		return "AL_SOURCE_STATE";
		break;
	case AL_INITIAL:
		return "AL_INITIAL";
		break;
	case AL_PLAYING:
		return "AL_PLAYING";
		break;
	case AL_PAUSED:
		return "AL_PAUSED";
		break;
	case AL_STOPPED:
		return "AL_STOPPED";
		break;
	default:
		return "<unknown al_source state>";
		break;
	}
	return "";
}
#endif


namespace mm {

////////////////////////////////////////////////////////////////////////////////
SSoundSource::SSoundSource() :
	SoundSource(SSsrctype::SS_SRC_STREAM),
	mFileOffset(0),
	mIntBuffersSize(SS_SIZE_INT_BUFFERS),
	mFirstBuffer(0),
	mFileFinished(false)
{
	// Generate internal buffers.
	alGenBuffers(SS_NUM_INT_BUFFERS, mIntBuffers);
#ifdef DEBUG
	ASSERT(alGetError() == AL_NO_ERROR);
	for (int i=0 ; i < SS_NUM_INT_BUFFERS ; i++) {
		ASSERT(alIsBuffer(mIntBuffers[i]));
	}
#endif
	mRepeat = false;
	ASSERT(alGetError() == AL_NO_ERROR);
}


////////////////////////////////////////////////////////////////////////////////
SSoundSource::~SSoundSource()
{
	ALint st;

	ASSERT(alGetError() == AL_NO_ERROR);
	ASSERT(alIsSource(mSource));

	alGetSourcei(mSource, AL_SOURCE_STATE, &st);
	if (st == AL_PAUSED || st == AL_PLAYING) {
		debugWARNING("%s","Source killed during playback\n");
		alSourceStop(mSource);
	}
	alDeleteSources(1,&mSource);
	alDeleteBuffers(SS_NUM_INT_BUFFERS, mIntBuffers);

	ASSERT(alGetError() == AL_NO_ERROR);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SSoundSource::play(SoundBuffer* buf,
				   const Ogre::Real& gain,
				   const Ogre::Vector3& pos,
				   bool  repeat)
{
	ALint st(0);
	int processed(0);
	unsigned int i(0);
	long long int readSize(mIntBuffersSize);

	ASSERT(alGetError() == AL_NO_ERROR);
	ASSERT(alIsSource(mSource));
	alGetSourcei(mSource, AL_SOURCE_STATE, &st);

	// Void call?
	if (!buf) {
		if (!mBuffer) {
			debugERROR("NULL buffer, cannot start playback.\n");
			return SSerror::SS_NO_BUFFER;

		} else if (st == AL_PAUSED) {
			// Resume paused playback.
			alSourcePlay(mSource);
			this->update(pos);  // Use this opportunity to update intBuffers
			return SSerror::SS_NO_ERROR;

		} else if (st == AL_PLAYING) {
			this->update(pos);  // Use this opportunity to update intBuffers
			return SSerror::SS_NO_ERROR;

		} else {
			debugERROR("Invalid AL source state (%s), cannot start playback.\n",
					ALsrcState(st));
			return SSerror::SS_INTERNAL_ERROR;
		}

	// Resume paused playback?
	} else if (buf == mBuffer) {
		if (st == AL_PAUSED) {
			alSourcePlay(mSource);
		}
		this->update(pos);  // Use this opportunity to update intBuffers
		return SSerror::SS_NO_ERROR;
	}

	// New buffer passed. Stop any previous playback.
	if (st == AL_PLAYING || st == AL_PAUSED) {
		alSourceStop(mSource);
	}
	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);
	while (processed-- > 0) {
		alSourceUnqueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
		++mFirstBuffer %= SS_NUM_INT_BUFFERS;  // I <3 THIS LINES
		// Erase buffer data? How?
	}
	ASSERT(alGetError() == AL_NO_ERROR);

	// Start at the beggining of the file
	buf->restart();
	mFileFinished = false;

	// Fill internal buffers
	for (i=0 ; i < SS_NUM_INT_BUFFERS && readSize > 0 && !mFileFinished ; i++) {
		readSize = buf->filler(mIntBuffers[i], mIntBuffersSize, repeat, &mFileFinished);
	}
	if (readSize < 0) {
		debugRED("Error filling internal buffer #%d\n", i-1);
		return SSerror::SS_INTERNAL_ERROR;
	}

	// Queue filled buffers.
	alSourceQueueBuffers(mSource, i, mIntBuffers);
	mBuffer = buf;
	mFirstBuffer = 0;
#ifdef DEBUG
	ASSERT(alGetError() == AL_NO_ERROR);
	alGetSourcei(mSource, AL_SOURCE_TYPE, &st);
	ASSERT(st==AL_STREAMING);
#endif

	// Set current position, gain and repeat.
	alSource3f(mSource, AL_POSITION, pos.x, pos.y, pos.z);
	alSourcef(mSource, AL_GAIN, gain);
	mRepeat = repeat;
	// Play modefoke, PLAY!
	alSourcePlay(mSource);

	ASSERT(alGetError() == AL_NO_ERROR);
	return SSerror::SS_NO_ERROR;
}


////////////////////////////////////////////////////////////////////////////////
SSplayback
SSoundSource::update(const Ogre::Vector3& pos)
{
	bool restart(false);
	ALint st(AL_NONE);
	int processed(0);
	int readSize(mIntBuffersSize);
	SSplayback result(SSplayback::SS_NONE);

	ASSERT(alGetError() == AL_NO_ERROR);
	ASSERT(alIsSource(mSource));

	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);
	alGetSourcei(mSource, AL_SOURCE_STATE, &st);
	if (st == AL_STOPPED && processed) {
		// Stopped for lack of buffers. Restart quickly!
		restart = true;
	} else if (st == AL_STOPPED) {
		// Playback finished.
	    ASSERT(alGetError() == AL_NO_ERROR);
		result = SSplayback::SS_FINISHED;
		goto finish;
	}

	// If source is not relative to listener, then update position.
	alGetSourcei(mSource, AL_SOURCE_RELATIVE, &st);
	if (!st) {
		alSource3f(mSource, AL_POSITION, pos.x, pos.y, pos.z);
	}

	// Refresh used internal buffers
	while (!mFileFinished && readSize > 0 && processed-- > 0) {
		// Unqueue processed buffer.
		alSourceUnqueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
		// Refill buffer.
		readSize = mBuffer->filler(mIntBuffers[mFirstBuffer],
									mIntBuffersSize,
									mRepeat,
									&mFileFinished);
		// Queue back, if properly refilled.
		if (readSize > 0) {
			alSourceQueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
		}
		// Increment buffer pointer if there were no errors.
		if (readSize >= 0) {
			++mFirstBuffer %= SS_NUM_INT_BUFFERS;
		}
	}

	// Check if we need to restart (after queueing due to Windows issue)
    if (restart) {
        alSourcePlay(mSource);
        restart = false;
    }

	// Check for errors.
	alGetSourcei(mSource, AL_SOURCE_STATE, &st);
	if (mFileFinished && st == AL_STOPPED) {
		// Playback finished.
		result = SSplayback::SS_FINISHED;

	} else if (readSize >= 0){
		// Keep on rollin' baby.
		alGetSourcei(mSource, AL_SOURCE_STATE, &st);
		result = ((st==AL_PLAYING)  ? SSplayback::SS_PLAYING
									: SSplayback::SS_PAUSED);
	} else {
		debugWARNING("Error filling internal buffer #%d\n",
				((mFirstBuffer-1)%SS_NUM_INT_BUFFERS));
		// Attempt one final buffer refresh before failing.
		debugYELLOW("Trying to recover.\n");
		// mIntBuffers[mFirstBuffer] is the problematic buffer,
		// and hasn't been re-queued yet.
		readSize = mBuffer->filler(mIntBuffers[mFirstBuffer],
									mIntBuffersSize,
									mRepeat,
									&mFileFinished);
		if (readSize > 0) {
			debugGREEN("Successfull recovery!\n");
			alSourceQueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
			if (restart) { alSourcePlay(mSource); restart = false; }
			++mFirstBuffer %= SS_NUM_INT_BUFFERS;
			alGetSourcei(mSource, AL_SOURCE_STATE, &st);
			result = ((st==AL_PLAYING)  ? SSplayback::SS_PLAYING
										: SSplayback::SS_PAUSED);
		} else {
			debugERROR("Error persists, can not fill internal buffer.\n");
			result = SSplayback::SS_FINISHED;
			goto fail;
		}
	}

//  success
	finish:
        switch (alGetError())
        {
        case AL_NO_ERROR:
            break;
        case AL_INVALID_NAME:
            debugRED("AL_INVALID_NAME\n");
            break;
        case AL_INVALID_OPERATION:
            debugRED("AL_INVALID_OPERATION\n");
            break;
        case AL_INVALID_VALUE:
            debugRED("AL_INVALID_VALUE\n");
            break;
        case AL_OUT_OF_MEMORY:
            debugRED("AL_OUT_OF_MEMORY\n");
            break;
        default:
            debugRED("Another OpenAL error\n");
            break;
        }
		while (processed-- > 0) {
			// Playing finished, but there are still buffers to unqueue.
			alSourceUnqueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
			++mFirstBuffer %= SS_NUM_INT_BUFFERS;
		}
		switch (alGetError())
		{
        case AL_NO_ERROR:
            break;
        case AL_INVALID_NAME:
            debugRED("AL_INVALID_NAME\n");
            break;
        case AL_INVALID_OPERATION:
            debugRED("AL_INVALID_OPERATION\n");
            break;
        case AL_INVALID_VALUE:
            debugRED("AL_INVALID_VALUE\n");
            break;
        case AL_OUT_OF_MEMORY:
            debugRED("AL_OUT_OF_MEMORY\n");
            break;
        default:
            debugRED("Another OpenAL error\n");
            break;
		}
		ASSERT(alGetError() == AL_NO_ERROR);
		return result;
	fail:
		alSourceStop(mSource);
		alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);
		while (processed-- > 0) {
			alSourceUnqueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
			++mFirstBuffer %= SS_NUM_INT_BUFFERS;
		}
		ASSERT(alGetError() == AL_NO_ERROR);
		return result;
}


////////////////////////////////////////////////////////////////////////////////
void
SSoundSource::pause()
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
SSoundSource::stop()
{
	ALint processed(0);

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

	// Unqueue buffers.
	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);
	while (processed-- > 0) {
		alSourceUnqueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
		++mFirstBuffer %= SS_NUM_INT_BUFFERS;
	}
	ASSERT(alGetError() == AL_NO_ERROR);

	// Set NULL buffer.
	alSourcei(mSource, AL_BUFFER, AL_NONE);
#ifdef DEBUG
	{
		ALint buf(AL_NONE);
		alGetSourcei(mSource, AL_BUFFER, &buf);
		ASSERT(buf == AL_NONE);
	}
#endif

	mFirstBuffer = 0u;
	mBuffer = 0;
	ASSERT(alGetError() == AL_NO_ERROR);
	return;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SSoundSource::restart(const Ogre::Real& gain,
					  const Ogre::Vector3& pos,
					  const bool repeat)
{
	int i(0);
	ALint st(AL_NONE),
		  relative(AL_NONE),
		  processed(0);
	int readSize(mIntBuffersSize);

	ASSERT(alGetError() == AL_NO_ERROR);
	ASSERT(alIsSource(mSource));

	if (!mBuffer) {
		return SSerror::SS_NO_BUFFER;
	}

	// Stop and clean any current playback.
	alSourceStop(mSource);
	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);
	while (processed-- > 0) {
		alSourceUnqueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
		++mFirstBuffer %= SS_NUM_INT_BUFFERS;
	}

	// Fill internal buffers (according to file type).
	mBuffer->restart();
	mFileFinished = false;
	mFirstBuffer = 0;
	for (i=0 ; i < SS_NUM_INT_BUFFERS && readSize==mIntBuffersSize ; i++) {
		readSize = mBuffer->filler(mIntBuffers[i], mIntBuffersSize, mRepeat, &mFileFinished);
	}

	if (readSize < 0) {
		debugRED("Error filling internal buffer #%d\n", i-1);
		return SSerror::SS_INTERNAL_ERROR;
	}

	// Queue filled buffers.
	alSourceQueueBuffers(mSource, i, mIntBuffers);
	mFirstBuffer = 0;
#ifdef DEBUG
	ASSERT(alGetError() == AL_NO_ERROR);
	alGetSourcei(mSource, AL_SOURCE_TYPE, &st);
	ASSERT(st==AL_STREAMING);
#endif

	// Set current position, gain and repeat
	alGetSourcei(mSource, AL_SOURCE_RELATIVE, &st);
	if (!st) {
		alSource3f(mSource, AL_POSITION, pos.x, pos.y, pos.z);
	}  // else: source relative to listener, no position update needed
	alSourcef(mSource, AL_GAIN, gain);
	mRepeat = repeat;

	// Finally restart the goddamn thing.
	alSourcePlay(mSource);

	ASSERT(alGetError() == AL_NO_ERROR);
	return SSerror::SS_NO_ERROR;
}

}

