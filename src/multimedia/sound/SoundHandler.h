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

#include <vector>
#include <algorithm>

#include <OgreString.h>
#include <OgrePrerequisites.h>

#include "SoundEnums.h"
#include "SoundManager.h"
#include "SoundSource.h"


namespace mm {

class SoundHandler
{
	struct Playlist
	{
		Playlist(const Ogre::String& name="");  // repeat = true
		Playlist(const Ogre::String& name,
				 const std::vector<Ogre::String>& list,
				 bool  repeat=true,
				 bool  randomOrder=false,
				 bool  randomSilence=false,
				 float silence=0.0f);
		Playlist(const Playlist& pl);
		Playlist& operator=(const Playlist& pl);
		~Playlist(void);
	public:
		Ogre::String 				mName;
		std::vector<Ogre::String>	mList;      // Sounds names list
		std::vector<uint>			mPlayOrder; // Sounds playing order
		unsigned int				mCurrent;   // Current position in mPlayOrder
		unsigned int 				mState;     // Repeat/Shuffle/Randomwait
		// TODO: consider deletion of global state (mGState member)
		unsigned int				mGState;	// Global handling (play/pause)
		float						mSilence;   // Wait time between sounds (sec)
		float						mTimeSinceFinish;
	};

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

	/*********************************************************************/
	/***************    CTOR, DTOR & INITIALIZATIONS    ******************/
private:
	/* Prevent the compiler from generating methods to copy the instance: */
	SoundHandler(SoundHandler const&);    // Don't implement!
	void operator=(SoundHandler const&);  // Don't implement!

	/** @remarks
	 ** ctor creates and attaches a SoundManager to the object. */
	SoundHandler();

	/** @remarks
	 ** dtor deletes also the attached SoundManager, and thus it releases
	 ** all OpenAL sound resources (context, device) */
	~SoundHandler();

public:
	/** Lists available sound devices. */
	std::vector<std::string>
	getAvailableSoundDevices();

	/**
	 ** @brief
	 ** Name of the currently open sound device. There's always one.
	 **
	 ** @return
	 ** Empty string if ALC_ENUMERATION_EXT extension is not present;
	 ** name of the opened sound device otherwise.
	 **/
	std::string
	getSoundDevice();

	/**
	 ** @brief
	 ** Changes current sound device to devName.
	 **
	 ** @remarks
	 ** NULL argument selects default sound device.
	 ** This destroys current sound context, all playing sounds will be lost.
	 ** If devName can't be used, nothing is done.
	 **
	 ** @return
	 ** SS_NO_ERROR			Success.
	 ** SS_INTERNAL_ERROR	Couldn't create context on specified device.
	 **/
	SSerror
	setSoundDevice(std::string* devName);

	/**
	 ** @brief
	 ** Set the camera attached to the SoundManager.
	 **
	 ** @remarks
	 ** The camera determines the position and orientation of the listener,
	 ** both of which get updated on each call to SceneManager::update()
	 **/
	inline void
	setCamera(const Ogre::Camera* cam);


	/*********************************************************************/
	/*****************    SOUNDS LOADING/UNLOADING    ********************/
public:
	/**
	 ** @brief
	 ** Loads a list of sound files, to be played with streaming mechanisms.
	 **
	 ** @remarks
	 ** Filenames are expected to contain the sound format extension
	 ** (e.g., *.mp3, *.wav, *.ogg, etc.)
	 **
	 ** @return
	 ** List of files which failed to load, with respective error message.
	 ** File names are separated with UNIX newline characters (viz. '\n')
	 **/
	Ogre::String
	loadStreamSounds(const std::vector<Ogre::String>& list);

	/**
	 ** @brief
	 ** Loads a list of sound files, to be played directly from memory.
	 **
	 ** @remarks
	 ** Filenames are expected to contain the sound format extension
	 ** (e.g., *.mp3, *.wav, *.ogg, etc.)
	 **
	 ** @return
	 ** List of files which failed to load, with respective error message.
	 ** File names are separated with UNIX newline characters (viz. '\n')
	 **/
	Ogre::String
	loadDirectSounds(const std::vector<Ogre::String>& list);

