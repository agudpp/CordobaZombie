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

#include <cstdint>  // uint64_t
#include <vector>
#include <tuple>
#include <deque>
#include <utility>	// std::pair
#include <unordered_map>

#include <OgrePrerequisites.h>  // Ogre::Real
#include <OgreVector3.h>        // Ogre::Vector3
#include <OgreString.h>	        // Ogre::String
#include <OgreCamera.h>         // Ogre::Camera

#include "SoundEnums.h"
#include "SoundSource.h"

#if defined(_WIN32) || defined(CYGWIN)
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
class OpenALHandler;


class SoundManager
{
	friend class SoundAPI;
	typedef void* EnvSoundId;

	/* SoundSource wrapping useful for SoundManager manipulations. */
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
	/**********************    INITIALIZATION    *************************/
public:

	/**
     ** @brief
     ** Set the OpenALHandler to be used by this instance
     **
     ** @param
     ** The handler pointer to be used. Note that we need this handler over all
     ** the life time of this instance, so you cannot destory it before this
     ** class.
     **/
    void
    setOpenALHandler(OpenALHandler* handler);

	/**
	 ** @brief
	 ** Lists available sound devices
	 **/
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
	 ** @brief Tells whether OpenAL system is set up correctly
	 **/
	inline bool
	hasOpenALcontext(void);

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

	/**
	 ** @brief
	 ** View current listener 3D position.
	 **/
	inline Ogre::Vector3
	getPosition();

	/**
	 ** @brief
	 ** View current listener 3D orientation.
	 **
	 ** @remarks
	 ** Uses AT/UP vector format. For an explanation visit:
	 ** http://stackoverflow.com/questions/7861306/clarification-on-openal-listener-orientation
	 **/
	inline std::pair<Ogre::Vector3, Ogre::Vector3>
	getOrientation();

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
	 ** SS_NO_MEMORY		Insufficient memory for operation, nothing was done.
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
	 ** SS_NO_MEMORY		Insufficient memory for operation, nothing was done.
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
	 **     rh: reference to the resource handler with access to the file
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
	 ** SS_NO_MEMORY		Not enough memory to work with. Go buy some.
	 ** SS_INVALID_FILE		Unsupported/erroneous file audio format
	 ** SS_FILE_NOT_FOUND	Audio file not found. Is it listed as resource?
	 ** SS_INTERNAL_ERROR	Something went wrong. Does an OpenAL context exist?
	 **/
	SSerror
	loadSound(rrh::ResourceHandler& rh,
	          const Ogre::String& sName,
	          SSformat format = SSformat::SS_NOTHING,
	          SSbuftype type = SSbuftype::SS_BUF_LOADED);

	/**
	 ** @brief
	 ** Tells whether sName is a loaded buffer in the system.
	 **/
	bool
	isSoundLoaded(const Ogre::String& sName);

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


    /**
     ** @brief
     ** Destroy and uninitialize all the buffers and sources already loaded. This
     ** method will stop all the current active sounds and will destory everything
     ** else. Basically, is simulating the "destructor" since is a singleton
     ** class. This should be fixed when we change this class to not be anymore
     ** a singleton.
     **
     **/
    void
    destroyAll(void);

	/*********************************************************************/
	/****************    GLOBAL PLAYBACK CONTROLS    *********************/
public:
	/** XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX **
	 * XXX   All these global methods should be accessed     XXX *
	 * XXX   via the stubs implemented in the SoundHandler.  XXX */

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
	/******************    ENVIRONMENTAL SOUNDS    ***********************/
public:
	/**
	 ** @brief
	 ** Searchs an environmental sound by given sName,
	 ** and tells whether it is currently playing.
	 **
	 ** @return
	 **  true: environmental sound "sName" is playing.
	 ** false: environmental sound "sName" is not playing |OR| it wasn't found
	 **/
	bool
	isPlayingEnvSound(const Ogre::String& sName);

	/**
	 ** @brief
	 ** Searchs an environmental sound by given id,
	 ** and tells whether it is currently playing.
	 **
	 ** @return
	 **  true: environmental sound was found and is playing.
	 ** false: environmental sound is not playing |OR| it wasn't found.
	 **/
	bool
	isPlayingEnvSound(EnvSoundId id=0);

	/**
	 ** @brief
	 ** Searchs an environmental sound by given sName,
	 ** and tells whether it is currently active (viz. playing or paused)
	 **
	 ** @return
	 **  true: environmental sound "sName" is active.
	 ** false: environmental sound "sName" is not active |OR| it wasn't found
	 **/
	bool
	isActiveEnvSound(const Ogre::String& sName);

