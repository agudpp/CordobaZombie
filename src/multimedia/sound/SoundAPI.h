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



#include <OgreString.h>         // Ogre::String
#include <OgreVector3.h>        // Ogre::Vector3
#include <OgreSceneNode.h>      // Ogre::SceneNode
#include <OgrePrerequisites.h>  // Ogre::Real

#include <debug/DebugUtil.h>
#include "SoundEnums.h"
#include "SoundManager.h"


namespace mm {

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
	SoundAPI(Ogre::SceneNode* sNode=NULL);
	virtual ~SoundAPI();

	/**
	 ** @brief
	 ** Set the Ogre SceneNode attached to this SoundAPI.
	 **
	 ** @remarks
	 ** Should be called right after creation.
	 ** Argument must NOT be NULL.
	 **/
	void
	setSceneNode(Ogre::SceneNode* sNode);

	/**
	 ** @brief
	 ** Wether calling "play(void)" will actually play something.
	 **/
	bool
	readyForPlayback() const;

	/**
	 ** @brief
	 ** Is the SoundAPI currently playing some sound?
	 **
	 ** @remarks
	 ** Returns true iff the SoundAPI is playing.
	 **/
	bool
	playing() const;

	/**
	 ** @brief
	 ** True both if the SoundAPI is playing, or if it's in a paused state.
	 **/
	bool
	active() const;

	/**
	 ** @brief
	 ** Returns current sound name (empty if none)
	 **/
	const Ogre::String&
	getSound() const;

	/**
	 ** @brief
	 ** Sets current sound name
	 **
	 ** @remarks
	 ** Won't start playback, only records sound name.
	 ** Applicable only if the SoundAPI is currently inactive.
	 ** Else fails with error code "SS_ILLEGAL_OPERATION".
	 **
	 ** @return
	 ** SS_NO_ERROR				Sound name successfully set.
	 ** SS_FILE_NOT_FOUND		Sound not found, inexistent buffer name.
	 ** SS_ILLEGAL_OPERATION	SoundAPI was active (viz. playing or paused)
	 **/
	SSerror
	setSound(const Ogre::String& sName);

	/**
	 ** @brief
	 ** Returns current repeat value
	 **/
	const bool
	getRepeat() const;

	/**
	 ** @brief
	 ** Sets current repeat value
	 **
	 ** @remarks
	 ** Applicable only if the SoundAPI is currently inactive.
	 ** Else fails with error code "SS_ILLEGAL_OPERATION".
	 **
	 ** @return
	 ** SS_NO_ERROR				New repeat value successfully set.
	 ** SS_ILLEGAL_OPERATION	SoundAPI was active (viz. playing or paused)
	 **/
	SSerror
	setRepeat(bool repeat);

	/**
	 ** @brief
	 ** Returns current gain value
	 **/
	const Ogre::Real&
	getGain() const;

	/**
	 ** @brief
	 ** Sets current gain value
	 **
	 ** @remarks
	 ** Doesn't change gain of currently playing sound, if any.
	 ** Only affects sounds whose playback starts after this call.
	 **/
	void
	setGain(const Ogre::Real& gain);

	/**
	 ** @brief
	 ** Start playback of previously set sound.
	 **
	 ** @remarks
	 ** Uses previous values for sound buffer name (see setSound()),
	 ** repeat (see setRepeat()), and gain (see setGain()).
	 **
	 ** @remarks
	 ** If the SoundAPI was active (i.e. playing some sound, or in a paused
	 ** playback state) nothing is done.
	 **
	 ** @return
	 ** SS_NO_ERROR				Sound playback successfully started.
	 ** SS_NO_SOURCES			No available sources to play sound.
	 ** SS_ILLEGAL_OPERATION	We were not readyForPlayback()
	 ** SS_INTERNAL_ERROR		Unspecified.
	 **/
	SSerror
	play(void);

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
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	play(const Ogre::String& sName,
		 bool repeat = false,
		 const Ogre::Real& gain = DEFAULT_UNIT_GAIN);

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
	 ** @remarks
	 ** Internally memory gets freed.
	 ** If the sound is to be replayed again soon call pause();play(); instead.
	 **/
	void
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
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	restart();

protected:
	/**
	 ** @brief
	 ** Retrieve sound 3D position, for playback purposes.
	 **/
	const Ogre::Vector3&
	getPosition() const;

protected:
	SoundManager&	 mSoundManager;  // Cannot be NULL
	Ogre::SceneNode* mNode;

private:
	bool mRepeat;
	Ogre::Real mGain;
	Ogre::String mSoundName;  // Name of the currently playing sound.
	long mActivationIndex;    // SoundManager manipulation only (¡choraso!)
};


