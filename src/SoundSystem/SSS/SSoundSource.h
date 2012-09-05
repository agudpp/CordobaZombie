/*
 * SSoundSource.h
 *
 *	StreamingSoundSources
 *	---------------------
 *  Abstract interface for OpenAL source manipulation.
 *  This class implements the abstract class SoundSource.
 *  >>> PLAYS BUFFERS USING A STREAMING MECHANISM <<<
 *
 *  Created on: Jun 11, 2012
 *     Company: estrategia201Z
 *      Author: Budde, Carlos Esteban.
 */

#ifndef SSOUNDSOURCE_H_
#define SSOUNDSOURCE_H_

#include "SoundSource.h"

class SSoundSource : public SoundSource
{
	friend class SoundManager;

protected:
	SSoundSource();
	virtual ~SSoundSource();

	/**
	 ** @brief
	 ** View SoundSource::play()
	 **
	 ** @remarks
	 ** buf must be of type SSbuftype::SS_BUF_STREAM_WAV or SS_BUF_STREAM_OGG
	 ** If source playing or paused, may update internal buffers.
	 **/
	SSerror
	play(SoundBuffer* buf,
		 const Ogre::Vector3& pos,
		 bool  repeat=false);

	/**
	 ** @brief
	 ** View SoundSource::update()
	 **/
	SSplayback
	update(const Ogre::Vector3& pos = Ogre::Vector3(0.0f,0.0f,0.0f));

	/**
	 ** @brief
	 ** View SoundSource::pause()
	 **/
	void
	pause();

	/**
	 ** @brief
	 ** View SoundSource::stop()
	 **/
	void
	stop();

	/**
	 ** @brief
	 ** View SoundSource::restart()
	 **/
	SSerror
	restart(const Ogre::Vector3& pos = Ogre::Vector3(0.0f,0.0f,0.0f));

private:
	/* Repeat on end? */
	bool mRepeat;

	/* Current playing offset within the file's audio data. */
	int mFileOffset;

	/* Internal buffers for stream queuing. */
	ALBuffer mIntBuffers[SS_NUM_INT_BUFFERS];	// Buffers array
	const unsigned int mIntBuffersSize;			// Data size of each
	unsigned int mFirstBuffer;					// First buffer in playing queue
};


#endif /* SSOUNDSOURCE_H_ */
