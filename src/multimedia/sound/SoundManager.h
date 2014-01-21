/*
 * SoundManager.h
 *
 *	This class handles the whole sound system playback
 *	by periodically updating all playing sources.
 *	That includes the GameUnits' SoundAPIs (e.g. zombies grunts, weapon shots)
 *	and the environmental sounds (e.g. background music, random sounds)
 *
 *	Available audio file names (aka "sounds") must be loaded by means of
 *	loadSound() interface. Environmental sounds are completely controlled
 *	here, from creation to destruction.
 *
 *	The SoundManager is a SINGLETON CLASS.
 *	The SoundManager is handled by the SoundHandler.
 *
 *  Created on: May 3, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_


#include <vector>
#include <deque>
#include <utility>	// std::pair
#include <unordered_map>

#include <OgrePrerequisites.h>  // Ogre::Real
#include <OgreVector3.h>        // Ogre::Vector3
#include <OgreString.h>	        // Ogre::String
#include <OgreCamera.h>         // Ogre::Camera

#include "SoundEnums.h"
#include "SoundSource.h"

#if defined(_WIN32) || defined(CYGWIN) || defined(MINGW)
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  include <AL/al.h>
#  include <AL/alc.h>
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif


// Forward declarations
namespace rrh {
class ResourceHandler;
}

namespace mm {

// Forward declarations
class SoundBuffer;
class LSoundSource;
class SSoundSource;
class SoundAPI;
class SoundHandler;
class OpenALHandler;


class SoundManager
{
    friend class SoundHandler;
	friend class SoundAPI;

	typedef int EnvSoundId;
	static const EnvSoundId INVALID_ENVSOUND_ID;

	// SoundSource wrapping
	struct ActiveSound
	{
		inline ActiveSound(SoundSource*  src = 0,
						   SSplayback pState = SSplayback::SS_FINISHED,
						   float gain = DEFAULT_ENV_GAIN, int index = -1);
		inline virtual ~ActiveSound();

		SoundSource* mSource;
		SSplayback	 mPlayState;	// Individual playback state
		SSplayback	 mGlobalState;	// Global playback state
		float		 mFadeTime;		// Fade out/in time
		float		 mFadeAccTime;	// Fade out/in elapsed time
		float		 mVolume;		// OpenAL source's gain old value (for fading)
		int			 mActivId;		// Activation index (position in mActiveSounds)
		/**
		 * @remarks
		 *
		 * The SoundAPI's "mActivationIndex" field is used to keep the position
		 * of the API's ActiveSound within the mUnitSounds vector.
		 *
		 * On the other hand, the ActiveSound's "mActivId" field keeps
		 * the position of the ActiveSound (either Unit or Environmental)
		 * within the SoundManager's mActiveSounds vector.
		 */
	};

public:
	/**
	 ** @brief
	 ** We use a singleton pattern for this class.
	 ** This is threadsafe on Linux/Windows if compiling with gcc/g++/cygwin.
	 **
	 ** @remarks
	 ** On first call, THE SoundManager is created, along with an OpenAL context
	 ** on the system's default sound device.
	 ** Listener position and velocity default to (0,0,0),
	 ** orientation to (0,0,-1,0,1,0). Camera defaults to NULL.
	 **
	 ** @remarks
	 ** getInstance() singleton pattern requires ctor, copytor and assign
	 ** methods to be declared private.
	 **
	 ** @return
	 ** A reference to the sole existent instance of SoundManager.
	 **/
	inline static SoundManager&
	getInstance();

private:
	/**
	 ** @brief
	 ** Prevent the compiler from generating methods to copy the instance.
	 **/
	SoundManager(SoundManager const&);    // Don't implement!
	void operator=(SoundManager const&);  // Don't implement!

	/**
	 ** @remarks
	 ** Creates sound context on default sound device, and all SoundSources.
	 **/
	SoundManager();

	/**
	 ** @remarks
	 ** Releases OpenAL sound resources (context, device)
	 **/
	~SoundManager();

	/**
	 ** @brief
	 ** Updates the sound playback volume, according to its fade in/out state.
	 **/
	void
	fadeUpdate(ActiveSound& s, const float globalTimeFrame);

	/**
	 ** @brief
	 ** Updates an already existent sound on which play() was called.
	 **/
	SSerror
	playExistentSound(ActiveSound& s, float gain, bool repeat);


	/*********************************************************************/
	/*******************XXX    INITIALIZATION    *************************/
