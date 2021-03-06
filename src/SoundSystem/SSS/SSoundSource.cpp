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
#include "DebugUtil.h"


////////////////////////////////////////////////////////////////////////////////
SSoundSource::SSoundSource() :
	SoundSource(SSsrctype::SS_SRC_STREAM),
	mRepeat(false),
	mFileOffset(0),
	mIntBuffersSize(SS_SIZE_INT_BUFFERS),
	mFirstBuffer(0)
{
	/* Generate internal buffers. */
	alGenBuffers(SS_NUM_INT_BUFFERS, mIntBuffers);
#ifdef DEBUG
	ASSERT(alGetError() == AL_NO_ERROR);
	for (int i=0 ; i < SS_NUM_INT_BUFFERS ; i++) {
		ASSERT(alIsBuffer(mIntBuffers[i]));
	}
#endif

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
		debugRED("%s","Source killed during playback\n");
		alSourceStop(mSource);
	}
	alDeleteSources(1,&mSource);
	alDeleteBuffers(SS_NUM_INT_BUFFERS, mIntBuffers);

	ASSERT(alGetError() == AL_NO_ERROR);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SSoundSource::play(SoundBuffer* buf,
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

	/* Void call? */
	if (!buf) {
		if (!mBuffer) {
			debug("NULL buffer, cannot start playback.\n");
			return SSerror::SS_NO_BUFFER;

		} else if (st == AL_PAUSED) {
			/* Resume paused playback */
			alSourcePlay(mSource);
			this->update(pos);  // Use this opportunity to update intBuffers
			return SSerror::SS_NO_ERROR;

		} else if (st == AL_PLAYING) {
			this->update(pos);  // Use this opportunity to update intBuffers
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
		this->update(pos);  // Use this opportunity to update intBuffers
		return SSerror::SS_NO_ERROR;
	}

	/* New buffer passed. Stop any previous playback. */
	if (st == AL_PLAYING || st == AL_PAUSED) {
		alSourceStop(mSource);
	}
	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);
	while (processed-- > 0) {
		alSourceUnqueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
		++mFirstBuffer %= SS_NUM_INT_BUFFERS;  // I <3 THIS LINES
		// Erase buffer data? How?
	}

	/* Fill internal buffers (according to file type). */
	for (i=0 ; i < SS_NUM_INT_BUFFERS && readSize==mIntBuffersSize ; i++) {
		readSize = buf->filler(mIntBuffers[i], mIntBuffersSize, repeat);
	}
	if (readSize < 0) {
		debugRED("Error filling internal buffer #%d\n", i-1);
		return SSerror::SS_INTERNAL_ERROR;
	}

	/* Queue filled buffers. */
	alSourceQueueBuffers(mSource, i, mIntBuffers);
	mBuffer = buf;
	mFirstBuffer = 0;
#ifdef DEBUG
	ASSERT(alGetError() == AL_NO_ERROR);
	alGetSourcei(mSource, AL_SOURCE_TYPE, &st);
	ASSERT(st==AL_STREAMING);
#endif

	/* Set current position and repeat */
	alSource3f(mSource, AL_POSITION, pos.x, pos.y, pos.z);
	mRepeat = repeat;
	/* Play modefoke, PLAY! */
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

	ASSERT(alGetError() == AL_NO_ERROR);
	ASSERT(alIsSource(mSource));

	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);
	alGetSourcei(mSource, AL_SOURCE_STATE, &st);
	if (st == AL_STOPPED && processed) {
		/* Stopped for lack of buffers. Restart quickly! */
		restart = true;
	} else if (st == AL_STOPPED) {
		/* Playback finished. */
		goto finish;
	}

	/* Update position */
	alGetSourcei(mSource, AL_SOURCE_RELATIVE, &st);
	if (!st) {
		alSource3f(mSource, AL_POSITION, pos.x, pos.y, pos.z);
	}  // else: source relative to listener, no position update needed

	/* Refresh used internal buffers */
	while (readSize==mIntBuffersSize && processed-- > 0) {
		/* Unqueue processed buffer. */
		alSourceUnqueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);

		/* Refill buffer. */
		readSize = mBuffer->filler(mIntBuffers[mFirstBuffer], mIntBuffersSize, mRepeat);

		/* Queue back, if propperly refilled. */
		if (readSize > 0) {
			alSourceQueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
			if (restart) { alSourcePlay(mSource); restart = false; }
			++mFirstBuffer %= SS_NUM_INT_BUFFERS;
		}
	}

	/* Check for errors. */
	if (readSize < 0) {
		debugRED("Error filling internal buffer #%d\n",
				((mFirstBuffer-1)%SS_NUM_INT_BUFFERS));

		/* Attempt one final buffer refresh before failing. */
		debugYELLOW("Trying to recover.%s", "\n");

		/* mIntBuffers[mFirstBuffer] is the problematic buffer, *
		 * and hasn't been re-queued yet.                       */
		readSize = mBuffer->filler(mIntBuffers[mFirstBuffer],
									mIntBuffersSize, mRepeat);
		if (readSize > 0) {
			debugGREEN("Successfull recovery!%s", "\n");
			alSourceQueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
			if (restart) { alSourcePlay(mSource); restart = false; }
			++mFirstBuffer %= SS_NUM_INT_BUFFERS;
		} else {
			debugERROR("Error persists, can not fill internal buffer.%s", "\n");
			goto fail;
		}
	}

	while (processed-- > 0) {
		/* Playing finished, but there are still buffers to unqueue. */
		alSourceUnqueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
		++mFirstBuffer %= SS_NUM_INT_BUFFERS;
	}

