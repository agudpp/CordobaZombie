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

// Handles returned to the user on sound creation
typedef std::pair<const Ogre::String,const int> SingleSoundHandle;
typedef SingleSoundHandle PlaylistHandle;
static const SingleSoundHandle INVALID_HANDLE;

// For internal containers memory policies:
static const int HANDLER_MIN_CACHE_SIZE = 10;


// Main class of the sound system
class SoundHandler
{
    struct SingleSound
    {
        // TODO implement this struct in the source file
        // XXX  Plagiarize the Playlist struct
        SingleSound(const Ogre::String& soundName="");  // repeat = true
        SingleSound(const Ogre::String& soundName,
                    bool  repeat = true,
                    float gain = DEFAULT_ENV_GAIN);
        SingleSound(const SingleSound& ss);
        SingleSound& operator=(const SingleSound& ss);
        ~SingleSound();
    public:
        Ogre::String                mSoundName;
        float                       mGain;
        SingleSoundHandle           mHandle;  // For user referencing
        SoundManager::EnvSoundId    mPlayID;  // For SoundManager internals
    };

	struct Playlist
	{
        // TODO reimplement this struct to support changes:
        //        · two new members: mHandle and mPlayID
        //        · one member deleted: mName
		Playlist();  // repeat = true
		Playlist(const std::vector<Ogre::String>& list,
				 bool  repeat = true,
				 bool  randomOrder = false,
				 bool  randomSilence = false,
				 float silence = 0.0f,
				 float gain = DEFAULT_ENV_GAIN);
		Playlist(const Playlist& pl);
		Playlist& operator=(const Playlist& pl);
		~Playlist(void);
	public:
		std::vector<Ogre::String>	mList;      // Sounds names list
		std::vector<unsigned int>   mPlayOrder; // Sounds playing order
		unsigned int				mCurrent;   // Current position in mPlayOrder
		unsigned int 			    mState;     // Repeat/Shuffle/Randomwait
		float                       mSilence;   // Wait time between sounds (sec)
		float                       mTimeSinceFinish;
		float                       mGain;
		PlaylistHandle              mHandle;  // For user referencing
        SoundManager::EnvSoundId    mPlayID;  // For SoundManager internals
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

	// TODO implement EVERY single-sound method in the source file
	// XXX  READ THE HEADER COMMENTS, everything is said there
    // XXX  Copy the state-transition logic from the Playlist methods!!!
	// XXX  I'd better deal with Playlists first (see below)

	/**
	 ** @brief
	 ** Creates a new SingleSound from audio data
	 **
	 ** @param
	 **  sName: name of the sound the SingleSound will contain
	 **   gain: default playback volume for this sound
	 ** repeat: whether to repeat on end
	 **
     ** @return
     ** INVALID_HANDLE on error | Handle of generated SingleSound otherwise
     **
     ** @remarks
     ** Sound "sName" should have been loaded into the sound system beforehand,
     ** with either loadDirectSounds() or loadStreamSounds()
     ** If INVALID_HANDLE is returned, getError() will show an error code
     ** describing what went wrong internally:
     **  · SS_NO_ERROR         Things are OK! Why did you call getError()?
     **  · SS_NO_MEMORY        System ran out of memory. Go buy some, quick!
     **  · SS_FILE_NOT_FOUND   "sName" not found, no such sound was loaded
     **  · SS_INTERNAL_ERROR   Unspecified
	 **/
	SingleSoundHandle
	newSound(const Ogre::String& sName,
	         float gain  = DEFAULT_ENV_GAIN,
	         bool repeat = true);

	/**
	 ** @brief
	 ** Whether the handle points to a valid SingleSound in the system
	 **/
	bool
	existsSound(const SingleSoundHandle& h) const;

    /**
     ** @brief
     ** Removes the SingleSound from the sound system
     **
     ** @remarks
     ** Any playback is stopped before deletion.
     ** If "h" didn't point to any valid SingleSound, nothing is done.
     ** If it did, it becomes the INVALID_HANDLE when method returns.
     **/
    void
    deleteSound(SingleSoundHandle& h);

    /**
     ** @brief
     ** Starts playing the SingleSound
     **
     ** @return
     ** SS_NO_ERROR             Playback started.
     ** SS_ILLEGAL_OPERATION    Unexpected playback state: nothing was done.
     ** SS_NO_SOURCES           No available sources to play sound.
     ** SS_INVALID_HANDLE       "h" didn't point to a valid SingleSound.
     ** SS_INTERNAL_ERROR       Unspecified.
     **
     ** @remarks
     ** If the SingleSound had been paused, playback is resumed from pause point
     ** If the SingleSound had been stopped, it restarts from the begginning.
     ** If the SingleSound is currently playing, nothing is done.
     **/
	SSerror
	startSound(SingleSoundHandle& h);