protected:
    /**
     ** @brief
     ** Set the ResourceHandler to be used by this instance.
     **
     ** @param
     ** A pointer to the handler which will be used to load audio files.
     **
     ** @remarks
     ** The ResourceHandler is needed to load audio files into the sound system.
     **/
	void
	setResourceHandler(rrh::ResourceHandler* rh);

	/**
     ** @brief
     ** Set the OpenALHandler to be used by this instance.
     **
     ** @param
     ** A pointer to the handler which will be used to play sounds.
     **
     ** @remarks
     ** The OpenALHandler is needed to access the configured audio device,
     ** and play/pause/stop sounds in it.
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

public:
	/**
	 ** @brief
	 ** Lists available sound devices
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
	 ** @brief
	 ** View current listener 3D position.
	 **/
	Ogre::Vector3
	getPosition() const;

	/**
	 ** @brief
	 ** View current listener 3D orientation.
	 **
	 ** @remarks
	 ** Uses AT/UP vector format. For an explanation visit:
	 ** http://stackoverflow.com/questions/7861306/clarification-on-openal-listener-orientation
	 **/
	std::pair<Ogre::Vector3, Ogre::Vector3>
	getOrientation() const;

protected:
	/**
	 ** @brief
	 ** Adds "numSources" new Loaded Sound Sources to the SoundManager.
	 **
	 ** @remarks
	 ** Loaded Sound Sources play buffers entirely loaded into memory,
	 ** such as those obtained from small WAV or OGG files.
	 ** This is the default mechanism for playing audio files.
	 **
	 ** @return
	 ** SS_NO_ERROR			Sources successfully added.
     ** SS_NO_MEMORY        System ran out of memory. Go buy some, quick!
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	addLSoundSources(unsigned int numSources);

	/**
	 ** @brief
	 ** Adds "numSources" new Streaming Sound Sources to the SoundManager.
	 **
	 ** @remarks
	 ** Streaming Sound Sources use stream buffering for playback.
	 ** This mechanism requires a special buffer of type SS_BUF_STREAM.
	 **
	 ** @return
	 ** SS_NO_ERROR			Sources successfully added.
     ** SS_NO_MEMORY        System ran out of memory. Go buy some, quick!
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	addSSoundSources(unsigned int numSources);

	/**
	 ** @brief
	 ** Removes "numSources" SoundSources from the SoundManager.
	 **
	 ** @remarks
	 ** FIXME: CURRENTLY NOT IMPLEMENTED. DO NOT USE.
	 **/
	void
	removeSoundSources(unsigned int numSources);

	/**
	 ** @brief
	 ** Loads audio file "sName" for playback.
	 **
	 ** @param
	 **  sName: name of the audio file (realtive path, i.e., filename only)
	 ** format: file's audio compression format (WAV, OGG, MP3)
	 **   type: buffer's type (streaming vs mem.loaded)
	 **
	 ** @remarks
	 ** Searches for the audio file "sName" in filesystem, and tries to
	 ** load it as a new available sound buffer.
	 ** If file was already loaded as a buffer then nothing is done.
	 **
	 ** @remarks
	 ** If the compression format is not specified, file's name's extension
	 ** is used to determine it.
	 ** Default buffer type is SS_BUF_LOADED (i.e. load all audio data in mem)
	 **
	 ** @return
	 ** SS_NO_ERROR			Audio file "sName" successfully loaded as buffer.
	 ** SS_NO_BUFFER		Audio file "sName" had already been loaded.
	 ** SS_NO_MEMORY		System ran out of memory. Go buy some, quick!
	 ** SS_INVALID_FILE		Unsupported/erroneous file audio format
	 ** SS_FILE_NOT_FOUND	Audio file not found. Was the ResourceHandler set?
	 ** SS_INTERNAL_ERROR	Something went wrong. Does an OpenAL context exist?
	 **/
	SSerror
	loadSound(const Ogre::String& sName,
	          SSformat format = SSformat::SS_NOTHING,
	          SSbuftype type = SSbuftype::SS_BUF_LOADED);

	/**
	 ** @brief
	 ** Unloads the audio buffer named "sName" from (the SoundSystem) memory.
	 ** If no such buffer exists, nothing is done.
	 **
	 ** @remarks
	 ** Doesn't check if the sound is being used by some source.
	 ** May cause inconsistencies if user doesn't check that beforehand.
	 **/
	void
	unloadSound(const Ogre::String& sName);

