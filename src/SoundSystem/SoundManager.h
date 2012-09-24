/*
 * SoundManager.h
 *
 *	This class connects the SoundAPIs from the Units
 *	with the internal SoundSources of the sound system.
 *
 *	It handles the sound system playback by periodically updating all sources.
 *	It's a SINGLETON CLASS.
 *
 *  Created on: May 3, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_

#include <cstdint>  // uint64_t
#include <vector>
#include <deque>
#include <Ogre.h>
#include "MultiplatformTypedefs.h"
#include "SoundEnums.h"
#include "SoundSource.h"
#include "SoundAPI.h"

#if defined(_WIN32) || defined(CYGWIN)
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  include <AL/al.h>
#  include <AL/alc.h>
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif

/* FWD declaration, to avoid circular dependencies. */
class SoundBuffer;
class LSoundSource;
class SSoundSource;

typedef _HashTable<Ogre::String, SoundBuffer*>::HashTable HashStrBuff;


class SoundManager
{
	friend class SoundAPI;

	typedef enum {
		FADING_OUT_AND_PAUSE = 0,
		FADING_OUT	= 1,
		FADING_IN	= 2,
		FADING_NONE	= 4
	} fadeState;


	/**
	 ** TODO: Refactorizar las estructuras internas EnvSound y ActiveSound
	 **
	 ** Crear una clase "ActiveSound" en el header de SoundSource,
	 ** que herede de SoundSource y unifique las actuales EnvSound y ActiveSound
	 **
	 ** Prototipo:
	 ** class ActiveSound : public SoundSource
	 ** {
	 ** public:
	 **		SSplayback	 globalState;	// Global playback state
	 **		fadeState	 fadeStatus;	// Individual fading out/in state
	 **		float		 fadeTime;		// Fade out/in time
	 **		float		 fadeAccTime;	// Fade out/in elapsed time
	 **		float		 volume;		// OpenAL source's gain value (for fading)
	 ** };
	 **
	 ** Las actuales estructuras internas EnvSound y ActiveSound
	 ** del SoundManager se transformarían, resp., en lo siguiente:
	 **
	 **	typedef std::pair<Ogre::String, ActiveSound> EnvSound;
	 **	typedef std::pair<SoundAPI*,    ActiveSound> ApiSound;
	 **
	 **	Y podríamos mantener tres vectores en el SoundManager:
	 **		¤ uno con todos los sonidos activos (EnvSounds y ApiSounds mezclados
	 **		  para evitar repetición de código en las funciones globales)
	 **		¤ otro con punteros a los EnvSounds del vector de sonidos activos
	 **		¤ otro con punteros a los ApiSounds del vector de sonidos activos
	 **	Preguntarle al Agus si esto es un diseño eficiente.
	 **/

	struct EnvSound {
		/* Environmental sound (sound name + source) */
		Ogre::String name;
		SoundSource* source;
		SSplayback	 globalState;	// Global playback state
		fadeState	 fadeStatus;	// Individual fading out/in state
		float		 fadeTime;		// Fade out/in time
		float		 fadeAccTime;	// Fade out/in elapsed time
		float		 volume;		// OpenAL source's gain value (for fading)
		EnvSound(const Ogre::String& sName,
				 SoundSource* src  = 0,
				 SSplayback gState = SSplayback::SS_FINISHED) :
					 name(sName),
					 source(src),
					 globalState(gState),
					 fadeStatus(FADING_NONE),
					 fadeTime(1.0f),
					 fadeAccTime(0.0f),
					 volume(0.0f)
		{ /* Default ctor. suffices. */ }
	};