//	success:
		ASSERT(alGetError() == AL_NO_ERROR);
		alGetSourcei(mSource, AL_SOURCE_STATE, &st);
		return ((st==AL_PLAYING) ? SSplayback::SS_PLAYING
								 : SSplayback::SS_PAUSED);
	fail:
		alSourceStop(mSource);
		alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);
		while (processed-- > 0) {
			alSourceUnqueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
			++mFirstBuffer %= SS_NUM_INT_BUFFERS;
		}
	finish:
		alSourcei(mSource, AL_BUFFER, AL_NONE);
		mFirstBuffer = 0u;
		mBuffer = 0;
		ASSERT(alGetError() == AL_NO_ERROR);
		return SSplayback::SS_FINISHED;
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
	/* Unqueue buffers. */
	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);
	while (processed-- > 0) {
		alSourceUnqueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
		++mFirstBuffer %= SS_NUM_INT_BUFFERS;
	}
	mBuffer->restart();
#ifdef DEBUG
	{
		ALint st(AL_NONE);
		alGetSourcei(mSource, AL_SOURCE_STATE, &st);
		ASSERT(st == AL_STOPPED);
	}
#endif
	/* Set NULL buffer. */
	alSourcei(mSource, AL_BUFFER, AL_NONE);
	mFirstBuffer = 0u;
	mBuffer = 0;
	ASSERT(alGetError() == AL_NO_ERROR);
	return;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SSoundSource::restart(const Ogre::Vector3& pos)
{
	int i(0);
	ALint st(AL_NONE),
		  relative(AL_NONE),
		  processed(0);
	int readSize(mIntBuffersSize);
	SSerror result = SSerror::SS_INTERNAL_ERROR;

	ASSERT(alGetError() == AL_NO_ERROR);
	ASSERT(alIsSource(mSource));

	if (!mBuffer) {
		return SSerror::SS_NO_BUFFER;
	}

	/* Stop and clean any current playback. */
	alSourceStop(mSource);
	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);
	while (processed-- > 0) {
		alSourceUnqueueBuffers(mSource, 1, &mIntBuffers[mFirstBuffer]);
		++mFirstBuffer %= SS_NUM_INT_BUFFERS;
	}

	/* Fill internal buffers (according to file type). */
	mBuffer->restart();
	mFirstBuffer = 0;
	for (i=0 ; i < SS_NUM_INT_BUFFERS && readSize==mIntBuffersSize ; i++) {
		readSize = mBuffer->filler(mIntBuffers[i], mIntBuffersSize, mRepeat);
	}

	if (readSize < 0) {
		debugRED("Error filling internal buffer #%d\n", i-1);
		return SSerror::SS_INTERNAL_ERROR;
	}

	/* Queue filled buffers. */
	alSourceQueueBuffers(mSource, i, mIntBuffers);
	mFirstBuffer = 0;
#ifdef DEBUG
	ASSERT(alGetError() == AL_NO_ERROR);
	alGetSourcei(mSource, AL_SOURCE_TYPE, &st);
	ASSERT(st==AL_STREAMING);
#endif

	/* Set current position and restart the goddamn thing. */
	alGetSourcei(mSource, AL_SOURCE_RELATIVE, &st);
	if (!st) {
		alSource3f(mSource, AL_POSITION, pos.x, pos.y, pos.z);
	}  // else: source relative to listener, no position update needed
	alSourcePlay(mSource);

	ASSERT(alGetError() == AL_NO_ERROR);
	return SSerror::SS_NO_ERROR;
}