public:
    /**
     ** @brief
     ** Tells whether sName is a loaded buffer in the sound system.
     **/
    bool
    isSoundLoaded(const Ogre::String& sName);

    /**
     ** @brief
     ** Tells whether id identifies a valid environmental sound index in the
     ** sound system.
     **/
    bool
    isValidIndex(EnvSoundId id);

private:
    /**
     ** @brief
     ** Destroy and uninitialize all the buffers and sources already loaded.
     ** This method will stop all the current active sounds and will destory
     ** everything else.
     ** Basically, is simulating the "destructor" since is a singleton class.
     ** TODO Fix when we change this class to stop being a singleton.
     **/
    void
    shutDown(void);


	/*********************************************************************/
	/*************XXX    GLOBAL PLAYBACK CONTROLS    *********************/
protected:
	/**
	 ** @brief
	 ** Updates all sounds currently active in the system.
	 **
	 ** @param
	 ** finished: if not NULL, will hold the IDs of the terminated EnvSounds
	 **   paused: if not NULL, will hold the IDs of the faded till paused EnvSounds
	 ** globalTimeFrame: time elapsed during the computations of the last frame
	 ** 				 (former "GLOBAL_TIME_FRAME")
	 **
	 ** @remarks
	 ** Updates must be done periodically, to allow streaming sources (such as
	 ** MP3 or large OGGs) to refresh their internal buffering mechanisms.
	 **/
	void
	update(const float globalTimeFrame,
			std::vector<EnvSoundId> *finished=0,
			std::vector<EnvSoundId> *paused=0);

	/**
	 ** @brief
	 ** Pauses ("globally") all currently playing sounds.
	 **/
	void
	globalPause();

	/**
	 ** @brief
	 ** Plays again all (globally) paused sounds.
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
	 ** That means that all active sources and buffers get released,
	 ** and the mActiveSounds vector is emptied (and so are the mEnvSounds
	 ** and mUnitSounds vectors as well).
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
	 **  time: fade-out time, in seconds. If negative, defaults to 1.0
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
	 ** time: fade-in time, in seconds. If negative, defaults to 1.0
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
	/***************XXX    ENVIRONMENTAL SOUNDS    ***********************/
public:
    /**
     ** @brief
     ** Is the environmental sound valid?
     **
     ** @return
     **  true: sID represents some valid environmental sound
     ** false: sID does not represent any valid sound in the system
     **
     ** @remarks
     ** For current implementation, "valid" environmental sounds are only those
     ** which are playing or paused, perhaps in some fading state.
     **/
    bool
    isValidEnvSound(const EnvSoundId& sID) const;

	/**
	 ** @brief
	 ** Is the environmental sound playing?
	 **
	 ** @return
	 **  true: sID (is valid and) is some playing environmental sound
	 ** false: sID is not playing |OR| is invalid
	 **/
	bool
	isPlayingEnvSound(const EnvSoundId& sID) const;

	/**
	 ** @brief
	 ** Is the environmental sound going to repeat on end?
	 **
	 ** @return
	 **  true: sID (is valid and) has the repeat option turned on
	 ** false: sID has the repeat option turned off |OR| is invalid
	 **/
	bool
	getEnvSoundRepeat(const EnvSoundId& sID) const;