	/**
	 ** @brief
	 ** Unloads from the system the given list of sounds.
	 ** Sounds which had not been previously loaded are simply ignored.
	 **
	 ** @remarks
	 ** This method is expensive, as it must check for EVERY sound
	 ** if it's currently under system usage.
	 **
	 ** @return
	 ** List of files which failed to unload because they were being used
	 ** by the system, either as Environmental or SoundAPI sounds.
	 ** File names are separated with UNIX newline characters (viz. '\n')
	 **/
	Ogre::String
	unloadSounds(const std::vector<Ogre::String>& list);

	/**
	 ** @brief
	 ** Adds "N" new sound sources that can play loaded Stream Sounds.
	 **
	 ** @return
	 ** SS_NO_ERROR			Sources successfully added.
	 ** SS_NO_MEMORY		Insufficient memory for operation, nothing was done.
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	addStreamSources(unsigned int N);

	/**
	 ** @brief
	 ** Adds "N" new sound sources that can play loaded Direct Sounds.
	 **
	 ** @return
	 ** SS_NO_ERROR			Sources successfully added.
	 ** SS_NO_MEMORY		Insufficient memory for operation, nothing was done.
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	addDirectSources(unsigned int N);

	/**
	 ** @brief
	 ** Stop all playbacks, release all resources, and get the hell out.
	 **/
	void
	shutDown(void);


	/*********************************************************************/
	/****************    GLOBAL PLAYBACK CONTROLS    *********************/
public:
	/**
	 ** @brief
	 ** Updates all sounds currently active in the system.
	 **
	 ** @param
	 ** globalTimeFrame: time elapsed during the computations of the last frame
	 ** 				 (former "GLOBAL_TIME_FRAME")
	 **
	 ** @remarks
	 ** Updates must be done periodically, to allow streaming sources (such as
	 ** MP3 or large OGGs) to refresh their internal buffering mechanisms.
	 **/
	void
	update(const float globalTimeFrame);

	/**
	 ** @brief
	 ** Pauses ("globally") all currently playing sounds.
	 **/
	void
	globalPause();

	/**
	 ** @brief
	 ** Plays again ("globally") all paused sounds.
	 **
	 ** @remarks
	 ** Sounds, both environmental and attached to a unit's SoundAPI,
	 ** which were paused manually (i.e. before calling globalPause()),
	 ** will NOT start playing again with this function.
	 **/
	void
	globalPlay();

	/**
	 ** @brief
	 ** Stops all sounds.
	 **
	 ** @remarks
	 ** BEWARE! this function frees all playback resources.
	 ** That means that all active sources and buffers get released.
	 ** Playbacks however are not erased, but they are reset.
	 **/
	void
	globalStop();

	/**
	 ** @brief
	 ** Restarts all active sounds in the system.
	 **
	 ** @remarks
	 ** This affects both playing and paused sounds.
	 **/
	void
	globalRestart();

	/**
	 ** @brief
	 ** Fades out the playback volume of all playing sounds in the system.
	 **
	 ** @param
	 **  time: fade-out time, in seconds. If negative defaults to 1.0
	 ** pause: whether to pause the sounds once muted. Default: true.
	 **
	 ** @remarks
	 ** If pause==true, globalFadeIn() will restart playbacks when called.
	 ** Individual fadings (in or out) are overriden by this function.
	 **/
	void
	globalFadeOut(const Ogre::Real& time, const bool pause=true);

	/**
	 ** @brief
	 ** Fades back in all (globally) paused or faded-out sounds.
	 **
	 ** @param
	 ** time: fade-in time, in seconds. If negative defaults to 1.0
	 **
	 ** @remarks
	 ** Sounds (both environmental and attached to a unit's SoundAPI)
	 ** which were faded-out manually, i.e. before calling globalFadeOut(),
	 ** will NOT be modified by this function.
	 **
	 ** @remarks
	 ** Playback is restarted if the sounds had been faded-out and paused.
	 **/
	void
	globalFadeIn(const Ogre::Real& time);