	struct ActiveSound {
		/* Game unit sound (sound api + source) */
		SoundAPI*    sAPI;
		SoundSource* sSRC;
		SSplayback	 globalState;	// Global playback state
		fadeState	 fadeStatus;	// Individual fading out/in state
		float		 fadeTime;		// Fade out/in time
		float		 fadeAccTime;	// Fade out/in elapsed time
		float		 volume;		// OpenAL source's gain value (for fading)
		ActiveSound(SoundAPI*     api = NULL,
					SoundSource*  src = NULL,
					SSplayback gState = SSplayback::SS_FINISHED) :
						sAPI(api),
						sSRC(src),
						globalState(gState),
						fadeStatus(FADING_NONE),
						fadeTime(1.0f),
						fadeAccTime(0.0f),
						volume(0.0f)
		{ /* Default ctor. suffices. */ }
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
	 ** @returns
	 ** A reference to the sole existent instance of SoundManager.
	 **/
	inline static SoundManager&
	getInstance();

private:
	/* Prevent the compiler from generating methods to copy the instance: */
	SoundManager(SoundManager const&);    // Don't implement!
	void operator=(SoundManager const&);  // Don't implement!

	/* @remark
	 * ctor creates sound context on default sound device,
	 * and all SoundSources. */
	SoundManager();

	/* @remark
	 * dtor releases OpenAL sound resources (context, device) */
	~SoundManager();

	/* Updates the sound playback volume, according to its fade in/out state. */
	void fadeUpdate(SoundManager::EnvSound& s);

	/* Updates the sound playback volume, according to its fade in/out state. */
	void fadeUpdate(SoundManager::ActiveSound& s);


	/*********************************************************************/
	/**********************    INITIALIZATION    *************************/
public:
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
	 ** @returns
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
	setCamera(Ogre::Camera* cam);

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
	 ** @remark
	 ** Loaded Sound Sources play buffers entirely loaded into memory,
	 ** such as those obtained from small WAV or OGG files.
	 ** This is the default mechanism for playing audio files.
	 **
	 ** @return
	 ** SS_NO_ERROR			Sources successfully added.
	 ** SS_NO_MEMORY		Insufficient memory for operation, nothing was done.
	 ** SS_INTERNAL_ERROR	Unespecified.
	 **/
	SSerror
	addLSoundSources(unsigned int numSources);

	/**
	 ** @brief
	 ** Adds "numSources" new Streaming Sound Sources to the SoundManager.
	 **
	 ** @remark
	 ** Streaming Sound Sources use stream buffering for playback.
	 ** This mechanism requires a special buffer of type SS_BUF_STREAM.
	 **
	 ** @return
	 ** SS_NO_ERROR			Sources successfully added.
	 ** SS_NO_MEMORY		Insufficient memory for operation, nothing was done.
	 ** SS_INTERNAL_ERROR	Unespecified.
	 **/
	SSerror
	addSSoundSources(unsigned int numSources);

	/**
	 ** @brief
	 ** Removes "numSources" SoundSources from the SoundManager.
	 **
	 ** @remark
	 ** FIXME: CURRENTLY NOT IMPLEMENTED. DO NOT USE.
	 **/
	void
	removeSoundSources(unsigned int numSources);

	/**
	 ** @brief
	 ** Loads audio file "sName" for playback.
	 **
	 ** @params
	 **  sName: name of the audio file (realtive path, i.e., filename only)
	 ** format: file's audio compression format (WAV, OGG, MP3)
	 **   type: buffer's type (streaming vs mem.loaded)
	 **
	 ** @remarks
	 ** Searches for the audio file "sName" in filesystem, and tries to
	 ** load it as a new available sound buffer.
	 ** If the compression format is not specified, file's name's extension
	 ** is used to determine it.
	 ** Default buffer type is SS_BUF_LOADED (i.e. load all audio data in mem)
	 **
	 ** @return
	 ** SS_NO_ERROR			Audio file "sName" successfully loaded as buffer.
	 ** SS_NO_MEMORY		Not enough memory to work with. Go buy some.
	 ** SS_INVALID_FILE		Unsupported/erroneous file audio format
	 ** SS_FILE_NOT_FOUND	Inexistent audio file.
	 ** SS_INTERNAL_ERROR	Something went wrong. Does an OpenAL context exist?
	 **/
	SSerror
	loadSound(const Ogre::String& sName, SSformat format=SSformat::SS_NONE,
				SSbuftype type=SSbuftype::SS_BUF_LOADED);