    /**
     ** @brief
     ** Pauses the SingleSound
     **
     ** @return
     ** SS_NO_ERROR         SingleSound paused.
     ** SS_INVALID_HANDLE   "h" didn't point to a valid SingleSound.
     ** SS_INTERNAL_ERROR   Unspecified.
     **
     ** @remarks
     ** If "h" didn't point to a valid SingleSound, nothing is done.
     ** This function overrides fadings.
     **/
    SSerror
    pauseSound(SingleSoundHandle& h);

    /**
     ** @brief
     ** Stops the SingleSound
     **
     ** @return
     ** SS_NO_ERROR         SingleSound stopped, resources released.
     ** SS_INVALID_HANDLE   "h" didn't point to a valid SingleSound.
     ** SS_INTERNAL_ERROR   Unspecified.
     **
     ** @remarks
     ** Internal resources are released, restarting may be expensive.
     ** If "h" didn't point to a valid SingleSound, nothing is done.
     **/
    SSerror
    stopSound(SingleSoundHandle& h);

    /**
     ** @brief
     ** Restarts the SingleSound
     **
     ** @return
     ** SS_NO_ERROR         SingleSound restarted.
     ** SS_INVALID_HANDLE   "h" didn't point to a valid SingleSound.
     ** SS_INTERNAL_ERROR   Unspecified.
     **
     ** @remarks
     ** If the sound was paused or playing, playback restarts from square one.
     ** If "h" didn't point to a valid SingleSound, nothing is done.
     ** This function overrides fadings, both individual and global.
     **/
    SSerror
    restartSound(SingleSoundHandle& h);

    /**
     ** @brief
     ** Fades out the SingleSound
     **
     ** @param
     **     h: handle to the SingleSound to fade out
     **  time: fade out time, in seconds. If negative defaults to 1.0
     ** pause: whether to pause the sound once muted.
     **
     ** @return
     ** SS_NO_ERROR             SingleSound fade out started.
     ** SS_ILLEGAL_OPERATION    Unexpected playback state: nothing was done.
     ** SS_INVALID_HANDLE       "h" didn't point to a valid SingleSound.
     ** SS_INTERNAL_ERROR       Unspecified.
     **
     ** @remarks
     ** This funtion only affects sounds in a "plain" playing state.
     ** So if the sound has been paused, or is under the effect of some other
     ** fade (e.g. global fade in) nothing is done.
     ** If pause==true, fadeInSound() will restart playback when called.
     ** If "h" didn't point to a valid SingleSound, nothing is done.
     **/
    SSerror
    fadeOutSound(SingleSoundHandle& h, float time = 1.0, bool pause = true);

    /**
     ** @brief
     ** Fades back in the SingleSound to its original playback volume
     **
     ** @param
     **    h: handle to the SingleSound to fade in
     ** time: fade in time, in seconds. If negative defaults to 1.0
     **
     ** @return
     ** SS_NO_ERROR             SingleSound fade in started.
     ** SS_ILLEGAL_OPERATION    Unexpected playback state: nothing was done.
     ** SS_INVALID_HANDLE       "h" didn't point to a valid SingleSound.
     ** SS_INTERNAL_ERROR       Unspecified.
     **
     ** @remarks
     ** This function only affects sounds which have been paused,
     ** or are under the effect of an individual fade out.
     ** So if the sound is just playing, or is under the effect of some other
     ** fade (e.g. global fade in) nothing is done.
     ** Playback is restarted if the sound had been faded-out and/or paused.
     ** If "h" didn't point to a valid SingleSound, nothing is done.
     **/
    SSerror
    fadeInSound(SingleSoundHandle& h, float time = 1.0);

    /**
     ** @brief
     ** Retrieve the name of the sound this SingleSound contains
     **
     ** @return
     ** Empty string    "h" didn't match any SingleSound handle.
     ** Sound name      Otherwise
     **/
    const Ogre::String
    getSoundSoundname(const SingleSoundHandle& h) const;

    /**
     ** @brief
     ** Get the SingleSound playing state.
     **
     ** @return
     ** SS_PLAYING      Playing (maybe fading in/out)
     ** SS_PAUSED       Paused
     ** SS_FINISHED     Stopped
     ** SS_NONE         Playlist not found
     **/
    const SSplayback
    getSoundPlayState(const SingleSoundHandle& name) const;