	/*********************************************************************/
	/*************************    PLAYLISTS    ***************************/
public:
	/**
	 ** @brief
	 ** Create a new playlist, with given name and sounds list.
	 **
	 **	@param
	 **       name: playlist name
	 ** soundsList: names of the sounds the playlist will contain
	 **     repeat: whether to repeat on end
	 **     random: whether to play in random order
	 **    silence: time to wait (in seconds) between sounds playback.
	 **             If negative value given, silence time is randomized
	 **             after each play, within the range [MIN_SILENCE,MAX_SILENCE]
	 **
	 ** @return
	 ** Message "Playlist <name> exists!\n" if a playlist by the name "name"
	 ** had already been created into the system.
	 ** Otherwise:
	 ** List of sounds which could not be included inside the playlist,
	 ** toghether with the corresponding error messages.
	 ** File names are separated with UNIX newline characters (i.e., '\n')
	 **/
	Ogre::String
	newPlaylist(const Ogre::String& name,
				const std::vector<Ogre::String>& soundsList,
				bool  repeat=true,
				bool  random=false,
				float silence=0.0f);

	/**
	 ** @brief
	 ** Whether a playlist named "name" has already been loaded into the system.
	 **/
	bool
	existsPlaylist(const Ogre::String& name) const;

	/**
	 ** @brief
	 ** Remove the playlist from the Sound System
	 **
	 ** @remarks
	 ** If playlist "name" didn't exist, nothing is done.
	 ** If it did, any playback is stopped before deletion.
	 **/
	void
	deletePlaylist(const Ogre::String& name);

	/**
	 ** @brief
	 ** Start playlist playback.
	 **
	 ** @remarks
	 ** If the playlist had been paused, playback is resumed from pause point.
	 ** If the playlist had been stopped, it restarts from the begginning.
	 ** If the playlist is currently playing, nothing is done.
	 **
	 ** @param
	 ** name: name of the playlist, mandatory if @arg "pl" is NULL
	 **   pl: optional, provide the Playlist pointer to save on search time.
	 **
	 ** @return
	 ** SS_NO_ERROR			Playback successfully started.
	 ** SS_FILE_NOT_FOUND	No playlist "name" found.
	 ** SS_INTERNAL_ERROR	Unspecified error.
	 **/
	SSerror
	startPlaylist(const Ogre::String& name, Playlist *pl = 0);

	/**
	 ** @brief
	 ** Pauses playlist.
	 **
	 ** @remarks
	 ** If startPlaylist(name) is invoked after calling pausePlaylist(name),
	 ** playback resumes from where it paused.
	 **
	 ** @remarks
	 ** If no playlist "name" exists, nothing is done.
	 **
	 ** @param
	 ** name: name of the playlist, mandatory if @arg "pl" is NULL
	 **   pl: optional, provide the Playlist pointer to save on search time.
	 **/
	void
	pausePlaylist(const Ogre::String& name, Playlist *pl = 0);

	/**
	 ** @brief
	 ** Stops playlist.
	 **
	 ** @remarks
	 ** Playlist current position is lost, and current resources are released.
	 ** If startPlaylist(name) is invoked after calling stopPlaylist(name),
	 ** playback starts from the begginning of the playlist.
	 **
	 ** @remarks
	 ** If no playlist "name" exists, nothing is done.
	 **
	 ** @param
	 ** name: name of the playlist, mandatory if @arg "pl" is NULL
	 **   pl: optional, provide the Playlist pointer to save on search time.
	 **/
	void
	stopPlaylist(const Ogre::String& name, Playlist *pl = 0);

	/**
	 ** @brief
	 ** Restarts playlist.
	 **
	 ** @remarks
	 ** Same effect than stopPlaylist + startPlaylist.
	 **
	 ** @param
	 ** name: name of the playlist, mandatory if @arg "pl" is NULL
	 **   pl: optional, provide the Playlist pointer to save on search time.
	 **
	 ** @return
	 ** SS_NO_ERROR			Playback successfully restarted.
	 ** SS_FILE_NOT_FOUND	No playlist "name" found.
	 ** SS_INTERNAL_ERROR	Unspecified error.
	 **/
	SSerror
	restartPlaylist(const Ogre::String& name, Playlist *pl = 0);