	/**
	 ** @brief
	 ** Unloads the audio buffer named "sName" from memory.
	 ** If no such buffer exists, nothing is done.
	 **/
	void
	unloadSound(const Ogre::String& sName);



	/*********************************************************************/
	/****************    GLOBAL PLAYBACK CONTROLS    *********************/
public:
	/**
	 ** @brief
	 ** Updates all sounds currently active in the system.
	 **
	 ** @remark
	 ** Updates must be done periodically, to allow streaming sources (such as
	 ** MP3 or large OGGs) to refresh their internal buffering mechanisms.
	 **/
	void
	update();

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
	 ** Sounds (both environmental and attached to a unit's SoundAPI)
	 ** which were paused manually, i.e. before calling globalPause(),
	 ** will NOT start playing again with this function.
	 **/
	void
	globalPlay();

	/**
	 ** @brief
	 ** Stops all sounds. This frees all resources.
	 **/
	void
	globalStop();

	/**
	 ** @brief
	 ** Restarts all active sounds in the system.
	 **/
	void
	globalRestart();

	/**
	 ** @brief
	 ** Fades out the playback volume of all playing sounds in the system.
	 **
	 ** @params
	 **  time: fade-out time, in seconds. If negative, defaults to 1.0
	 ** pause: whether to pause the sounds once muted. Default: true.
	 **
	 ** @remark
	 ** If pause==true, globalFadeIn() will restart playbacks when called.
	 **/
	void
	globalFadeOut(const Ogre::Real& time, const bool pause=true);

	/**
	 ** @brief
	 ** Fades back in all (globally) faded-out sounds.
	 **
	 ** @params
	 ** time: fade-in time, in seconds. If negative, defaults to 1.0
	 **
	 ** @remark
	 ** Sounds (both environmental and attached to a unit's SoundAPI)
	 ** which were faded-out manually, i.e. before calling globalFadeOut(),
	 ** will NOT be modified by this function.
	 **
	 ** @remark
	 ** Playback is restarted if the sounds had been faded-out and paused.
	 **/
	void
	globalFadeIn(const Ogre::Real& time);



	/*********************************************************************/
	/******************    ENVIRONMENTAL SOUNDS    ***********************/
public:
	/**
	 ** @brief
	 ** Tells whether audio file "sName" is playing as an environmental sound.
	 **/
	bool
	isPlayingEnvSound(const Ogre::String& sName);

	/**
	 ** @brief
	 ** Tells whether audio file "sName" is an active environmental sound.
	 ** (i.e. playing or paused)
	 **/
	bool
	isActiveEnvSound(const Ogre::String& sName);

	/**
	 ** @brief
	 ** Tells whether environmental sound "sName" was started with repeat.
	 **
	 ** @returns
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
	 ** At most, 1 "sName" environmental sound can be active at any time.
	 ** That is: if "sName" is already playing as an environmental sound,
	 ** calls to playEnvSound(sName) will return sucessfully but will NOT
	 ** start any new playback.
	 **
	 ** @returns
	 ** SS_NO_ERROR			Playback started
	 ** SS_NO_SOURCES		No available sources to play sound.
	 ** SS_FILE_NOT_FOUND	Sound "sName" not found (no buffer "sName" loaded).
	 ** SS_INTERNAL_ERROR	Unspecified
	 **/
	SSerror
	playEnvSound(const Ogre::String& sName, bool repeat=false);

	/**
	 ** @brief
	 ** Pauses environmental sound "sName"
	 ** If no environmental sound with such name exists, nothing is done.
	 **/
	void
	pauseEnvSound(const Ogre::String& sName);

	/**
	 ** @brief
	 ** Stops environmental sound "sName"
	 **
	 ** @remark
	 ** The sound is detached from the active sounds list.
	 ** The associated source and buffer are released.
	 ** If no environmental sound "sName" exists, nothing is done.
	 **/
	void
	stopEnvSound(const Ogre::String& sName);

	/**
	 ** @brief
	 ** Restarts environmental sound "sName"
	 **
	 ** @remark
	 ** If paused or playing, playback restarts from the beginning.
	 ** If stopped, or if no environmental sound by the name "sName"
	 ** had been created, nothing is done.
	 **
	 ** @return
	 ** SS_NO_ERROR			     Sound playback successfully restarted.
	 ** SS_NO_BUFFER             "sName" didn't match any existent env. sound
	 ** SS_INTERNAL_ERROR	     Unspecified.
	 **/
	SSerror
	restartEnvSound(const Ogre::String& sName);

	/**
	 ** @brief
	 ** Fades out environmental sound "sName" playback volume.
	 **
	 ** @params
	 ** sName: name of the environmental sound
	 **  time: fade-out time, in seconds. If negative, defaults to 1.0
	 ** pause: whether to pause the sound once muted. Default: true.
	 **
	 ** @remarks
	 ** If pause==true, fadeInEnvSound() will restart playback when called.
	 ** If no environmental sound with such name exists, nothing is done.
	 **/
	void
	fadeOutEnvSound(const Ogre::String& sName,
					const Ogre::Real& time,
					const bool pause=true);

	/**
	 ** @brief
	 ** Fades back in the environmental sound "sName" playback volume
	 ** to its original value.
	 **
	 ** @params
	 ** sName: name of the environmental sound
	 **  time: fade-in time, in seconds. If negative, defaults to 1.0
	 **
	 ** @remarks
	 ** If "sName" hadn't been faded-out, nothing is done.
	 ** If no environmental sound with such name exists, nothing is done.
	 ** Playback is restarted if the sound had been faded-out and paused.
	 **/
	void
	fadeInEnvSound(const Ogre::String& sName, const Ogre::Real& time);



	/*********************************************************************/
	/*******************    UNITS' APIS SOUNDS    ************************/
private:
	/**
	 ** @brief
	 ** Checks whether the SoundAPI is playing some sound.
	 **
	 ** @remarks
	 ** Returns true iff the sAPI is currently playing some sound.
	 **/
	inline bool
	findPlayingAPI (const SoundAPI& sAPI) const;

	/**
	 ** @brief
	 ** Checks whether the SoundAPI is active.
	 **
	 ** @remarks
	 ** Returns true both if the sAPI is currently playing some sound,
	 ** or if it's in a paused playback state.
	 **/
	inline bool
	findActiveAPI (const SoundAPI& sAPI) const;

	/**
	 ** @brief
	 ** Attaches the sound "sName" to sAPI, and starts playback.
	 **
	 ** @remark
	 ** More specifically this function attaches an available source
	 ** to the specified SoundAPI, loads buffer named "sName" into the source,
	 ** registers the pair (sAPI,source) as a new active sound, and starts
	 ** the source playback.
	 ** If the SoundAPI was active (i.e. playing some sound, or in a paused
	 ** playback state) nothing is done.
	 **
	 ** @returns
	 ** SS_NO_ERROR			Life's beautiful.
	 ** SS_NO_SOURCES		No available sources to play sound.
	 ** SS_FILE_NOT_FOUND	Sound "sName" not found (no buffer "sName" loaded).
	 ** SS_INTERNAL_ERROR	Unspecified.
	 **/
	SSerror
	playSound (SoundAPI& sAPI, const Ogre::String& sName, bool repeat=false);

	/**
	 ** @brief
	 ** Pauses the sAPI's currently playing sound.
	 ** If no sound had been started by sAPI, nothing is done.
	 **/
	inline void
	pauseSound (const SoundAPI& sAPI);

	/**
	 ** @brief
	 ** Stops the sAPI's currently playing sound.
	 **
	 ** @remark
	 ** The SoundAPI is detached from the active sounds list.
	 ** The source and buffer associated to the SoundAPI get detached.
	 ** If no sound had been started by this API, nothing is done.
	 **/
	void
	stopSound (SoundAPI& sAPI);

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
	 ** @params
	 **  sAPI: SoundAPI
	 **  time: fade-out time, in seconds. If negative, defaults to 1.0
	 ** pause: whether to pause the sound once muted. Default: true.
	 **
	 ** @remark
	 ** If stopped, or if no sound was being played by this SoundAPI,
	 ** nothing is done.
	 **
	 ** @remarks
	 ** If no sound was being played by this SoundAPI, nothing is done.
	 ** If pause==true, fadeInSound() will restart playback when called.
	 **/
	void
	fadeOutSound(SoundAPI& sAPI, const Ogre::Real& time, const bool pause=true);

	/**
	 ** @brief
	 ** Fades back in the sAPI's playback volume to its original value.
	 **
	 ** @params
	 ** sAPI: SoundAPI
	 ** time: fade-in time, in seconds. If negative, defaults to 1.0
	 **
	 ** @remarks
	 ** If "sAPI" hadn't been faded-out, nothing is done.
	 ** If no sound was being played by this SoundAPI, nothing is done.
	 ** Playback is restarted if the sound had been faded-out and paused.
	 **/
	void
	fadeInSound(SoundAPI& sAPI, const Ogre::Real& time);



	/*********************************************************************/
	/*********************    CLASS' MEMBERS    **************************/
private:
	/* Camera from which position and orientation
	 * are obtained for update() method. */
	Ogre::Camera* mCam;

	/* Sounds loaded in memory at startup.
	 * Buffers can be shared, and remain in memory between plays. */
	HashStrBuff mLoadedBuffers;

	/* LSS stands for "loaded sound sources" (class LSoundSource).
	 * These are the free sources ready for direct playback. */
	std::deque<LSoundSource*> mAvailableLSS;

	/* SSS stands for "streaming sound sources" (class SSoundSource).
	 * These are the free sources ready for streaming playback. */
	std::deque<SSoundSource*> mAvailableSSS;

	/* Currently playing environmental sounds. */
	std::vector<EnvSound> mEnvSounds;

	/* Currently playing unit sounds.
	 * i.e. a series of playing SoundSources, each with its owner sAPI. */
	std::vector<ActiveSound> mActiveSounds;
};


/******************************************************************************/
/****************************     INLINES     *********************************/


////////////////////////////////////////////////////////////////////////////////
SoundManager&
SoundManager::getInstance()
{
	static SoundManager instance;
	return instance;
}


////////////////////////////////////////////////////////////////////////////////
inline void SoundManager::setCamera(Ogre::Camera* cam) { mCam = cam; }


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
inline void
SoundManager::pauseEnvSound(const Ogre::String& sName)
{
	for (int i=mEnvSounds.size()-1 ; i>=0 ; i--) {
		if (mEnvSounds[i].name == sName) {
			mEnvSounds[i].source->pause();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
inline bool
SoundManager::findPlayingAPI (const SoundAPI& sAPI) const
{
	if (sAPI.mActivationIndex < 0
		|| mActiveSounds.size() <= sAPI.mActivationIndex
		|| mActiveSounds[sAPI.mActivationIndex].sAPI != &sAPI) {
		return false;
	} else {
		ALint st;
		alGetSourcei(mActiveSounds[sAPI.mActivationIndex].sSRC->mSource,
					 AL_SOURCE_STATE, &st);
		return st == AL_PLAYING;
	}
}


////////////////////////////////////////////////////////////////////////////////
inline bool
SoundManager::findActiveAPI (const SoundAPI& sAPI) const
{
	return (sAPI.mActivationIndex >= 0
			&& mActiveSounds.size() > sAPI.mActivationIndex
			&& mActiveSounds[sAPI.mActivationIndex].sAPI == &sAPI);
}


////////////////////////////////////////////////////////////////////////////////
inline void
SoundManager::pauseSound(const SoundAPI& sAPI)
{
	if (findActiveAPI(sAPI)) {
		mActiveSounds[sAPI.mActivationIndex].sSRC->pause();
	}
}



#endif /* SOUNDMANAGER_H_ */