    /**
     ** @brief
     ** Set/Get the SingleSound {gain, repeat-on-end}
     **
     ** @return
     ** Setters: true on success | false if handle "h" was invalid
     ** Getters: condition value
     **
     ** @remarks
     ** For getters, optional boolean "found" parameter will be filled with:
     ** true    if operation successfull
     ** false   if "h" didn't point to a valid SingleSound.
     **/
    bool setSoundGain(const SingleSoundHandle& h, float gain);
    float getSoundGain(const SingleSoundHandle& h, bool* found=0) const;
    bool setSoundRepeat(const SingleSoundHandle& h, bool  repeat);
    bool getSoundRepeat(const SingleSoundHandle& h, bool* found=0) const;


    /*********************************************************************/
    /*************************    PLAYLISTS    ***************************/
public:

    // TODO: reimplement EVERY playlist method in the source file
    // XXX  READ THE HEADER COMMENTS, everything is said there
    // XXX  But mantain the state-transition logic of the current methods!!!
    // XXX  BTW, move all the implementations into the source file

    /**
     ** @brief
     ** Creates a new Playlist from given sounds list.
     **
     ** @param
     ** soundsList: names of the sounds the Playlist will contain
     **     repeat: whether to repeat on end (of Playlist)
     **     random: whether to play in random order
     **    silence: time to wait (in seconds) between sounds playback.
     **             If negative value given, silence time is randomized
     **             after each play, within the range [MIN_SILENCE,MAX_SILENCE]
     **      fails: sounds from the list which could not be loaded
     **
     ** @return
     ** INVALID_HANDLE on error | Handle of generated Playlist otherwise
     **
     ** @remarks
     ** The sounds in the list should have been loaded into the sound system
     ** beforehand, with either loadDirectSounds() or loadStreamSounds()
     ** If INVALID_HANDLE is returned, getError() will show an error code
     ** describing what went wrong internally:
     **  · SS_NO_ERROR         Things are OK! Why did you call getError()?
     **  · SS_NO_MEMORY        System ran out of memory. Go buy some, quick!
     **  · SS_FILE_NOT_FOUND   None of the sounds in the list could be loaded.
     **  · SS_INTERNAL_ERROR   Unspecified.
     ** If @param fails is given and some sounds from the list are not found,
     ** these (toghether with the corresponding error messages) are listed
     ** there. UNIX newline character ('\n') is the separator.
     **/
    PlaylistHandle
    newPlaylist(const std::vector<Ogre::String>& soundsList,
                float gain    = DEFAULT_ENV_GAIN,
                bool  repeat  = true,
                bool  random  = false,
                float silence = 0.0f,
                Ogre::String* fails = 0);

    /**
     ** @brief
     ** Whether the handle points to a valid Playlist in the system
     **/
    bool
    existsPlaylist(const PlaylistHandle& h) const;

    /**
     ** @brief
     ** Removes the Playlist from the sound system
     **
     ** @remarks
     ** Any playback is stopped before deletion.
     ** If "h" didn't point to any valid Playlist, nothing is done.
     ** If it did, it becomes the INVALID_HANDLE when method returns.
     **/
    void
    deletePlaylist(PlaylistHandle& h);

    /**
     ** @brief
     ** Starts Playlist playback.
     **
     ** @return
     ** SS_NO_ERROR             Playback started.
     ** SS_ILLEGAL_OPERATION    Unexpected playback state: nothing was done.
     ** SS_NO_SOURCES           No available sources to play sound.
     ** SS_INVALID_HANDLE       "h" didn't point to a valid Playlist.
     ** SS_INTERNAL_ERROR       Unspecified.
     **
     ** @remarks
     ** If the Playlist had been paused, playback is resumed from pause point.
     ** If the Playlist had been stopped, it restarts from the begginning.
     ** If the Playlist is currently playing, nothing is done.
     ** If "h" didn't point to a valid Playlist, nothing is done.
     **/
    SSerror
    startPlaylist(PlaylistHandle& h);

    /**
     ** @brief
     ** Pauses the Playlist.
     **
     ** @return
     ** SS_NO_ERROR         Playlist paused.
     ** SS_INVALID_HANDLE   "h" didn't point to a valid Playlist.
     ** SS_INTERNAL_ERROR   Unspecified.
     **
     ** @remarks
     ** If startPlaylist() is invoked after calling pausePlaylist(),
     ** playback resumes from where it paused (natürlich!)
     ** If "h" didn't point to a valid Playlist, nothing is done.
     **/
    SSerror
    pausePlaylist(PlaylistHandle& h);

    /**
     ** @brief
     ** Stops the Playlist.
     **
     ** @return
     ** SS_NO_ERROR         Playlist stopped, resources released.
     ** SS_INVALID_HANDLE   "h" didn't point to a valid SingleSound.
     ** SS_INTERNAL_ERROR   Unspecified.
     **
     ** @remarks
     ** Playlist current position is lost, and internal resources are released.
     ** If startPlaylist() is invoked after calling stopPlaylist(),
     ** playback starts from the begginning of the Playlist.
     ** If "h" didn't point to a valid Playlist, nothing is done.
     **/
    SSerror
    stopPlaylist(PlaylistHandle& h);