	/**
	 ** @brief
	 ** Fades out the playback volume of the playlist's current sound.
	 **
	 ** @param
	 **  name: name of the playlist
	 **  time: fade-out time, in seconds. If negative defaults to 1.0
	 ** pause: whether to pause the sounds once muted. Default: true.
	 **
	 ** @remarks
	 ** This funtion only affects playlists in a "plain" playing state.
	 ** So if the playlist has been paused, or is under the effect of
	 ** another fade, nothing is done.
	 ** If pause==true, fadeInPlaylist() will restart playback when called.
	 **
	 ** @return
	 ** SS_NO_ERROR			Playlist fade-out started.
	 ** SS_FILE_NOT_FOUND	No playlist "name" found.
	 ** SS_INTERNAL_ERROR	Unspecified error.
	 **/
	SSerror
	fadeOutPlaylist(const Ogre::String& name,
					const Ogre::Real& time,
					const bool pause=true);

	/**
	 ** @brief
	 ** Fades back in the playlist's current sound volume to its original value.
	 **
	 ** @param
	 **  name: name of the playlist
	 **  time: fade-out time, in seconds. If negative defaults to 1.0
	 **
	 ** @remarks
	 ** This function only affects sounds which have been paused,
	 ** or are under the effect of an individual fade out.
	 ** So if the playback is just playing, or is under the effect of
	 ** another fade (e.g. globalFadeOut), nothing is done.
	 ** Playback is restarted if the sound had been faded-out and/or paused.
	 **
	 ** @return
	 ** SS_NO_ERROR			Playlist fade-in started.
	 ** SS_FILE_NOT_FOUND	No playlist "name" found.
	 ** SS_INTERNAL_ERROR	Unspecified error.
	 **/
	SSerror
	fadeInPlaylist(const Ogre::String& name,
				   const Ogre::Real& time);

	/**
	 ** @brief
	 ** Changes the name of the playlist "oldName" to "newName"
	 **
	 ** @return
	 ** true	Playlist successfully renamed.
	 ** false	Playlist "oldName" not found.
	 **/
	bool renamePlaylist(const Ogre::String& oldName, const Ogre::String& newName);

	/**
	 ** @brief
	 ** Retrieve the sounds registered inside the playlist "name"
	 **
	 ** @return
	 ** Empty vector	Playlist "name" not found
	 ** Sounds vector	Otherwise
	 **/
	const std::vector<Ogre::String>
	getPlaylistSounds(const Ogre::String& name) const;

	/**
	 ** @brief
	 ** Get the playlist playing state.
	 **
	 ** @return
	 ** SS_PLAYING		Playing (or fading in/out)
	 ** SS_PAUSED		Paused
	 ** SS_FINISHED		Stopped
	 ** SS_NONE			Playlist not found
	 **/
	SSplayback
	getPlaylistPlayState(const Ogre::String& name) const;

	/**
	 ** @brief
	 ** Set/Get the {repeat-on-end ; random-order ; random-silence} conditions
	 ** for the playlist named "name".
	 **
	 ** @return
	 ** Setters: true	if success.
	 **          false	if playlist "name" not found.
	 ** Getters: condition value
	 **
	 ** @remarks
	 ** For getters, optional boolean "found" parameter will be filled with:
	 ** true	if operation successfull
	 ** false	if playlist "name" not found
	 **/
	bool setPlaylistRepeat(const Ogre::String& name, bool  repeat);
	bool getPlaylistRepeat(const Ogre::String& name, bool* found=0) const;
	bool setPlaylistRandomOrder(const Ogre::String& name, bool  random);
	bool getPlaylistRandomOrder(const Ogre::String& name, bool* found=0) const;
	bool setPlaylistRandomSilence(const Ogre::String& name, bool  random);
	bool getPlaylistRandomSilence(const Ogre::String& name, bool* found=0) const;


	/*********************************************************************/
	/********************    AUXILIARY FUNCTIONS    **********************/
private:
	Ogre::String
	loadSounds(const std::vector<Ogre::String>&, SSbuftype);

	const Playlist*
	getPlaylist(const Ogre::String& name) const;

	Playlist*
	getPlaylist(const Ogre::String& name);

	// Playlists state modifiers
	void setPlaylistState(Playlist *pl, unsigned long flags);
	void unsetPlaylistState(Playlist *pl, unsigned long flags);
	void setPlaylistGlobalState(Playlist *pl, long flags);
	void unsetPlaylistGlobalState(Playlist *pl, long flags);