/******************************************************************************/
/****************************     INLINES     *********************************/

////////////////////////////////////////////////////////////////////////////////
inline SoundAPI::SoundAPI(Ogre::SceneNode* sNode) :
	mSoundManager(SoundManager::getInstance()),
	mNode(sNode),
	mRepeat(false),
	mGain(DEFAULT_UNIT_GAIN),
	mSoundName(),
	mActivationIndex(-1)
{ /* Default constructor suffices. */ }

////////////////////////////////////////////////////////////////////////////////
inline SoundAPI::~SoundAPI()
{
	// Relies on good and efficient behaviour if we're currently inactive
	stop();
	mNode = 0;  // C good practices
}

////////////////////////////////////////////////////////////////////////////////
inline void
SoundAPI::setSceneNode(Ogre::SceneNode* sNode)
{
	ASSERT(sNode);
	mNode = sNode;
}

////////////////////////////////////////////////////////////////////////////////
inline bool SoundAPI::readyForPlayback() const
{
	return mNode != 0 && !mSoundName.empty();
}

////////////////////////////////////////////////////////////////////////////////
inline bool SoundAPI::playing() const { return mSoundManager.findPlayingAPI(*this); }

////////////////////////////////////////////////////////////////////////////////
inline bool SoundAPI::active() const { return mSoundManager.findActiveAPI(*this); }

////////////////////////////////////////////////////////////////////////////////
inline const Ogre::String& SoundAPI::getSound() const { return mSoundName; }

////////////////////////////////////////////////////////////////////////////////
inline SSerror
SoundAPI::setSound(const Ogre::String& sName)
{
	if (mSoundManager.findActiveAPI(*this))
		return SSerror::SS_ILLEGAL_OPERATION;
	else if (!mSoundManager.isSoundLoaded(sName))
		return SSerror::SS_FILE_NOT_FOUND;
	else
		mSoundName = sName;
	return SSerror::SS_NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////////
inline const bool SoundAPI::getRepeat() const { return mRepeat; }

////////////////////////////////////////////////////////////////////////////////
inline SSerror
SoundAPI::setRepeat(bool repeat)
{
	if (mSoundManager.findActiveAPI(*this))
		return SSerror::SS_ILLEGAL_OPERATION;
	else
		mRepeat = repeat;
	return SSerror::SS_NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////////
inline const Ogre::Real& SoundAPI::getGain() const { return mGain; }

////////////////////////////////////////////////////////////////////////////////
inline void SoundAPI::setGain(const Ogre::Real& gain) { mGain = gain; }

////////////////////////////////////////////////////////////////////////////////
inline SSerror
SoundAPI::play()
{
	if (readyForPlayback())
		return mSoundManager.playSound(*this, mSoundName, mGain, mRepeat);
	else
		return SSerror::SS_ILLEGAL_OPERATION;
}

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
inline void SoundAPI::stop() { mSoundManager.stopSound(*this); }

////////////////////////////////////////////////////////////////////////////////
inline SSerror SoundAPI::restart() { return mSoundManager.restartSound(*this); }

////////////////////////////////////////////////////////////////////////////////
inline const Ogre::Vector3&
SoundAPI::getPosition() const
{
	ASSERT(mNode != NULL);
	return mNode->getPosition();
}

}

#endif /* SOUNDAPI_H_ */