    /**
     ** @brief
     ** Restarts the Playlist.
     **
     ** @return
     ** SS_NO_ERROR         Playlist restarted.
     ** SS_INVALID_HANDLE   "h" didn't point to a valid Playlist.
     ** SS_INTERNAL_ERROR   Unspecified.
     **
     ** @remarks
     ** Same effect than { stopPlaylist(); startPlaylist(); }
     ** If "h" didn't point to a valid Playlist, nothing is done.
     ** This function overrides fadings, both individual and global.
     **/
    SSerror
    restartPlaylist(PlaylistHandle& h);

    /**
     ** @brief
     ** Fades out the playback volume of the Playlist's current sound.
     **
     ** @param
     **     h: handle to the Playlist to fade out
     **  time: fade out time, in seconds. If negative defaults to 1.0
     ** pause: whether to pause the sound once muted.
     **
     ** @return
     ** SS_NO_ERROR             Playlist fade out started.
     ** SS_ILLEGAL_OPERATION    Unexpected playback state: nothing was done.
     ** SS_INVALID_HANDLE       "h" didn't point to a valid Playlist.
     ** SS_INTERNAL_ERROR       Unspecified.
     **
     ** @remarks
     ** This funtion only affects Playlists in a "plain" playing state.
     ** So if the Playlist has been paused, or is under the effect of some
     ** other fade (e.g. global fade in) nothing is done.
     ** If pause==true, fadeInSound() will restart playback when called.
     ** If "h" didn't point to a valid Playlist, nothing is done.
     **/
    SSerror
    fadeOutPlaylist(PlaylistHandle& h, float time = 1.0, bool pause = true);

    /**
     ** @brief
     ** Fades back in the Playlist current sound to its original playback volume
     **
     ** @param
     **    h: handle to the Playlist to fade in
     ** time: fade in time, in seconds. If negative defaults to 1.0
     **
     ** @return
     ** SS_NO_ERROR             SingleSound fade in started.
     ** SS_ILLEGAL_OPERATION    Unexpected playback state: nothing was done.
     ** SS_INVALID_HANDLE       "h" didn't point to a valid SingleSound.
     ** SS_INTERNAL_ERROR       Unspecified.
     **
     ** @remarks
     ** This function only affects Playlists which have been paused,
     ** or are under the effect of an individual fade out.
     ** So if the Playlist is just playing, or is under the effect of some
     ** other fade (e.g. global fade in) nothing is done.
     ** Playback is restarted if the sound had been faded-out and/or paused.
     ** If "h" didn't point to a valid SingleSound, nothing is done.
     **/
    SSerror
    fadeInPlaylist(PlaylistHandle& h, float time = 1.0);

    /**
     ** @brief
     ** Retrieve the sounds successfully loaded inside the Playlist
     **
     ** @return
     ** Empty vector    Playlist "name" not found
     ** Sounds vector   Otherwise
     **/
    const std::vector<Ogre::String>
    getPlaylistSoundlist(const PlaylistHandle& h) const;

    /**
     ** @brief
     ** Get the Playlist playing state.
     **
     ** @return
     ** SS_PLAYING      Playing (maybe in silence, or fading in/out)
     ** SS_PAUSED       Paused  (maybe in silence)
     ** SS_FINISHED     Stopped
     ** SS_NONE         Playlist not found
     **/
    SSplayback
    getPlaylistPlayState(const Ogre::String& name) const;

    /**
     ** @brief
     ** Set/Get the Playlist {gain, repeat-on-end, random-order, random-silence}
     **
     ** @return
     ** Setters: true on success | false if handle "h" was invalid
     ** Getters: condition value
     **
     ** @remarks
     ** For getters, optional boolean "found" parameter will be filled with:
     ** true    if operation successfull
     ** false   if "h" didn't point to a valid Playlist.
     **/
    bool setPlaylistGain(const PlaylistHandle& h, float gain);
    float getPlaylistGain(const PlaylistHandle& h, bool* found=0) const;
    bool setPlaylistRepeat(const PlaylistHandle& h, bool  repeat);
    bool getPlaylistRepeat(const PlaylistHandle& h, bool* found=0) const;
    bool setPlaylistRandomOrder(const PlaylistHandle& h, bool  random);
    bool getPlaylistRandomOrder(const PlaylistHandle& h, bool* found=0) const;
    bool setPlaylistRandomSilence(const PlaylistHandle& h, bool  random);
    bool getPlaylistRandomSilence(const PlaylistHandle& h, bool* found=0) const;


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