	/*********************************************************************/
	/***********************    CLASS MEMBERS    *************************/
private:

	typedef void* EnvSoundId;

	static SoundManager&	sSoundManager;
	std::vector<Playlist*>	mPlaylists;
	std::vector<EnvSoundId>	mFinishedPlaylists;
	std::vector<EnvSoundId>	mPausedPlaylists;
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
inline SoundHandler::SoundHandler() { /* Nothig to do. */ }


////////////////////////////////////////////////////////////////////////////////
inline SoundHandler::~SoundHandler() { shutDown(); }


////////////////////////////////////////////////////////////////////////////////
inline std::vector<std::string>
SoundHandler::getAvailableSoundDevices()
{
	return sSoundManager.getAvailableSoundDevices();
}


////////////////////////////////////////////////////////////////////////////////
inline std::string
SoundHandler::getSoundDevice()
{
	return sSoundManager.getSoundDevice();
}


////////////////////////////////////////////////////////////////////////////////
inline SSerror
SoundHandler::setSoundDevice(std::string* devName)
{
	return sSoundManager.setSoundDevice(devName);
}


////////////////////////////////////////////////////////////////////////////////
inline void
SoundHandler::setCamera(const Ogre::Camera* cam)
{
	sSoundManager.setCamera(cam);
}


////////////////////////////////////////////////////////////////////////////////
inline Ogre::String
SoundHandler::loadStreamSounds(const std::vector<Ogre::String>& list)
{
	return loadSounds(list, SSbuftype::SS_BUF_STREAM_OGG);
}


////////////////////////////////////////////////////////////////////////////////
inline Ogre::String
SoundHandler::loadDirectSounds(const std::vector<Ogre::String>& list)
{
	return loadSounds(list, SSbuftype::SS_BUF_LOADED);
}


////////////////////////////////////////////////////////////////////////////////
inline SSerror
SoundHandler::addStreamSources(unsigned int N)
{
	return sSoundManager.addSSoundSources(N);
}


////////////////////////////////////////////////////////////////////////////////
inline SSerror
SoundHandler::addDirectSources(unsigned int N)
{
	return sSoundManager.addLSoundSources(N);
}


////////////////////////////////////////////////////////////////////////////////
inline bool
SoundHandler::existsPlaylist(const Ogre::String& name) const
{
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i]->mName == name) {
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
inline bool
SoundHandler::renamePlaylist(const Ogre::String& oldName, const Ogre::String& newName)
{
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i]->mName == oldName) {
			mPlaylists[i]->mName = newName;
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
inline const std::vector<Ogre::String>
SoundHandler::getPlaylistSounds(const Ogre::String& name) const
{
	const std::vector<Ogre::String> notFound;
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i]->mName == name) {
			return mPlaylists[i]->mList;
		}
	}
	return notFound;
}


////////////////////////////////////////////////////////////////////////////////
inline const SoundHandler::Playlist*
SoundHandler::getPlaylist(const Ogre::String& name) const
{
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i]->mName == name) {
			return reinterpret_cast<const Playlist*>(mPlaylists[i]);
		}
	}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
inline SoundHandler::Playlist*
SoundHandler::getPlaylist(const Ogre::String& name)
{
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i]->mName == name) {
			return mPlaylists[i];
		}
	}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
inline void
SoundHandler::setPlaylistState(Playlist *pl, unsigned long flags) {
	pl->mState |= flags;
}


////////////////////////////////////////////////////////////////////////////////
inline void
SoundHandler::unsetPlaylistState(Playlist *pl, unsigned long flags) {
	pl->mState &= ~flags;
}


////////////////////////////////////////////////////////////////////////////////
inline void
SoundHandler::setPlaylistGlobalState(SoundHandler::Playlist *pl, long flags) {
	pl->mGState |= flags;
}


////////////////////////////////////////////////////////////////////////////////
inline void
SoundHandler::unsetPlaylistGlobalState(SoundHandler::Playlist *pl, long flags) {
	pl->mGState &= ~flags;
}

}

#endif /* SOUNDHANDLER_H_ */