	/**
	 ** @brief
	 ** Searchs an environmental sound by given id,
	 ** and tells whether it is currently active (viz. playing or paused)
	 **
	 ** @return
	 **  true: environmental sound was found and is active.
	 ** false: environmental sound is not active |OR| it wasn't found.
	 **/
	bool
	isActiveEnvSound(EnvSoundId id=0);

	/**
	 ** @brief
	 ** Tells whether environmental sound "sName" was started with repeat.
	 **
	 ** @return
	 **  true: "sName" is an active environmental sound with repeat option.
	 ** false: "sName" is an active environmental sound with NO repeat option,
	 **        or "sName" is NOT an active environmental sound
	 **/
	bool
	getEnvSoundRepeat(const Ogre::String& sName);

	/**
	 ** @brief
	 ** Plays audio file "sName" as an environmental sound.
	 ** i.e. no orientation, no distance fade.
	 **
	 ** @remarks
	 ** Sound "sName" should have already been loaded with loadSound()
	 ** At most one environmental sound "sName" can be active at any time.
	 ** That is, if the file "sName" is already playing as an env. sound,
	 ** calls to playEnvSound(sName) will return sucessfully but will NOT
	 ** start any new playback.
	 ** This function affects ONLY PAUSED OR NEW SOUNDS.
	 ** Nothing will be done if "sName" was playing or in a fading state.
	 **
	 ** @param
	 **  sName: name of the audio file to play
	 **   gain: volume of the sound, in [ 0.0 , 1.0 ] scale (default: 0.07)
	 ** repeat: whether to repeat on end (default: false)
	 **     id: if not NULL, give an ID to this sound for tracking
	 **
	 ** @return
	 ** SS_NO_ERROR			Playback started
	 ** SS_NO_SOURCES		No available sources to play sound.
	 ** SS_FILE_NOT_FOUND	Sound "sName" not found (no buffer "sName" loaded).
	 ** SS_INTERNAL_ERROR	Unspecified
	 **/
	SSerror
	playEnvSound(const Ogre::String& sName,
				 const Ogre::Real& gain = DEFAULT_ENV_GAIN,
				 bool repeat = false,
				 EnvSoundId id = 0);

	/**
	 ** @brief
	 ** Pauses environmental sound "sName"
	 **
	 ** @param
	 ** sName: name of the audio file to pause
	 **    id: if not NULL, only EnvSound with matching id will be paused
	 **
	 ** @remarks
	 ** If no environmental sound with such name/id exists, nothing is done.
	 ** This function overrides fadings.
	 **/
	void
	pauseEnvSound(const Ogre::String& sName, EnvSoundId id=0);

	/**
	 ** @brief
	 ** Stops environmental sound "sName"
	 **
	 ** @param
	 ** sName: name of the audio file to stop
	 **    id: if not NULL, only EnvSound with matching id will be stopped
	 **
	 ** @remarks
	 ** The sound is detached from the active sounds list.
	 ** The associated source and buffer are released.
	 ** If no environmental sound "sName" exists, nothing is done.
	 **
	 ** @return
	 ** SS_NO_ERROR			Sound successfully stopped, resources released.
	 ** SS_NO_BUFFER		"sName" didn't match any existent env. sound
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	stopEnvSound(const Ogre::String& sName, EnvSoundId id=0);

	/**
	 ** @brief
	 ** Restarts environmental sound "sName"
	 **
	 **
	 ** @param
	 ** sName: name of the audio file to restart
	 **    id: if not NULL, only EnvSound with matching id will be restarted
	 **
	 ** @remarks
	 ** If paused or playing, playback restarts from the beginning.
	 ** If stopped, or if no environmental sound by the name "sName"
	 ** had been created, nothing is done.
	 ** This function overrides fadings, both individual and global.
	 **
	 ** @return
	 ** SS_NO_ERROR			     Sound playback successfully restarted.
	 ** SS_NO_BUFFER             "sName" didn't match any existent env. sound
	 ** SS_INTERNAL_ERROR	     Unspecified.
	 **/
	SSerror
	restartEnvSound(const Ogre::String& sName, EnvSoundId id=0);

