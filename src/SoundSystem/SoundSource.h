/*
 * SoundSource.h
 *
 *  Interface for OpenAL source manipulation.
 *  This class hides the playback mechanisms of the supported audio formats
 *  listed in SoundEnums::SSformat.
 *
 *  This is an ABSTRACT CLASS.
 *
 *  Created on: May 3, 2012
 *     Company: estrategia201Z
 *      Author: Budde, Carlos Esteban.
 */

#ifndef SOUNDSOURCE_H_
#define SOUNDSOURCE_H_

#include <Ogre.h>
#include <OgreVector3.h>    // Ogre::Vector3
#include <OgreSceneNode.h>  // Ogre::SceneNode
#include "SoundEnums.h"
#include "SoundBuffer.h"

#if defined _WIN32 || defined CYGWIN
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#elif defined linux || defined _linux || defined __linux || defined __linux__
#  include <AL/al.h>
#  include <AL/alc.h>
#else
#  error "Unsupported platform, aborting compilation."
#endif


typedef ALuint ALSource;

/* FWD declaration */
class SoundManager;

class SoundSource
{
protected:

	friend class SoundManager;

	/**
	 ** @remark
	 ** Ctor registers a new source into OpenAL system.
	 ** This requires an EXISTING OPENAL CONTEXT.
	 **/
	inline SoundSource(SSsrctype type = SSsrctype::SS_SRC_LOADED);
	inline virtual ~SoundSource();

	/**
	 ** @brief
	 ** Tells whether the source is currently playing some sound.
	 **/
	inline virtual bool
	isPlaying();

	/**
	 ** @brief
	 ** Tells whether the source is active (i.e. playing some sound, or in a
	 ** paused state).
	 **/
	inline virtual bool
	isActive();

	/**
	 ** @brief
	 ** Starts/continues source playback on specified SoundBuffer.
	 **
	 ** @remarks
	 ** On first play, "buf" must contain a valid OpenAL buffer.
	 ** On replay (after pause) buf can be NULL, or contain the same buffer
	 ** used to start playback.
	 ** If source was already playing, nothing is done.
	 **
	 ** @param
	 **    buf: buffer with the sound to play
	 **   gain: volume of the sound, in [ 0.0 , 1.0 ] scale
	 **    pos: sound source position inside Ogre's 3D world
	 ** repeat: whether to repeat on end (default: false)
	 **
	 ** @return
	 ** SS_NO_ERROR			     Sound playback successfully started.
	 ** SS_NO_BUFFER             Buffer parameter NULL, or invalid.
	 ** SS_INTERNAL_ERROR	     Unespecified.
	 **/
	virtual SSerror
	play(SoundBuffer* buf,
		 const Ogre::Real& gain,
		 const Ogre::Vector3& pos,
		 bool repeat=false) = 0;

	/**
	 ** @brief
	 ** Updates internal playback status, and informs accordingly.
	 **
	 ** @remark
	 ** Position gets updated only if state == SS_PLAYING.
	 ** If state == SS_STOPPED, buffer gets detached.
	 **
	 ** @param
	 ** pos: sound source position inside Ogre's 3D world
	 **
	 ** @return
	 ** SS_PLAYING		Still playing buffer.
	 ** SS_PAUSED		Playback paused, leave things untouched.
	 ** SS_FINISHED		Playback ended, free memory and do callbacks.
	 **/
	virtual SSplayback
	update(const Ogre::Vector3& pos = Ogre::Vector3(0.0f,0.0f,0.0f)) = 0;

	/**
	 ** @brief
	 ** Pauses current playback.
	 **/
	virtual void
	pause() = 0;

	/**
	 ** @brief
	 ** Stops current playback, and detaches buffer.
	 **/
	virtual void
	stop() = 0;

	/**
	 ** @brief
	 ** Restarts playback.
	 **
	 ** @remark
	 ** If paused or playing, playback restarts from the beginning.
	 ** If stopped or if no buffer is attached, nothing is done.
	 **
	 ** @param
	 **   gain: volume of the sound, in [ 0.0 , 1.0 ] scale
	 **    pos: sound source position inside Ogre's 3D world
	 ** repeat: whether to repeat on end (default: false)
	 **
	 ** @return
	 ** SS_NO_ERROR			Sound playback successfully restarted.
	 ** SS_NO_BUFFER		Source wasn't playing anything, or stale buffer.
	 ** SS_INTERNAL_ERROR	Unespecified.
	 **/
	virtual SSerror
	restart(const Ogre::Real& gain,
			const Ogre::Vector3& pos = Ogre::Vector3(0.0f,0.0f,0.0f),
			const bool repeat = false) = 0;

	/**
	 ** @brief
	 ** Check SoundSource specific type (LSoundSource vs. SSoundSource)
	 **/
	inline SSsrctype
	getType();

	/**
	 ** @brief
	 ** Retrieve repeat value.
	 **/
	inline bool
	getRepeat();

protected:
	ALSource		mSource;  // ID of the attached OpenAL source.
	SoundBuffer*	mBuffer;  // Currently playing sound buffer.
	SSsrctype		mType;    // Specific source type (LSS vs. SSS)
	bool			mRepeat;  // Repeat on end?
};




/******************************************************************************/
/****************************     INLINES     *********************************/


////////////////////////////////////////////////////////////////////////////////
inline
SoundSource::SoundSource(SSsrctype type) :
		mType(type),
		mBuffer(0),
		mRepeat(false)
{
	ASSERT(alcGetCurrentContext() != NULL);
	ASSERT(alGetError() == AL_NO_ERROR);

	alGenSources(1,&mSource);
	alSourcei(mSource, AL_LOOPING, AL_FALSE);
	alSource3f(mSource, AL_POSITION, 0.0f, 0.0f, 0.0f);

	ASSERT(alGetError() == AL_NO_ERROR);
}


////////////////////////////////////////////////////////////////////////////////
inline SoundSource::~SoundSource() { /* Default destructor suffices */ }


////////////////////////////////////////////////////////////////////////////////
inline bool
SoundSource::isPlaying()
{
	ALint st(AL_NONE);
	alGetSourcei(mSource, AL_SOURCE_STATE, &st);
	return (st == AL_PLAYING);
}


////////////////////////////////////////////////////////////////////////////////
inline bool
SoundSource::isActive()
{
	ALint st(AL_NONE);
	alGetSourcei(mSource, AL_SOURCE_STATE, &st);
	return (st == AL_PLAYING || st == AL_PAUSED);
}


////////////////////////////////////////////////////////////////////////////////
inline SSsrctype SoundSource::getType() { return mType; }


////////////////////////////////////////////////////////////////////////////////
inline bool SoundSource::getRepeat() { return mRepeat; }


#endif /* SOUNDSOURCE_H_ */
