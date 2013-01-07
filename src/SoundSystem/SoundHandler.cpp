/*
 * SoundHandler.cpp
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

#include <vector>

#include "SoundHandler.h"



// SoundHandler's static member initialization.
SoundManager& SoundHandler::sSoundManager = SoundManager::getInstance();


////////////////////////////////////////////////////////////////////////////////
Ogre::String
SoundHandler::loadStreamSounds(const std::vector<Ogre::String>& list)
{
	Ogre::String fails("");

	// TODO
	debugWARNING("TODO\n");ASSERT(false);

	return fails;
}


////////////////////////////////////////////////////////////////////////////////
Ogre::String
SoundHandler::loadDirectSounds(const std::vector<Ogre::String>& list)
{
	Ogre::String fails("");

	// TODO
	debugWARNING("TODO\n");ASSERT(false);

	return fails;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::unloadSounds(const std::vector<Ogre::String>& list)
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::shutDown(void)
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
Ogre::String
SoundHandler::newPlaylist(const Ogre::String& name,
						  const std::vector<Ogre::String>& soundsList,
						  bool  repeat=true,
						  bool  random=false,
						  float silence=0.0f)
{
	Ogre::String fails("");

	// TODO
	debugWARNING("TODO\n");ASSERT(false);

	return fails;
}

////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::deletePlaylist(const Ogre::String& name)
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}



////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::startPlaylist(const Ogre::String& name) const
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::pausePlaylist(const Ogre::String& name) const
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::stopPlaylist(const Ogre::String& name) const
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::restartPlaylist(const Ogre::String& name) const
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::fadeOutPlaylist(const Ogre::String& name,
							  const Ogre::Real& time,
							  const bool pause=true) const
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::fadeInPlaylist(const Ogre::String& name,
							 const Ogre::Real& time) const
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}



