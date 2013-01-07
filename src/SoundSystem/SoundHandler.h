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
#include "SoundEnums.h"
#include "SoundManager.h"
#include "SoundSource.h"

class SoundHandler
{
	struct Playlist
	{
		Playlist(const Ogre::String& name="");
		Playlist(const Ogre::String& name,
				 const std::vector<Ogre::String>& list,
				 bool  repeat=true,
				 bool  random=false,
				 float silence=0.0f);
		~Playlist(void);
	public:
		Ogre::String 				mName;
		std::vector<Ogre::String>	mList;		// Sounds names list
		std::vector<uint>			mPlayOrder; // Sounds playing order
		unsigned int				mCurrent;	// Current position in mPlayOrder
		bool						mRepeat;	// Repeat playback on end
		bool						mRandom;	// Shuffle the vector mPlayOrder
		float						mSilence;	// Time to wait between sounds
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
	 ** File names are separated with UNIX newline characters (i.e., '\n')
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
	 ** File names are separated with UNIX newline characters (i.e., '\n')
	 **/
	Ogre::String
	loadDirectSounds(const std::vector<Ogre::String>& list);

	/**
	 ** @brief
	 ** Unloads from the system the given list of sounds.
	 ** Sounds which had not been previously loaded are simply ignored.
	 **
	 ** @remarks
	 ** If some sound was the content of the buffer of a currently playing
	 ** SoundSource, calling this function will result in undefined behavior.
	 **/
	void
	unloadSounds(const std::vector<Ogre::String>& list);

	/**
	 ** @brief
	 ** Stop all playbacks, release all resources, and get the hell out.
	 **/
	void
	shutDown(void);


	/*********************************************************************/
	/****************    GLOBAL PLAYBACK CONTROLS    *********************/
public:
	/* XXX - Stubs to the SoundManager methods. */

	/**
	 ** @brief
	 ** Pauses ("globally") all currently playing sounds.
	 **/
	inline void
	globalPause() const;

	/**
	 ** @brief
	 ** Plays again ("globally") all paused sounds.
	 **
	 ** @remarks
	 ** Sounds, both environmental and attached to a unit's SoundAPI,
	 ** which were paused manually (i.e. before calling globalPause()),
	 ** will NOT start playing again with this function.
	 **/
	inline void
	globalPlay() const;

	/**
	 ** @brief
	 ** Stops all sounds.
	 **
	 ** @remarks
	 ** BEWARE! this function frees all playback resources.
	 ** That means that all active sources and buffers get released.
	 ** Playbacks however are not erased, but they are reset.
	 **/
	inline void
	globalStop() const;

	/**
	 ** @brief
	 ** Restarts all active sounds in the system.
	 **
	 ** @remarks
	 ** This affects both playing and paused sounds.
	 **/
	inline void
	globalRestart() const;

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
	inline void
	globalFadeOut(const Ogre::Real& time, const bool pause=true) const;

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
	inline void
	globalFadeIn(const Ogre::Real& time) const;



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
	 **    silence: amount of time to wait between sounds playback
	 **
	 ** @return
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
	 ** @return
	 ** SS_NO_ERROR			Playback successfully started.
	 ** SS_FILE_NOT_FOUND	No playlist "name" found.
	 ** SS_INTERNAL_ERROR	Unspecified error.
	 **/
	SSerror
	startPlaylist(const Ogre::String& name) const;

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
	 **/
	void
	pausePlaylist(const Ogre::String& name) const;

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
	 **/
	void
	stopPlaylist(const Ogre::String& name) const;

	/**
	 ** @brief
	 ** Restarts playlist.
	 **
	 ** @remarks
	 ** Same effect than stopPlaylist + startPlaylist, but more efficient
	 ** (here no resources are released and re-created)
	 **
	 ** @return
	 ** SS_NO_ERROR			Playback successfully restarted.
	 ** SS_FILE_NOT_FOUND	No playlist "name" found.
	 ** SS_INTERNAL_ERROR	Unspecified error.
	 **/
	SSerror
	restartPlaylist(const Ogre::String& name) const;

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
					const bool pause=true) const;

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
				   const Ogre::Real& time) const;

private:
	static SoundManager&	sSoundManager;
	std::vector<Playlist>	mPlaylists;
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
inline SoundHandler::~SoundHandler() { /* TODO: invoke shutDown(); */ }


////////////////////////////////////////////////////////////////////////////////
inline
SoundHandler::Playlist::Playlist(const Ogre::String& name) :
	mName(name),
	mCurrent(0),
	mRepeat(true),
	mRandom(false),
	mSilence(0.0f)
{}


////////////////////////////////////////////////////////////////////////////////
inline
SoundHandler::Playlist::Playlist(const Ogre::String& name,
								 const std::vector<Ogre::String>& list,
								 bool  repeat,
								 bool  random,
								 float silence) :
	mName(name),
	mList(list),
	mCurrent(0),
	mRepeat(repeat),
	mRandom(random),
	mSilence(silence)
{
	// Initialize play order
	mPlayOrder.resize(list.size());
	for (uint i=0 ; i < list.size() ; i++) { mPlayOrder.push_back(i); }
	// If random was chosen, shuffle the play order
	if (random) { std::random_shuffle(mPlayOrder.begin(),mPlayOrder.end()); }
}


////////////////////////////////////////////////////////////////////////////////
inline
SoundHandler::Playlist::~Playlist(void)
{
	// Stop anything we were playing.
	// TODO
	debugWARNING("TODO\n");
}


////////////////////////////////////////////////////////////////////////////////
inline void SoundHandler::globalPause() const { sSoundManager.globalPause(); }

////////////////////////////////////////////////////////////////////////////////
inline void SoundHandler::globalPlay() const { sSoundManager.globalPlay(); }

////////////////////////////////////////////////////////////////////////////////
inline void SoundHandler::globalStop() const { sSoundManager.globalStop(); }

////////////////////////////////////////////////////////////////////////////////
inline void SoundHandler::globalRestart() const { sSoundManager.globalRestart(); }

////////////////////////////////////////////////////////////////////////////////
inline void
SoundHandler::globalFadeOut(const Ogre::Real& time, const bool pause) const
{
	sSoundManager.globalFadeOut(time, pause);
}

////////////////////////////////////////////////////////////////////////////////
inline void
SoundHandler::globalFadeIn(const Ogre::Real& time) const
{
	sSoundManager.globalFadeIn(time);
}


#endif /* SOUNDHANDLER_H_ */
