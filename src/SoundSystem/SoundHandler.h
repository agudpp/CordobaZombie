/*
 * SoundHandler.h
 *
 *  This is the highest level class of the whole SoundSystem.
 *
 *  It's in charge of manipulating the SoundManager to implement playlists,
 *  random ambience sounds (e.g. distant howls and screams, crow's cries, etc),
 *  equally spaced repeating sounds (e.g. an ambulance's siren), and so on.
 *
 *	This is a SINGLETON CLASS.
 *  User access to the game's Sound System should be through the SoundHandler.
 *
 *  Created on: Jan 5, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef SOUNDHANDLER_H_
#define SOUNDHANDLER_H_

#include "SoundManager.h"

class SoundHandler
{
public:
	/**
	 ** @brief
	 ** We use a singleton pattern for this class.
	 ** This is threadsafe on Linux/Windows if compiling with gcc/g++/cygwin.
	 **
	 ** @remarks
	 ** On first call, THE SoundHandler is created, along with an attached
	 ** SoundManager (see SoundManager.h)
	 **
	 ** @remarks
	 ** getInstance() singleton pattern requires ctor, copytor and assign
	 ** methods to be declared private.
	 **
	 ** @return
	 ** A reference to the sole existent instance of SoundHandler.
	 **/
	inline static SoundHandler&
	getInstance();

private:
	/* Prevent the compiler from generating methods to copy the instance: */
	SoundHandler(SoundHandler const&);    // Don't implement!
	void operator=(SoundHandler const&);  // Don't implement!

	/* @remarks
	 * ctor creates and attaches a SoundManager to the object. */
	SoundHandler();

	/* @remarks
	 * dtor deletes also the attached SoundManager, and thus it releases
	 * all OpenAL sound resources (context, device) */
	~SoundHandler();

private:
	static SoundManager&	sSoundManager;
};


/******************************************************************************/
/****************************     INLINES     *********************************/


////////////////////////////////////////////////////////////////////////////////
SoundHandler&
SoundHandler::getInstance()
{
	static SoundHandler instance;
	return instance;
}


////////////////////////////////////////////////////////////////////////////////
inline
SoundHandler::SoundHandler()
{
	// TODO
}


////////////////////////////////////////////////////////////////////////////////
inline
SoundHandler::~SoundHandler()
{
	// TODO
}


#endif /* SOUNDHANDLER_H_ */
