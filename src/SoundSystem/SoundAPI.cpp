/*
 * SoundAPI.cpp
 *
 *  This class is the Unit's interaction point with the game's sound system.
 *  Playable sounds are only accessed here for playback: play, pause, stop.
 *  Available (playable) sounds are those which have been previously loaded
 *  through the SoundManager.
 *
 *  Created on: May 8, 2012
 *     Company: estrategia201Z
 *      Author: Budde, Carlos Esteban.
 */


#include "SoundAPI.h"
#include "SoundManager.h"


/**
 ** TODO
 ** Pasar todas estas funciones al .h como inlines.
 **/

////////////////////////////////////////////////////////////////////////////////
/**
 ** @remark
 ** The system's sound manager (see SoundManager.h) must be created first,
 ** before any SoundAPI.
 **/
SoundAPI::SoundAPI(Ogre::SceneNode* sNode) :
	mSoundManager(SoundManager::getInstance()),
	mNode(sNode),
	mSoundName(),
	mActivationIndex(-1)
{ /* Default constructor suffices. */ }


////////////////////////////////////////////////////////////////////////////////
SoundAPI::~SoundAPI()
{ /* Default destructor suffices. */ }


////////////////////////////////////////////////////////////////////////////////
bool
SoundAPI::playing()
{
	return mSoundManager.findPlayingAPI(*this);
}


////////////////////////////////////////////////////////////////////////////////
bool
SoundAPI::active()
{
	return mSoundManager.findActiveAPI(*this);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundAPI::play(const Ogre::String &sName, bool repeat, const Ogre::Real& gain)
{
	mSoundName = sName;
	return mSoundManager.playSound(*this, sName, gain, repeat);
}


////////////////////////////////////////////////////////////////////////////////
void
SoundAPI::pause()
{
	mSoundManager.pauseSound(*this);
}

////////////////////////////////////////////////////////////////////////////////
void
SoundAPI::stop()
{
	mSoundManager.stopSound(*this);
	mSoundName.clear();
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundAPI::restart()
{
	return mSoundManager.restartSound(*this);
}

