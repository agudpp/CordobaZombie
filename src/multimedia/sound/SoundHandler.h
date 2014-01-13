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
#include <random_generator/RandomGenerator.h>


// Forward declarations
namespace rrh {
class ResourceHandler;
}

namespace mm {

typedef std::pair<const Ogre::String,const int> SingleSoundHandle;
static const SingleSoundHandle INVALID_HANDLE;  // Invalid SingleSoundHandle
static const int HANDLER_MIN_CACHE_SIZE = 10;  // Mem reservation for some vectors

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
				 float silence=0.0f,
				 float gain=DEFAULT_ENV_GAIN);
		Playlist(const Playlist& pl);
		Playlist& operator=(const Playlist& pl);
		~Playlist(void);
	public:
		Ogre::String 				mName;
		std::vector<Ogre::String>	mList;      // Sounds names list
		std::vector<unsigned int>   mPlayOrder; // Sounds playing order
		unsigned int				mCurrent;   // Current position in mPlayOrder
		unsigned int 			    mState;     // Repeat/Shuffle/Randomwait
		float                       mSilence;   // Wait time between sounds (sec)
		float                       mTimeSinceFinish;
		float                       mGain;
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

	/**
	 ** @brief Return the instance of the SoundManager associated to this class
	 **
	 */
	inline SoundManager*
	soundManager(void);
	inline const SoundManager*
	soundManager(void) const;

    /**
     ** @brief
     ** Set the ResourceHandler to be used for audio files loading.
     **/
    void
    setResourceHandler(rrh::ResourceHandler* rh);

    /**
     ** @brief
     ** Set the OpenALHandler to be used for sounds playback in an audio device
     **/
    void
    setOpenALHandler(OpenALHandler* handler);

    /**
     ** @brief
     ** Set the camera attached to the SoundManager.
     **
     ** @remarks
     ** The camera determines the position and orientation of the listener,
     ** both of which get updated on each call to SceneManager::update()
     **/
    void
    setCamera(const Ogre::Camera* cam);

	/**
	 ** @brief Lists available sound devices.
	 **/
	std::vector<std::string>
	getAvailableSoundDevices() const;

	/**
	 ** @brief
	 ** Name of the currently open sound device. There's always one.
	 **
	 ** @return
	 ** Empty string if ALC_ENUMERATION_EXT extension is not present;
	 ** name of the opened sound device otherwise.
	 **/
	std::string
	getSoundDevice() const;

    /**
     ** @brief
     ** Tells whether the ResourceHandler is ready to search for files
     **/
    bool
    hasResourceHandler() const;

	/**
	 ** @brief
	 ** Tells whether the OpenAL system is set up correctly
	 **/
	bool
	hasOpenALcontext() const;

	/**
	 ** @brief Return the last error set internally.
	 **/
	const SSerror
	getError() const;


	/*********************************************************************/
	/*****************    SOUNDS LOADING/UNLOADING    ********************/
public:
	/**
	 ** @brief Loads a list of sound files, to be played in streaming fashion
	 **
	 ** @param  list    Names of the sound files to load
     **
     ** @return
     ** List of files which failed to load, with respective error message.
     ** File names are separated with UNIX newline characters (viz. '\n')
     **
	 ** @remarks
	 ** Filenames are expected to contain the sound format extension
	 ** (e.g., *.mp3, *.wav, *.ogg, etc.)
	 **/
	Ogre::String
	loadStreamSounds(const std::vector<Ogre::String>& list);

	/**
	 ** @brief Loads a list of sound files, to be played directly from memory
	 **
     ** @param  list    Names of the sound files to load
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
    /*************    SINGLE-SOUND PLAYBACK CONTROLS    ******************/
