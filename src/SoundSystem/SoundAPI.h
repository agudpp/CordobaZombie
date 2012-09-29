/*
 * SoundAPI.h
 *
 *  This class is the Unit's interaction point with the game's sound system.
 *  Playable sounds are only accessed here for playback: play, pause, stop.
 *  Available (playable) sounds are those which have been previously loaded
 *  through the SoundManager.
 *
 *  Created on: May 3, 2012
 *     Company: estrategia201Z
 *      Author: Budde, Carlos Esteban.
 */

#ifndef SOUNDAPI_H_
#define SOUNDAPI_H_



#include <Ogre.h>
#include <OgreVector3.h>
#include <OgreSceneNode.h>
#include "DebugUtil.h"
#include "SoundEnums.h"
#include "SoundManager.h"



class SoundAPI
{
	friend class SoundManager;

public:
	/**
	 ** @remarks
	 ** Internal scene node can be specified on creation.
	 ** The system's sound manager (see SoundManager.h) must be created first,
	 ** before any SoundAPI.
	 **/
	inline SoundAPI(Ogre::SceneNode* sNode=NULL);
	virtual inline ~SoundAPI();

	/**
	 ** @brief
	 ** Set the Ogre SceneNode attached to this SoundAPI.
	 **
	 ** @remarks
	 ** Should be called right after creation.
	 ** Argument must NOT be NULL.
	 **/
	inline void
	setSceneNode(Ogre::SceneNode* sNode);

	/**
	 ** @brief
	 ** API's scene node already set? (i.e. mNode != NULL)
	 **/
	inline bool
	readyForPlayback() const;

	/**
	 ** @brief
	 ** Is the SoundAPI currently playing some sound?
	 **
	 ** @remarks
	 ** Returns true iff the SoundAPI is playing.
	 **/
	inline bool
	playing() const;

	/**
	 ** @brief
	 ** True both if the SoundAPI is playing, or if it's in a paused state.
	 **/
	inline bool
	active() const;

	/**
	 ** @brief
	 ** Start playback of sound named 'sName'. Optionally toogle repeat on end.
	 **
	 ** @remarks
	 ** If the SoundAPI was active (i.e. playing some sound, or in a paused
	 ** playback state) nothing is done.
	 **
	 ** @param
	 **  sName: name of the audio file to play.
	 ** repeat: whether to repeat on end (default: false)
	 **   gain: volume of the sound, in [ 0.0 , 1.0 ] scale (default: 0.7)
	 **
	 ** @return
	 ** SS_NO_ERROR			Sound playback successfully started.
	 ** SS_NO_SOURCES		No available sources to play sound.
	 ** SS_FILE_NOT_FOUND	Sound "sName" not found (inexistent buffer name).
	 ** SS_INTERNAL_ERROR	Unespecified.
	 **/
	inline SSerror
	play(const Ogre::String &sName,
		 bool repeat = false,
		 const Ogre::Real& gain = DEFAULT_UNIT_GAIN);

	/**
	 ** @brief
	 ** Pause currently playing sound.
	 ** If no sound had been started by this API, nothing is done.
	 **/
	inline void
	pause();

	/**
	 ** @brief
	 ** Stop currently playing sound.
	 ** If no sound had been started by this API, nothing is done.
	 **
	 ** @remarks
	 ** Internally memory gets freed.
	 ** If the sound is to be replayed again soon call pause();play(); instead.
	 **/
	inline void
	stop();

	/**
	 ** @brief
	 ** Restarts playback.
	 **
	 ** @remarks
	 ** If paused or playing, playback restarts from the beginning.
	 ** If stopped, or if no sound was being played by this SoundAPI,
	 ** nothing is done.
	 **
	 ** @return
	 ** SS_NO_ERROR			Sound playback successfully restarted.
	 ** SS_NO_BUFFER		The SoundAPI wasn't playing anything.
	 ** SS_INTERNAL_ERROR	Unespecified.
	 **/
	inline SSerror
	restart();

	/**
	 ** @brief
	 ** Returns the currently playing sound name
	 **/
	inline const Ogre::String&
	getCurrentSound() const;

protected:
	/**
	 ** @brief
	 ** Retrieve sound 3D position, for playback purposes.
	 **/
	inline const Ogre::Vector3&
	getPosition() const;

protected:
	SoundManager&	 mSoundManager;  // Cannot be NULL
	Ogre::SceneNode* mNode;

private:
	Ogre::String mSoundName;  // Name of the currently playing sound.
	long mActivationIndex;    // SoundManager manipulation only (¡choraso!)
};


/******************************************************************************/
/****************************     INLINES     *********************************/

////////////////////////////////////////////////////////////////////////////////
inline SoundAPI::SoundAPI(Ogre::SceneNode* sNode) :
	mSoundManager(SoundManager::getInstance()),
	mNode(sNode),
	mSoundName(),
	mActivationIndex(-1)
{ /* Default constructor suffices. */ }

////////////////////////////////////////////////////////////////////////////////
inline SoundAPI::~SoundAPI() { /* Default destructor suffices. */ }

////////////////////////////////////////////////////////////////////////////////
inline void
SoundAPI::setSceneNode(Ogre::SceneNode* sNode)
{
	ASSERT(sNode);
	mNode = sNode;
}

////////////////////////////////////////////////////////////////////////////////
inline bool SoundAPI::readyForPlayback() const { return mNode != 0; }

////////////////////////////////////////////////////////////////////////////////
inline bool SoundAPI::playing() const { return mSoundManager.findPlayingAPI(*this); }

////////////////////////////////////////////////////////////////////////////////
inline bool SoundAPI::active() const { return mSoundManager.findActiveAPI(*this); }


////////////////////////////////////////////////////////////////////////////////
inline SSerror
SoundAPI::play(const Ogre::String &sName, bool repeat, const Ogre::Real& gain)
{
	mSoundName = sName;
	return mSoundManager.playSound(*this, sName, gain, repeat);
}

////////////////////////////////////////////////////////////////////////////////
inline void SoundAPI::pause() { mSoundManager.pauseSound(*this); }

////////////////////////////////////////////////////////////////////////////////
inline void SoundAPI::stop()
{
	mSoundManager.stopSound(*this);
	mSoundName.clear();
}

////////////////////////////////////////////////////////////////////////////////
inline SSerror SoundAPI::restart() { return mSoundManager.restartSound(*this); }

////////////////////////////////////////////////////////////////////////////////
inline const Ogre::String&
SoundAPI::getCurrentSound() const
{
	return mSoundName;
}

////////////////////////////////////////////////////////////////////////////////
inline const Ogre::Vector3&
SoundAPI::getPosition() const
{
	ASSERT(mNode != NULL);
	return mNode->getPosition();
}


#endif /* SOUNDAPI_H_ */