	/**
	 ** @brief
	 ** Fades out environmental sound "sName" playback volume.
	 **
	 ** @remarks
	 ** This funtion only affects sounds in a "plain" playing state.
	 ** So if the sound has been paused, or is under the effect of
	 ** another fade, nothing is done.
	 ** If pause==true, fadeInEnvSound() will restart playback when called.
	 ** If no environmental sound with such name exists, nothing is done.
	 **
	 ** @param
	 ** sName: name of the environmental sound
	 **  time: fade-out time, in seconds. If negative, defaults to 1.0
	 ** pause: whether to pause the sound once muted. Default: true.
	 **    id: if not NULL, only EnvSound with matching id will be faded-out
	 **
	 ** @return
	 ** SS_NO_ERROR			Sound fade-out started, or in an unexpected state
	 ** 					(and was left untouched)
	 ** SS_NO_BUFFER		"sName" didn't match any existent env. sound
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	fadeOutEnvSound(const Ogre::String& sName,
					  const Ogre::Real& time,
					  const bool pause=true,
					  EnvSoundId id=0);

	/**
	 ** @brief
	 ** Fades back in the environmental sound "sName" playback volume
	 ** to its original value.
	 **
	 ** @remarks
	 ** This function only affects sounds which have been paused,
	 ** or are under the effect of an individual fade out.
	 ** So if the sound is just playing, or is under the effect of
	 ** another fade, nothing is done.
	 ** Playback is restarted if the sound had been faded-out and/or paused.
	 ** If no environmental sound named "sName" exists, nothing is done.
	 **
	 ** @param
	 ** sName: name of the environmental sound
	 **  time: fade-in time, in seconds. If negative, defaults to 1.0
	 **    id: if not NULL, only EnvSound with matching id will be faded-out
	 **
	 ** @return
	 ** SS_NO_ERROR			Sound fade-in started, or in an unexpected state
	 ** 					(and was left untouched)
	 ** SS_NO_BUFFER		"sName" didn't match any existent env. sound
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	fadeInEnvSound(const Ogre::String& sName,
					 const Ogre::Real& time,
					 EnvSoundId id=0);



	/*********************************************************************/
	/*******************    UNITS' APIS SOUNDS    ************************/
public:
	/**
	 ** @brief
	 ** Tells whether audio file "sName" is an active sound in some SoundAPI.
	 ** (i.e. playing or paused)
	 **/
	bool
	isActiveAPISound(const Ogre::String& sName) const;

private:
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
	/**********************    CLASS MEMBERS    **************************/
private:

	typedef std::unordered_map<Ogre::String, SoundBuffer*> HashStrBuff;
	// Environmental Sounds
	typedef std::tuple<Ogre::String, ActiveSound*, EnvSoundId> EnvSound;
	// Units' Sounds
	typedef std::pair<SoundAPI*, ActiveSound*> UnitSound;

	// Current OpenAL context
	ALCcontext* mOpenALcontext;

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

	// The OpenALHandler to be used
	OpenALHandler* mOpenALHandler;
};


/******************************************************************************/
/****************************     INLINES     *********************************/


////////////////////////////////////////////////////////////////////////////////
inline
SoundManager::ActiveSound::ActiveSound(SoundSource* src, SSplayback pState,
										float gain, int index) :
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
{
	/* Default dtor. suffices. */
}


////////////////////////////////////////////////////////////////////////////////
inline SoundManager&
SoundManager::getInstance()
{
	static SoundManager instance;
	return instance;
}

////////////////////////////////////////////////////////////////////////////////
inline bool SoundManager::hasOpenALcontext() { return mOpenALcontext!=0; }

////////////////////////////////////////////////////////////////////////////////
inline void SoundManager::setCamera(const Ogre::Camera* cam) { mCam = cam; }


////////////////////////////////////////////////////////////////////////////////
inline Ogre::Vector3
SoundManager::getPosition()
{
	Ogre::Vector3 pos;
	alGetListener3f(AL_POSITION, &(pos.x), &(pos.y), &(pos.z));
	return pos;
}


////////////////////////////////////////////////////////////////////////////////
inline std::pair<Ogre::Vector3, Ogre::Vector3>
SoundManager::getOrientation()
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


////////////////////////////////////////////////////////////////////////////////
inline void
SoundManager::pauseEnvSound(const Ogre::String& sName, EnvSoundId id)
{
	for (int i=0 ; i < mEnvSounds.size() ; i++) {
		if (std::get<0>(mEnvSounds[i]) == sName &&
			std::get<1>(mEnvSounds[i])->mGlobalState != SSplayback::SS_PAUSED) {
			if (id && id != std::get<2>(mEnvSounds[i]))
				continue;  // Not our sound.
			std::get<1>(mEnvSounds[i])->mSource->pause();
			std::get<1>(mEnvSounds[i])->mPlayState = SSplayback::SS_PAUSED;
			std::get<1>(mEnvSounds[i])->mGlobalState = SSplayback::SS_NONE;
		}
	}
}

}

#endif /* SOUNDMANAGER_H_ */