public:
    /**
     ** @brief
     ** Plays audio file "sName" as a single environmental sound.
     ** i.e. no orientation, no distance fade.
     **
     ** @param
     **  sName: name of the audio file to play
     **   gain: volume of the sound, in [ 0.0 , 1.0 ] scale
     ** repeat: whether to repeat on end
     **
     ** @return
     ** INVALID_HANDLE on error | Handle of generated sound otherwise
     **
     ** @remarks
     ** Sound "sName" should have already been loaded with loadSound()
     **
     ** @remarks
     ** If INVALID_HANDLE is returned, getError() will return the specific
     ** error code generated:
     **  ¤ SS_NO_ERROR         Playback started
     **  ¤ SS_NO_SOURCES       No available sources to play sound.
     **  ¤ SS_FILE_NOT_FOUND   "sName" not found, no buffer "sName" loaded
     **  ¤ SS_INTERNAL_ERROR   Unspecified
     **/
	SingleSoundHandle
	playSound(const Ogre::String& sName,
              const Ogre::Real& gain = DEFAULT_ENV_GAIN,
              bool repeat = false);


	// TODO implement SingleSound methods from here downwards

    /**
     ** @brief
     ** Pauses single sound
     **
     ** @param
     ** h: handle to the single sound to pause
     **
     ** @return
     ** SS_NO_ERROR         Single sound successfully paused.
     ** SS_NO_BUFFER        "h" didn't match any single sound handle.
     ** SS_INTERNAL_ERROR   Unspecified.
     **
     ** @remarks
     ** If the handle did not match any single sound, nothing is done.
     ** This function overrides fadings.
     **/
    void
    pauseSound(const SingleSoundHandle& h);

    /**
     ** @brief
     ** Stops single sound
     **
     ** @param
     ** h: handle to the single sound to stop
     **
     ** @return
     ** SS_NO_ERROR         Single sound stopped, resources released.
     ** SS_NO_BUFFER        "h" didn't match any single sound handle.
     ** SS_INTERNAL_ERROR   Unspecified.
     **
     ** @remarks
     ** The single sound is destroyed, "h" is no longer a valid handle.
     ** If the handle did not match any single sound, nothing is done.
     **/
    SSerror
    stopEnvSound(SingleSoundHandle& h);

    /**
     ** @brief
     ** Restarts single sound
     **
     ** @param
     ** h: handle to the single sound to restart
     **
     ** @return
     ** SS_NO_ERROR         Single sound successfully restarted.
     ** SS_NO_BUFFER        "h" didn't match any single sound handle.
     ** SS_INTERNAL_ERROR   Unspecified.
     **
     ** @remarks
     ** If the sound was paused or playing, playback restarts from square one.
     ** If the handle did not match any single sound, nothing is done.
     ** This function overrides fadings, both individual and global.
     **/
    SSerror
    restartEnvSound(const SingleSoundHandle& h);

    /**
     ** @brief
     ** Fades out single sound
     **
     ** @param
     **     h: handle to the single sound to fade out
     **  time: fade out time, in seconds. If negative, defaults to 1.0
     ** pause: whether to pause the sound once muted. Default: true.
     **
     ** @return
     ** SS_NO_ERROR         Sound fade out started, or in an unexpected state
     **                     (and was left untouched)
     ** SS_NO_BUFFER        "h" didn't match any single sound handle.
     ** SS_INTERNAL_ERROR   Unspecified.
     **
     ** @remarks
     ** This funtion only affects sounds in a "plain" playing state.
     ** So if the sound has been paused, or is under the effect of some other
     ** fade, nothing is done.
     ** If pause==true, fadeInSound() will restart playback when called.
     ** If the handle did not match any single sound, nothing is done.
     **/
    SSerror
    fadeOutSound(const SingleSoundHandle& h,
                 const Ogre::Real& time = 1.0,
                 const bool pause = true);

    /**
     ** @brief
     ** Fades back in single sound to its original playback volume
     **
     ** @param
     **    h: handle to the single sound to fade in
     ** time: fade in time, in seconds. If negative, defaults to 1.0
     **
     ** @return
     ** SS_NO_ERROR         Sound fade in started, or in an unexpected state
     **                     (and was left untouched)
     ** SS_NO_BUFFER        "h" didn't match any single sound handle.
     ** SS_INTERNAL_ERROR   Unspecified.
     **
     ** @remarks
     ** This function only affects sounds which have been paused,
     ** or are under the effect of an individual fade out.
     ** So if the sound is just playing, or is under the effect of
     ** another fade, nothing is done.
     ** Playback is restarted if the sound had been faded-out and/or paused.
     ** If the handle did not match any single sound, nothing is done.
     **/
    SSerror
    fadeInSound(const SingleSoundHandle& h, const Ogre::Real& time);


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
	 ** This doesn't affect inactive (i.e. stopped or "finished") sounds.
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
	 ** MAY NOT be modified by this function.
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
     ** gain: playback volume in [ 0.0 , 1.0 ] scale
	 **   pl: optional, provide the Playlist pointer to save on search time.
	 **
	 ** @return
	 ** SS_NO_ERROR			  Playback successfully started.
	 ** SS_FILE_NOT_FOUND	  No playlist "name" found.
	 ** SS_ILLEGAL_OPERATION  Operation not allowed in current playlist state.
	 ** SS_INTERNAL_ERROR	  Unspecified error.
	 **/
	SSerror
	startPlaylist(const Ogre::String& name,
	              const Ogre::Real& gain = DEFAULT_ENV_GAIN,
	              Playlist *pl = 0);

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
	 ** SS_PLAYING		Playing (maybe in silence, or fading in/out)
	 ** SS_PAUSED		Paused  (maybe in silence)
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
	loadSounds(const std::vector<Ogre::String>&,
	           SSbuftype);

	const Playlist*
	getPlaylist(const Ogre::String& name) const;

	Playlist*
	getPlaylist(const Ogre::String& name);

	// Playlists state modifiers
	void setPlaylistState(Playlist *pl, unsigned long flags);
	void unsetPlaylistState(Playlist *pl, unsigned long flags);

	// Playlists state consistency checker (DEBUG build only!)
	void checkPlaylistState(const Playlist *pl) const;


	/*********************************************************************/
	/***********************    CLASS MEMBERS    *************************/