protected:
	/**
	 ** @brief
	 ** Create a new environmental sound from audio file "sName", and play it
	 **
     ** @param
     **    sID: storage for the id of the new sound
     **  sName: name of the audio file to play
     **   gain: default volume of the sound, in [ 0.0 , 1.0 ] scale
     ** repeat: whether to repeat on end
     **
     ** @return
     ** SS_NO_ERROR         New sound started, "sID" is now a valid EnvSoundId
     ** SS_NO_MEMORY        System ran out of memory. Go buy some, quick!
     ** SS_NO_SOURCES       No available sources to play sound
     ** SS_FILE_NOT_FOUND   Sound "sName" not found (no buffer "sName" loaded)
     ** SS_INTERNAL_ERROR   Unspecified
     **
     ** @remarks
     ** Environmental sounds have no orientation nor distance fade.
     ** They are always attached to the listener, thus heard in first person.
     ** Sound "sName" should have already been loaded with loadSound()
     ** On success "sID" will identifying the new sound inside the system.
     ** This is the id which shall be passed for later sound manipulations.
	 **/
    SSerror
    startNewEnvSound(EnvSoundId& sID,
                     const Ogre::String& sName,
                     const Ogre::Real& gain = DEFAULT_ENV_GAIN,
                     bool repeat = false);

	/**
	 ** @brief
	 ** Play the (supposedly paused) environmental sound "sID"
	 **
	 ** @param
     ** sID: internal sound id, generated with startNewEnvSound()
	 **
	 ** @return
	 ** SS_NO_ERROR         Playback started
	 ** SS_NO_SOURCES       No available sources to play sound.
	 ** SS_INVALID_HANDLE   "sID" didn't match any valid environmental sound
	 ** SS_INTERNAL_ERROR   Unspecified
     **
     ** @remarks
     ** This function affects only paused environmental sounds,
     ** starting playback again at the correct point in time.
     ** Nothing will be done if the sound existed and was playing,
     ** or in some (any!) fading state.
	 **/
	SSerror
	playEnvSound(const EnvSoundId& sID);

	/**
	 ** @brief
     ** Pause the environmental sound "sID"
	 **
     ** @param
     ** sID: internal sound id, generated with startNewEnvSound()
     **
     ** @return
     ** SS_NO_ERROR         Playback paused
     ** SS_INVALID_HANDLE   "sID" didn't match any valid environmental sound
     ** SS_INTERNAL_ERROR   Unspecified
     **
	 ** @remarks
	 ** This function overrides (non-global) fadings.
	 **/
	SSerror
	pauseEnvSound(const EnvSoundId& sID);

	/**
	 ** @brief
     ** Stop the environmental sound "sID"
	 **
     ** @param
     ** sID: internal sound id, generated with startNewEnvSound()
     **
     ** @return
     ** SS_NO_ERROR         Playback stopped
     ** SS_INVALID_HANDLE   "sID" didn't match any valid environmental sound
     ** SS_INTERNAL_ERROR   Unspecified
     **
     ** @remarks
     ** This function DESTROYS the sound, rendering its id invalid.
     ** Upon successfull stopping, "sID" will no longer be valid in the system.
     ** To play this sound again call startNewEnvSound() with the same
     ** parameters used before.
	 **/
	SSerror
	stopEnvSound(EnvSoundId& sID);

	/**
	 ** @brief
     ** Restart the environmental sound "sID"
	 **
     ** @param
     ** sID: internal sound id, generated with startNewEnvSound()
     **
     ** @return
     ** SS_NO_ERROR         Playback restarted
     ** SS_INVALID_HANDLE   "sID" didn't match any valid environmental sound
     ** SS_INTERNAL_ERROR   Unspecified
     **
	 ** @remarks
	 ** Playback restarts from the beginning of the audio file.
	 ** This function overrides fadings, both individual and global.
	 **/
	SSerror
	restartEnvSound(const EnvSoundId& sID);

	/**
	 ** @brief
     ** Fade out the environmental sound "sID" playback volume
	 **
     ** @param
     **   sID: internal sound id, generated with startNewEnvSound()
     **  time: fade out time, in seconds. If negative defaults to 1.0
     ** pause: whether to pause the sound once muted
     **
     ** @return
     ** SS_NO_ERROR             Playback restarted
     ** SS_INVALID_HANDLE       "sID" didn't match any valid environmental sound
     ** SS_ILLEGAL_OPERATION    Unexpected play state, sound was left untouched
     ** SS_INTERNAL_ERROR       Unspecified
     **
	 ** @remarks
	 ** This funtion only affects sounds in a "plain" playing state.
	 ** So if the sound has been paused, or is under the effect of
	 ** another fade, nothing is done.
	 ** If pause==true, fadeInEnvSound() will restart playback when called.
	 **/
	SSerror
	fadeOutEnvSound(const EnvSoundId& sID,
					  const Ogre::Real& time = 1.0,
					  const bool pause = true);

	/**
	 ** @brief
     ** Fade back in the environmental sound "sID" playback volume
     ** to its original value.
	 **
     ** @param
     **  sID: internal sound id, generated with startNewEnvSound()
     ** time: fadein time, in seconds. If negative defaults to 1.0
     **
     ** @return
     ** SS_NO_ERROR             Playback restarted
     ** SS_INVALID_HANDLE       "sID" didn't match any valid environmental sound
     ** SS_ILLEGAL_OPERATION    Unexpected play state, sound was left untouched
     ** SS_INTERNAL_ERROR       Unspecified
     **
	 ** @remarks
	 ** This function only affects sounds which have been paused,
	 ** or are under the effect of an individual fade out.
	 ** So if the sound is just playing, or is under the effect of
	 ** another fade, nothing is done.
	 ** Playback is restarted if the sound has been faded out and/or paused.
	 **/
	SSerror
	fadeInEnvSound(const EnvSoundId& sID,
                   const Ogre::Real& time = 1.0);


	/*********************************************************************/
	/****************XXX    UNITS' APIS SOUNDS    ************************/
