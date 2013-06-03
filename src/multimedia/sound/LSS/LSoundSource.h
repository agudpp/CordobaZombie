/*
 * LSoundSource.h
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

#ifndef LSOUNDSOURCE_H_
#define LSOUNDSOURCE_H_

#include "SoundSource.h"

namespace mm {

class LSoundSource : public SoundSource
{
	friend class SoundManager;

protected:
	/**
	 ** @brief
	 ** View SoundSource::SoundSource()
	 **/
	LSoundSource();

public:
	virtual ~LSoundSource();

protected:
	/**
	 ** @brief
	 ** View SoundSource::play(...)
	 **
	 ** @remarks
	 ** buf must be a SoundBuffer of type SSbuftype::SS_BUF_LOADED
	 **/
	SSerror
	play(SoundBuffer* buf,
		 const Ogre::Real& gain,
		 const Ogre::Vector3& pos,
		 bool repeat=false);

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
	restart(const Ogre::Real& gain,
			const Ogre::Vector3& pos = Ogre::Vector3(0.0f,0.0f,0.0f),
			const bool repeat = false);
};

}

#endif /* LSOUNDSOURCE_H_ */