private:

	typedef void* EnvSoundId;

	static SoundManager&	sSoundManager;
	SSerror                 mLastError;
    tool::RandomGenerator   mRNG;
	std::vector<Playlist*>	mPlaylists;
	std::vector<EnvSoundId>	mFinishedPlaylists;
	std::vector<EnvSoundId>	mPausedPlaylists;
    std::vector<SingleSoundHandle> mSingles;
    std::list<int> mFreshSingleSoundIds;
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


inline SoundManager*
SoundHandler::soundManager(void)
{
    return &sSoundManager;
}
inline const SoundManager*
SoundHandler::soundManager(void) const
{
    return &sSoundManager;
}


////////////////////////////////////////////////////////////////////////////////
inline SoundHandler::SoundHandler() :
    mLastError(SSerror::SS_NO_ERROR)
{
    mPlaylists.reserve(HANDLER_MIN_CACHE_SIZE);
    mSingles.reserve(HANDLER_MIN_CACHE_SIZE);
    for (int i=0 ; i < HANDLER_MIN_CACHE_SIZE ; i++)
        mFreshSingleSoundIds.push_back(i);
}


////////////////////////////////////////////////////////////////////////////////
inline SoundHandler::~SoundHandler()
{
    if (mPlaylists.size() > 0 && hasOpenALcontext()) {
        // They forgot to shut us down?
        debugWARNING("Destroying SoundHandler, shutDown() forced.\n");
        shutDown();
    }
}


////////////////////////////////////////////////////////////////////////////////
inline void
SoundHandler::setResourceHandler(rrh::ResourceHandler* rh)
{
    sSoundManager.setResourceHandler(rh);
}


////////////////////////////////////////////////////////////////////////////////
inline void
SoundHandler::setOpenALHandler(OpenALHandler* handler)
{
    sSoundManager.setOpenALHandler(handler);
}


////////////////////////////////////////////////////////////////////////////////
inline void
SoundHandler::setCamera(const Ogre::Camera* cam)
{
    sSoundManager.setCamera(cam);
}


////////////////////////////////////////////////////////////////////////////////
inline std::vector<std::string>
SoundHandler::getAvailableSoundDevices() const
{
	return sSoundManager.getAvailableSoundDevices();
}


////////////////////////////////////////////////////////////////////////////////
inline std::string
SoundHandler::getSoundDevice() const
{
	return sSoundManager.getSoundDevice();
}


////////////////////////////////////////////////////////////////////////////////
inline bool
SoundHandler::hasResourceHandler() const
{
    return sSoundManager.hasResourceHandler();
}


////////////////////////////////////////////////////////////////////////////////
inline bool
SoundHandler::hasOpenALcontext() const
{
    return sSoundManager.hasOpenALcontext();
}


////////////////////////////////////////////////////////////////////////////////
inline const SSerror SoundHandler::getError() const { return mLastError; }


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
	for (unsigned int i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i]->mName == name) {
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
inline SSerror
SoundHandler::restartPlaylist(const Ogre::String& name, Playlist *pl)
{
	stopPlaylist(name, pl);
	return startPlaylist(name, (pl ? pl->mGain : DEFAULT_ENV_GAIN), pl);
}


////////////////////////////////////////////////////////////////////////////////
inline bool
SoundHandler::renamePlaylist(const Ogre::String& oldName, const Ogre::String& newName)
{
	for (unsigned int i=0 ; i < mPlaylists.size() ; i++) {
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
	for (unsigned int i=0 ; i < mPlaylists.size() ; i++) {
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
	for (unsigned int i=0 ; i < mPlaylists.size() ; i++) {
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
	for (unsigned int i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i]->mName == name) {
			return mPlaylists[i];
		}
	}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
inline void
SoundHandler::setPlaylistState(Playlist *pl, unsigned long flags)
{
	pl->mState |= flags;
}


////////////////////////////////////////////////////////////////////////////////
inline void
SoundHandler::unsetPlaylistState(Playlist *pl, unsigned long flags)
{
	pl->mState &= ~flags;
}


////////////////////////////////////////////////////////////////////////////////
#ifndef DEBUG
inline void SoundHandler::checkPlaylistState(const Playlist *pl) const {}
// else "checkPlaylistState(pl)" is defined in the source file
#endif

}

#endif /* SOUNDHANDLER_H_ */