public:
	/**
	 ** @brief
	 ** Tells whether audio file "sName" is an active sound in some SoundAPI.
	 ** (i.e. playing or paused)
	 **/
	bool
	isActiveAPISound(const Ogre::String& sName) const;

protected:
	/**
	 ** @brief
	 ** Checks whether the SoundAPI is playing some sound.
	 **
	 ** @remarks
	 ** Returns true iff the sAPI is currently playing some sound.
	 **/
	bool
	findPlayingAPI(const SoundAPI& sAPI) const;

	/**
	 ** @brief
	 ** Checks whether the SoundAPI is active.
	 **
	 ** @remarks
	 ** Returns true both if the sAPI is currently playing some sound,
	 ** or if it's in a paused playback state.
	 **/
	bool
	findActiveAPI(const SoundAPI& sAPI) const;

	/**
	 ** @brief
	 ** Attaches the sound "sName" to sAPI, and starts playback.
	 **
	 ** @remarks
	 ** More specifically this function attaches an available source
	 ** to the specified SoundAPI, loads buffer named "sName" into the source,
	 ** registers the pair (sAPI,source) as a new active sound, and starts
	 ** the source playback.
	 ** If the SoundAPI was active (i.e. playing some sound, or in a paused
	 ** playback state) nothing is done.
	 **
	 ** @param
	 **   sAPI: reference to the SoundAPI the sound will be attached to.
	 **  sName: name of the audio file to play.
	 **   gain: volume of the sound, in [ 0.0 , 1.0 ] scale (default: 0.5)
	 ** repeat: whether to repeat on end (default: false)
	 **
	 ** @return
	 ** SS_NO_ERROR			Life's beautiful.
	 ** SS_NO_SOURCES		No available sources to play sound.
	 ** SS_FILE_NOT_FOUND	Sound "sName" not found (no buffer "sName" loaded).
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	playSound(SoundAPI& sAPI,
			  const Ogre::String& sName,
			  const Ogre::Real& gain = DEFAULT_UNIT_GAIN,
			  bool repeat = false);

	/**
	 ** @brief
	 ** Pauses the sAPI's currently playing sound.
	 ** If no sound had been started by sAPI, nothing is done.
	 **/
	void
	pauseSound (const SoundAPI& sAPI);

	/**
	 ** @brief
	 ** Stops the sAPI's currently playing sound.
	 **
	 ** @remarks
	 ** The SoundAPI is detached from the active sounds list.
	 ** The source and buffer associated to the SoundAPI get detached.
	 ** If no sound had been started by this API, nothing is done.
	 **
	 ** @return
	 ** SS_NO_ERROR			Sound successfully stopped, resources released.
	 ** SS_NO_BUFFER		sAPI wasn't playing anything
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	stopSound (SoundAPI& sAPI);

	/**
	 ** @brief
	 ** Restarts playback.
	 **
	 ** @remarks
	 ** If paused or playing, playback restarts from the beginning.
	 ** If stopped, or if no sound was being played by this SoundAPI,
	 ** nothing is done.
	 ** This function overrides fading, both individual and global.
	 **
	 ** @return
	 ** SS_NO_ERROR			     Sound playback successfully restarted.
	 ** SS_NO_BUFFER             sAPI wasn't playing anything.
	 ** SS_INTERNAL_ERROR	     Unspecified.
	 **/
	SSerror
	restartSound (SoundAPI& sAPI);

	/**
	 ** @brief
	 ** Fades out the sAPI's playback volume.
	 **
	 ** @remarks
	 ** This funtion only affects sounds in a "plain" playing state.
	 ** So if the sound has been paused, or is under the effect of
	 ** another fade, nothing is done.
	 ** If pause==true, fadeInEnvSound() will restart playback when called.
	 ** If no sound had been started by this API, nothing is done.
	 **
	 ** @param
	 **  sAPI: SoundAPI
	 **  time: fade-out time, in seconds. If negative, defaults to 1.0
	 ** pause: whether to pause the sound once muted. Default: true.
	 **
	 ** @return
	 ** SS_NO_ERROR			Sound fade-out started, or in an unexpected state
	 ** 					(and was left untouched)
	 ** SS_NO_BUFFER		sAPI wasn't playing anything
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	fadeOutSound(SoundAPI& sAPI, const Ogre::Real& time, const bool pause=true);

	/**
	 ** @brief
	 ** Fades back in the sAPI's playback volume to its original value.
	 **
	 ** @remarks
	 ** This function only affects sounds which have been paused,
	 ** or are under the effect of an individual fade out.
	 ** So if the sound is just playing, or is under the effect of
	 ** another fade, nothing is done.
	 ** Playback is restarted if the sound had been faded-out and/or paused.
	 ** If no sound had been started by this API, nothing is done.
	 **
	 ** @param
	 ** sAPI: SoundAPI
	 ** time: fade-in time, in seconds. If negative, defaults to 1.0
	 **
	 ** @return
	 ** SS_NO_ERROR			Sound fade-in started, or in an unexpected state
	 ** 					(and was left untouched)
	 ** SS_NO_BUFFER		sAPI wasn't playing anything
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	fadeInSound(SoundAPI& sAPI, const Ogre::Real& time);



	/*********************************************************************/
	/*******************XXX    CLASS MEMBERS    **************************/
