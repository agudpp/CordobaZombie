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


/* FWD declaration, to avoid circular dependencies. */
class SoundManager;


class SoundAPI
{
	friend class SoundManager;

public:
	/**
	 ** @remark
	 ** Internal scene node can be specified on creation.
	 ** The system's sound manager (see SoundManager.h) must be created first,
	 ** before any SoundAPI.
	 **/
	SoundAPI(Ogre::SceneNode* sNode=NULL);
	virtual ~SoundAPI();

	/**
	 ** @brief
	 ** Set the Ogre SceneNode attached to this SoundAPI.
	 **
	 ** @remark
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
	readyForPlayback();

	/**
	 ** @brief
	 ** Is the SoundAPI currently playing some sound?
	 **
	 ** @remarks
	 ** Returns true iff the SoundAPI is playing.
	 **/
	bool
	playing();

	/**
	 ** @brief
	 ** True both if the SoundAPI is playing, or if it's in a paused state.
	 **/
	bool
	active();

	/**
	 ** @brief
	 ** Start playback of sound named 'sName'. Optionally toogle repeat on end.
	 **
	 ** @remarks
	 ** If the SoundAPI was active (i.e. playing some sound, or in a paused
	 ** playback state) nothing is done.
	 **
	 ** @returns
	 ** SS_NO_ERROR			Sound playback successfully started.
	 ** SS_NO_SOURCES		No available sources to play sound.
	 ** SS_FILE_NOT_FOUND	Sound "sName" not found (inexistent buffer name).
	 ** SS_INTERNAL_ERROR	Unespecified.
	 **/
	SSerror
	play(const Ogre::String &sName, bool repeat=false);

	/**
	 ** @brief
	 ** Pause currently playing sound.
	 ** If no sound had been started by this API, nothing is done.
	 **/
	void
	pause();

	/**
	 ** @brief
	 ** Stop currently playing sound.
	 ** If no sound had been started by this API, nothing is done.
	 **
	 ** @remark
	 ** Internally memory gets freed.
	 ** If the sound is to be replayed again soon call pause();play(); instead.
	 **/
	void
	stop();

	/**
	 ** @brief
	 ** Restarts playback.
	 **
	 ** @remark
	 ** If paused or playing, playback restarts from the beginning.
	 ** If stopped, or if no sound was being played by this SoundAPI,
	 ** nothing is done.
	 **
	 ** @return
	 ** SS_NO_ERROR			Sound playback successfully restarted.
	 ** SS_NO_BUFFER		The SoundAPI wasn't playing anything.
	 ** SS_INTERNAL_ERROR	Unespecified.
	 **/
	SSerror
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
	/**
	 ** @brief
	 ** Display currently playing sound's name.
	 **
	 ** @remarks
	 ** If the SoundAPI is inactive, empty string is returned.
	 **/
	const Ogre::String&
	showAttachedSound();

private:
	Ogre::String mSoundName;  // Name of the currently playing sound.
	long mActivationIndex;    // SoundManager manipulation only (¡choraso!)
};


/******************************************************************************/
/****************************     INLINES     *********************************/


////////////////////////////////////////////////////////////////////////////////
inline void
SoundAPI::setSceneNode(Ogre::SceneNode* sNode)
{
	ASSERT(sNode != NULL);
	mNode = sNode;
}


////////////////////////////////////////////////////////////////////////////////
inline bool
SoundAPI::readyForPlayback()
{
	return mNode != NULL;
}


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