private:

	typedef std::unordered_map<Ogre::String, SoundBuffer*> HashStrBuff;
	// Environmental Sounds
	typedef std::pair<EnvSoundId*, ActiveSound*> EnvSound;
	// Units' Sounds
	typedef std::pair<SoundAPI*, ActiveSound*> UnitSound;

	// Camera from which position and orientation are obtained for update().
	const Ogre::Camera* mCam;

	// Sounds loaded in memory at startup.
	// Buffers can be shared, and remain in memory between plays.
	HashStrBuff mLoadedBuffers;

	// LSS stands for "loaded sound sources" (class LSoundSource).
	// These are the free sources ready for direct playback.
	std::deque<LSoundSource*> mAvailableLSS;

	// SSS stands for "streaming sound sources" (class SSoundSource).
	// These are the free sources ready for streaming playback.
	std::deque<SSoundSource*> mAvailableSSS;

	// Currently active sounds in the system (i.e. playing or paused)
	std::vector<ActiveSound*> mActiveSounds;

	// Pointers to the active environmental sounds.
	std::vector<EnvSound> mEnvSounds;

	// Pointers to the active units sounds.
	std::vector<UnitSound> mUnitSounds;

    // Current resources handler
    rrh::ResourceHandler* mResourceHandler;

	// Current OpenAL handler
	OpenALHandler* mOpenALHandler;

	// EnvSoundId handling
	std::set<EnvSoundId> mActiveEnvSoundIds;
};


/******************************************************************************/
/****************************     INLINES     *********************************/


////////////////////////////////////////////////////////////////////////////////
inline
SoundManager::ActiveSound::ActiveSound(SoundSource* src,
                                       SSplayback pState,
                                       float gain,
                                       int index) :
	mSource(src),
	mPlayState(pState),
	mGlobalState(SSplayback::SS_NONE),
	mFadeTime(1.0f),
	mFadeAccTime(0.0f),
	mVolume(gain),
	mActivId(index)
{ /* Default ctor. suffices. */ }


////////////////////////////////////////////////////////////////////////////////
inline SoundManager::ActiveSound::~ActiveSound()
{ /* Default dtor. suffices. */ }


////////////////////////////////////////////////////////////////////////////////
inline SoundManager&
SoundManager::getInstance()
{
	static SoundManager instance;
	return instance;
}


////////////////////////////////////////////////////////////////////////////////
inline void SoundManager::setCamera(const Ogre::Camera* cam) { mCam = cam; }


////////////////////////////////////////////////////////////////////////////////
inline Ogre::Vector3
SoundManager::getPosition() const
{
	Ogre::Vector3 pos;
	alGetListener3f(AL_POSITION, &(pos.x), &(pos.y), &(pos.z));
	return pos;
}


////////////////////////////////////////////////////////////////////////////////
inline std::pair<Ogre::Vector3, Ogre::Vector3>
SoundManager::getOrientation() const
{
	float ori[6];
	alGetListenerfv(AL_ORIENTATION, ori);
	return std::make_pair(Ogre::Vector3(ori[0],ori[1],ori[2]),
						  Ogre::Vector3(ori[3],ori[4],ori[5]));
}


////////////////////////////////////////////////////////////////////////////////
inline bool
SoundManager::isSoundLoaded(const Ogre::String& sName)
{
	return (mLoadedBuffers.find(sName) != mLoadedBuffers.end());
}


}

#endif /* SOUNDMANAGER_H_ */
