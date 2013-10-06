/*
 * SoundManager.cpp
 *
 *	This class handles the whole sound system playback
 *	by periodically updating all playing sources.
 *	That includes the GameUnits' SoundAPIs (v.gr. zombies grunts, weapon shots)
 *	and the environmental sounds (v.gr. background music)
 *
 *  Available audio file names (aka "sounds") must be loaded by means of
 *  loadSound() interface. Environmental sounds are completely controlled
 *  here, from creation to destruction.
 *
 *	The SoundManager is a SINGLETON CLASS.
 *	The SoundManager is handled by the SoundHandler.
 *
 *  Created on: May 9, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#include <OgreResourceGroupManager.h>

#include "SoundManager.h"
#include "SoundEnums.h"
#include "SoundBuffer.h"
#include "BufferBuilder.h"
#include "SoundSource.h"
#include "LSS/LSoundSource.h"
#include "SSS/SSoundSource.h"
#include "SoundAPI.h"
#include <debug/DebugUtil.h>


/* Multiplatform auxiliary function */
#if defined(_WIN32) || defined(CYGWIN)
static inline bool
fileExists(std::string fname)
{
	return System::IO::File::Exists(fname);
}
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  include <unistd.h>
static inline bool
fileExists(std::string fname)
{
	return !access(fname.c_str(), R_OK);
}
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif

#ifndef MAX
#  define  MAX(a,b)  ((a)>(b)?(a):(b))
#endif
#ifndef MIN
#  define  MIN(a,b)  ((a)>(b)?(b):(a))
#endif



// Expected maximum number of parallel playing sounds
#define  NUM_PARALLEL_SOUNDS  (1<<6)  /* 64 */


namespace mm {


////////////////////////////////////////////////////////////////////////////////
SoundManager::SoundManager() : mCam(0)
{
	float ori[6] = {0.0, 0.0, -1.0,	 // 'at' vector (i.e. my nose)
			 	    0.0, 1.0, 0.0};	 // 'up' vector (i.e. top of head)

	mActiveSounds.reserve(NUM_PARALLEL_SOUNDS);

	/* Create sound context on default sound device. */
	setSoundDevice(NULL);

	/* Initialize listener propperties */
	alListenerf(AL_GAIN, 1.0f);
	alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
	alListener3f(AL_VELOCITY, 0.0, 0.0, 0.0);
	alListenerfv(AL_ORIENTATION, ori);
}


////////////////////////////////////////////////////////////////////////////////
SoundManager::~SoundManager()
{
	ALCcontext* context = alcGetCurrentContext();
	ALCdevice*  device  = alcGetContextsDevice(context);

	/* Stop and erase remaining active sounds */
	globalStop();

	/* Delete sources */
	for (int i = mAvailableLSS.size()-1 ; i >= 0 ; i--) {
			delete mAvailableLSS[i];
	}
	mAvailableLSS.clear();
	for (int i = mAvailableSSS.size()-1 ; i >= 0 ; i--) {
			delete mAvailableSSS[i];
	}
	mAvailableSSS.clear();

	/* Free buffers */
	for (HashStrBuff::iterator it = mLoadedBuffers.begin() ;
			it != mLoadedBuffers.end() ; it++) {
		// Can't call clear() method directly because values are pointers.
		delete it->second;
	}
	mLoadedBuffers.clear();

	/* Destroy OpenAL context */
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

	return;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::fadeUpdate(ActiveSound& s, const float globalTimeFrame)
{
	ALint playing(AL_NONE);
	float value(0.0f);

	ASSERT(alGetError() == AL_NO_ERROR);

	if (!s.mSource->isPlaying()
			&& s.mPlayState   != SSplayback::SS_FADING_IN
			&& s.mGlobalState != SSplayback::SS_FADING_IN) {
		// Already paused or stopped, return swiftly.
		return;
	}

	// FADE_OUT
	if ((s.mPlayState &
			(SSplayback::SS_FADING_OUT | SSplayback::SS_FADING_OUT_AND_PAUSE)) ||
		(s.mGlobalState &
			(SSplayback::SS_FADING_OUT | SSplayback::SS_FADING_OUT_AND_PAUSE))) {

		// Slowly dissappear: reduce source's gain value
		s.mFadeAccTime -= globalTimeFrame;
		value = s.mFadeTime == 0.0f ? 0.0f	// Beware of 0 division
									: MAX(0.0f, s.mFadeAccTime / s.mFadeTime);
		alSourcef(s.mSource->mSource, AL_GAIN, value*s.mVolume);

		if (value <= 0.0f && ((s.mPlayState | s.mGlobalState)
							  & SSplayback::SS_FADING_OUT_AND_PAUSE)) {
			// Fading finished, and playback pause requested.
			alSourcePause(s.mSource->mSource);
			// However, we won't modify the global/individual state.
		}

	// FADE_IN
	} else if (s.mPlayState == SSplayback::SS_FADING_IN
				|| s.mGlobalState == SSplayback::SS_FADING_IN) {

		// Slowly materialize: increase source's gain value
		s.mFadeAccTime += globalTimeFrame;
		value = MIN(s.mFadeAccTime / s.mFadeTime, 1.0);
		alSourcef(s.mSource->mSource, AL_GAIN, value*s.mVolume);

		if (!playing) {
			// A previous fade-out paused the sound, we must restart it.
			s.mSource->play(NULL,
							value*s.mVolume,
							Ogre::Vector3(0.0f,0.0f,0.0f),
							s.mSource->getRepeat());
		}

		if (value >= 1.0) {
			// Fading finished
			s.mPlayState = SSplayback::SS_PLAYING;
			s.mGlobalState = SSplayback::SS_NONE;
		}
	}

	ASSERT(alGetError() == AL_NO_ERROR);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::playExistentSound(ActiveSound& s, float gain, bool repeat)
{
	Ogre::Vector3 origin(0.0f,0.0f,0.0f);
	SSerror err(SSerror::SS_NO_ERROR);
	ALenum st(AL_NONE);

	ASSERT(alGetError() == AL_NO_ERROR);

	if ((s.mPlayState| s.mGlobalState) & SSplayback::SS_FADING_OUT_AND_PAUSE) {
		// Completely faded out? If paused we must start playback.
		alGetSourcei(s.mSource->mSource, AL_SOURCE_STATE, &st);
	}

	if ((( s.mPlayState | s.mGlobalState) & SSplayback::SS_PAUSED) ||
		(((s.mPlayState | s.mGlobalState) & SSplayback::SS_FADING_OUT_AND_PAUSE)
		 && st == AL_PAUSED)) {
		// Paused: start playback again.
		ASSERT(s.mSource->isActive() && !s.mSource->isPlaying());
		err = s.mSource->play(NULL, gain, origin, repeat);
		if (err == SSerror::SS_NO_ERROR) {
			s.mPlayState = SSplayback::SS_PLAYING;
			s.mGlobalState = SSplayback::SS_NONE;
			s.mVolume = gain;
		} else {
			debugERROR("Error #%d while trying to restart playback: ", err);
		}
	}

//	if ((s.mPlayState | s.mGlobalState) & (SSplayback::SS_PLAYING
//										  |SSplayback::SS_FADING_IN
//										  |SSplayback::SS_FADING_OUT))
//	Playing: do nothing.

	ASSERT(alGetError() == AL_NO_ERROR);
	return err;
}


/******************************************************************************/
/******************************************************************************/
/**************************    INITIALIZATION    ******************************/


////////////////////////////////////////////////////////////////////////////////
std::vector<std::string>
SoundManager::getAvailableSoundDevices()
{
	int pos = 0,
		MAX_POS = 1<<10;
	const ALCchar* device  = NULL;
	const ALCchar* devices = NULL;
	std::vector<std::string> devsVec;

	if (!alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT")) {
		debug("%s","Can't retrieve sound devices info. "
				"Missing extension \"ALC_ENUMERATION_EXT\".\n");
		return devsVec;
	}

	devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	// "devices" contains the device names, separated by NULL
	// and terminated by two consecutive NULLs.
	pos = 0;
	do {
		device = &(devices[pos]);
		if (*device != '\0') {
			devsVec.push_back(device);
			pos += strlen(device)+1;
		}
	} while (*device != '\0' && pos < MAX_POS);

	if (pos >= MAX_POS) {
		debug("%s","Error retrieving sound devices info. List was too long.\n");
		devsVec.clear();
	}

	return devsVec;
}


////////////////////////////////////////////////////////////////////////////////
std::string
SoundManager::getSoundDevice()
{
	if (!alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT")) {
		debug("%s","Can't retrieve sound devices info. "
				"Missing extension \"ALC_ENUMERATION_EXT\".\n");
		return std::string("");
	} else {
		ALCdevice*  dev = alcGetContextsDevice(alcGetCurrentContext());
		return alcGetString(dev, ALC_DEVICE_SPECIFIER);
	}
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::setSoundDevice(std::string* devName)
{
	ALCcontext* oldContext = NULL;
	ALCcontext* newContext = NULL;
	ALCdevice* device = devName ? alcOpenDevice(devName->c_str())
								: alcOpenDevice(NULL);

	/* To hell with my coding style */
	if (device) {
		debug("Opened sound device: %s\n",
				alcGetString(device, ALC_DEVICE_SPECIFIER));

		newContext = alcCreateContext(device, NULL);
		if(!newContext) {
			goto fail;
		}

		oldContext = alcGetCurrentContext();  // Get current context, if any.
		if (!alcMakeContextCurrent(newContext)) {
			goto fail;
		}

		/* Cleanup old stuff */
		if (oldContext) {
			bool closeDevice = device != alcGetContextsDevice(oldContext);
			device = alcGetContextsDevice(oldContext);
			alcDestroyContext(oldContext);
			if (closeDevice) {
				alcCloseDevice(device);
			}
		}
		return SSerror::SS_NO_ERROR;
	}
	fail:
		debug("Couldn't use \"%s\" sound device.\n",
				(devName ? devName->c_str() : "default"));
		debugERROR("alcGetError returns error_code %d\n", alcGetError(device));
		if (newContext) {
			alcDestroyContext(newContext);
		}
		if (device) {
			alcCloseDevice(device);
		}
		return SSerror::SS_INTERNAL_ERROR;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::addSSoundSources(unsigned int numSources)
{
	for (int i=0 ; i<numSources ; i++) {
		mAvailableSSS.push_back(new SSoundSource);
	}
	return SSerror::SS_NO_ERROR;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::addLSoundSources(unsigned int numSources)
{
	for (int i=0 ; i<numSources ; i++) {
		mAvailableLSS.push_back(new LSoundSource);
	}
	return SSerror::SS_NO_ERROR;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::removeSoundSources(unsigned int numSources)
{
	/* TODO: Para un futuro, por ahora no la implementamos. */
	debugERROR("Función no implementada: %s\n", "SoundManager::removeSoundSources()");
	ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::loadSound(const Ogre::String& sName, SSformat format, SSbuftype type)
{
	std::string sNameFullPath;
	SSerror err = SSerror::SS_NO_ERROR;
	SoundBuffer* buffer(0);

	// Check if buffer had already been loaded.
	if (isSoundLoaded(sName)) {
		debugWARNING("Sound \"%s\" had already been loaded in the system.",
						sName.c_str());
		return SSerror::SS_NO_BUFFER;
	}

	// First find audio file absolute path.
	Ogre::ResourceGroupManager& resGM = Ogre::ResourceGroupManager::getSingleton();
	Ogre::FileInfoListPtr files = resGM.findResourceFileInfo(
			SOUNDS_RESOURCE_GROUP_NAME, sName);

	if (files.isNull()) {
		debug("%s","Recurso no encontrado.\n");
		return SSerror::SS_FILE_NOT_FOUND;

	} else {
		Ogre::FileInfoList::iterator it;
		for (it = files->begin() ; it < files->end() ; it++) {
			// Compose audio file absolute path.
			sNameFullPath.append(it->archive->getName()+"/"+sName);
			if (fileExists(sNameFullPath)) {
				debug("\n Loading audio file %s\n", sNameFullPath.c_str());
				break;
			} else {
				sNameFullPath.clear();
			}
		}
		// Found?
		if (it == files->end() || sNameFullPath.size() <= 0) {
			debug("%s","Recurso no encontrado.\n");
			return SSerror::SS_FILE_NOT_FOUND;
		}
	}

	// Try to load audio file into buffer
	err = BufferBuilder::bufferFromFile(sNameFullPath, &buffer, format, type);

	// Register new available sound
	if (err == SSerror::SS_NO_ERROR) {
		ASSERT(buffer);
		mLoadedBuffers.insert(std::make_pair(sName,buffer));
		ASSERT(mLoadedBuffers.find(sName)->second);
	}

	return err;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::unloadSound(const Ogre::String& sName)
{
	HashStrBuff::iterator it = mLoadedBuffers.find(sName);
	if (it != mLoadedBuffers.end()) {
		delete it->second;
		mLoadedBuffers.erase(it->first);
	}
	return;
}



/******************************************************************************/
/******************************************************************************/
/********************    GLOBAL PLAYBACK CONTROLS    **************************/


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::update(const float globalTimeFrame,
					 std::vector<EnvSoundId> *finished,
					 std::vector<EnvSoundId> *paused)
{
	ActiveSound* as;
	SSplayback st = SSplayback::SS_FINISHED;

	// Update Listener's position and orientation, if camera was initialized.
	if (mCam != NULL) {
		// Position
		Ogre::Vector3 pos(mCam->getDerivedPosition());
		// Orientation, quaternion notation
		Ogre::Quaternion quatOri(mCam->getDerivedOrientation());
		// Orientation, AT/UP vector notation
		Ogre::Vector3 at, up;
		float ori[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

		// Transform quaternion to at/up vector orientation.
		// See: http://goo.gl/s5omf
		up = (quatOri * Ogre::Vector3::UNIT_Y);
		at = (quatOri * Ogre::Vector3::NEGATIVE_UNIT_Z);
		ori[0] = at.x; ori[1] = at.y; ori[2] = at.z;
		ori[3] = up.x, ori[4] = up.y, ori[5] = up.z;

		alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
		alListenerfv(AL_ORIENTATION, ori);
	}

	// Update environmental sounds first.
	for (int i=0 ; i < mEnvSounds.size() ; i++) {

		as = std::get<1>(mEnvSounds[i]);
		st = as->mSource->update();

		if (st == SSplayback::SS_FINISHED) {
			if (0 != finished) {
				// Register ID of sound termination in "finished" vector
				finished->push_back((void*)std::get<2>(mEnvSounds[i]));
			}
			// Buffer was automatically detached from source.
			// Erase EnvSound and recycle SoundSource.
			stopEnvSound(std::get<0>(mEnvSounds[i]));
			if (mEnvSounds.size() > 0) i--;  // =D

		} else if ((as->mPlayState | as->mGlobalState)
				  	  & ( SSplayback::SS_FADING_IN
				  		| SSplayback::SS_FADING_OUT
				  		| SSplayback::SS_FADING_OUT_AND_PAUSE)) {
			// Update current gain accoring to fading state.
			fadeUpdate(*as, globalTimeFrame);
			if (0 != paused && !as->mSource->isPlaying()) {
				// Paused during fade-out, register ID in "paused" vector
				paused->push_back((void*)std::get<2>(mEnvSounds[i]));
			}
		}
	}

	// Update unit sounds.
	for (int i=0 ; i < mUnitSounds.size() ; i++) {

		as = mUnitSounds[i].second;
		st = as->mSource->update(mUnitSounds[i].first->getPosition());

		if (st == SSplayback::SS_FINISHED) {
			// Buffer was automatically detached from source.
			// Erase UnitSound and recycle SoundSource.
			stopSound(*mUnitSounds[i].first);
			if (mUnitSounds.size() > 0) { i--; }  // Woooaaaahhh!!!

		} else if ((as->mPlayState | as->mGlobalState)
				  	  & ( SSplayback::SS_FADING_IN
				  		| SSplayback::SS_FADING_OUT
				  		| SSplayback::SS_FADING_OUT_AND_PAUSE)) {
			fadeUpdate(*as, globalTimeFrame);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::globalPause()
{
	for (uint i=0 ; i < mActiveSounds.size() ; i++) {
		// All currently playing sounds get ("globally") paused
		if (mActiveSounds[i]->mSource->isPlaying()) {
			mActiveSounds[i]->mSource->pause();
			mActiveSounds[i]->mPlayState = SSplayback::SS_NONE;
			mActiveSounds[i]->mGlobalState = SSplayback::SS_PAUSED;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::globalPlay()
{
	SSerror err(SSerror::SS_NO_ERROR);
	Ogre::Vector3 o(0.0f, 0.0f, 0.0f);

	for (uint i=0 ; i < mActiveSounds.size() ; i++) {
		// If it had been globally paused
		if (mActiveSounds[i]->mGlobalState == SSplayback::SS_PAUSED) {
			err = mActiveSounds[i]->mSource->play(
					NULL, mActiveSounds[i]->mVolume,
					o,    mActiveSounds[i]->mSource->getRepeat());
			if (err == SSerror::SS_NO_ERROR) {
				mActiveSounds[i]->mPlayState = SSplayback::SS_PLAYING;
				mActiveSounds[i]->mGlobalState = SSplayback::SS_NONE;
			} else {
				debugERROR("globalPlay() failed on sound #%d\n", i);
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::globalStop()
{
	for (uint i=0 ; i < mActiveSounds.size() ; i++) {
		mActiveSounds[i]->mSource->stop();
		// Reset OpenAL source properties
		alSourcei(mActiveSounds[i]->mSource->mSource, AL_BUFFER, AL_NONE);
		alSourcei(mActiveSounds[i]->mSource->mSource, AL_SOURCE_RELATIVE, AL_FALSE);
		// Save back SoundSource in corresponding source queue
		if (mActiveSounds[i]->mSource->getType() == SSsrctype::SS_SRC_LOADED) {
			mAvailableLSS.push_back((LSoundSource*) mActiveSounds[i]->mSource);
		} else {
			ASSERT(mActiveSounds[i]->mSource->getType() == SSsrctype::SS_SRC_STREAM);
			mAvailableSSS.push_back((SSoundSource*) mActiveSounds[i]->mSource);
		}
		delete mActiveSounds[i];
	}
	/** TODO: check for memory leaks! */
	mActiveSounds.clear();
	mEnvSounds.clear();
	mUnitSounds.clear();
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::globalRestart()
{
	SSerror err(SSerror::SS_NO_ERROR);
	Ogre::Vector3 o(0.0f,0.0f,0.0f);

	for (uint i=0 ; i < mActiveSounds.size() ; i++) {
		err = mActiveSounds[i]->mSource->restart(
				mActiveSounds[i]->mVolume, o, mActiveSounds[i]->mSource->getRepeat());
		/* For both environmental and unit sounds we restart playback at the *
		 * origin of the system (i.e., position (0,0,0))                     *
		 * This isn't strictly correct for the latter, but update() should   *
		 * be called shortly afterwards, setting the unit's real position.   */
		if (err == SSerror::SS_NO_ERROR) {
			mActiveSounds[i]->mPlayState = SSplayback::SS_PLAYING;
			mActiveSounds[i]->mGlobalState = SSplayback::SS_NONE;
		} else {
			debugERROR("globalRestart() failed on sound #%d\n", i);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::globalFadeOut(const Ogre::Real& time, const bool pause)
{
	for (uint i=0 ; i < mActiveSounds.size() ; i++) {

		if ((mActiveSounds[i]->mPlayState &
				( SSplayback::SS_PLAYING
				| SSplayback::SS_FADING_IN
				| SSplayback::SS_FADING_OUT
				| SSplayback::SS_FADING_OUT_AND_PAUSE))
			||
			(mActiveSounds[i]->mGlobalState &
				( SSplayback::SS_PLAYING
				| SSplayback::SS_FADING_IN))) {

			ALSource src = mActiveSounds[i]->mSource->mSource;
			// Set times.
			mActiveSounds[i]->mFadeTime = time >= 0.0 ? time : 1.0f;
			mActiveSounds[i]->mFadeAccTime = mActiveSounds[i]->mFadeTime;
			// If current volume is the 'default', save it.
			if ((mActiveSounds[i]->mPlayState | mActiveSounds[i]->mGlobalState)
					& SSplayback::SS_PLAYING) {
				alGetSourcef(src, AL_GAIN, &mActiveSounds[i]->mVolume);
			}
			// Override individual status.
			mActiveSounds[i]->mPlayState = SSplayback::SS_NONE;
			mActiveSounds[i]->mGlobalState = pause ? SSplayback::SS_FADING_OUT_AND_PAUSE
												   : SSplayback::SS_FADING_OUT;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::globalFadeIn(const Ogre::Real& time)
{
	for (uint i=0 ; i < mActiveSounds.size() ; i++) {
		if (mActiveSounds[i]->mGlobalState &
				( SSplayback::SS_PAUSED
				| SSplayback::SS_FADING_OUT
				| SSplayback::SS_FADING_OUT_AND_PAUSE)) {
			// Set times
			mActiveSounds[i]->mFadeTime = time >= 0.0 ? time : 1.0f;
			mActiveSounds[i]->mFadeAccTime = 0.0f;
			// Override individual status.
			mActiveSounds[i]->mPlayState = SSplayback::SS_NONE;
			mActiveSounds[i]->mGlobalState = SSplayback::SS_FADING_IN;
		}
	}
}



/******************************************************************************/
/******************************************************************************/
/**********************    ENVIRONMENTAL SOUNDS    ****************************/


////////////////////////////////////////////////////////////////////////////////
bool
SoundManager::isPlayingEnvSound(const Ogre::String& sName)
{
	// Check whether "sName" is a playing environmental sound.
	for (uint i=0 ; i < mEnvSounds.size() ; i++) {
		if (std::get<0>(mEnvSounds[i]) == sName) {
			return std::get<1>(mEnvSounds[i])->mSource->isPlaying();
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
bool
SoundManager::isActiveEnvSound(const Ogre::String& sName)
{
	// Check whether "sName" is an active environmental sound.
	for (uint i=0 ; i < mEnvSounds.size() ; i++) {
		if (std::get<0>(mEnvSounds[i]) == sName) {
			ASSERT(std::get<1>(mEnvSounds[i])->mSource->isActive());
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
bool
SoundManager::getEnvSoundRepeat(const Ogre::String& sName)
{
	// If the environmental sound "sName" exists, return its repeat value.
	for (uint i=0 ; i < mEnvSounds.size() ; i++) {
		if (std::get<0>(mEnvSounds[i]) == sName) {
			return std::get<1>(mEnvSounds[i])->mSource->getRepeat();
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::playEnvSound(const Ogre::String& sName,
						   const Ogre::Real& gain,
						   bool repeat,
						   EnvSoundId id)
{
	SoundBuffer* buf(0);
	SoundSource* src(0);
	Ogre::Vector3 origin(0.0f, 0.0f, 0.0f);
	SSerror err(SSerror::SS_NO_ERROR);
	HashStrBuff::const_iterator it(mLoadedBuffers.end());

	// Check whether "sName" is an active environmental sound.
	for (uint i=0 ; i < mEnvSounds.size() ; i++) {
		if (std::get<0>(mEnvSounds[i]) == sName) {
			return playExistentSound(*(std::get<1>(mEnvSounds[i])), gain, repeat);
		}
	}

	// Lookup buffer.
	it = mLoadedBuffers.find(sName);
	if (it == mLoadedBuffers.end()) {
		debugWARNING("Sound \"%s\" wasn't loaded.\n", sName.c_str());
		return SSerror::SS_FILE_NOT_FOUND;
	} else {
		ASSERT(it->second);
		buf = it->second;
	}

	// Get free source.
	if (buf->type == SSbuftype::SS_BUF_LOADED) {
		if (mAvailableLSS.empty()) {
			debug("No free LSources to play env. sound \"%s\"\n", sName.c_str());
			return SSerror::SS_NO_SOURCES;
		} else {
			src = mAvailableLSS.front();
			mAvailableLSS.pop_front();
		}
	} else { // buffer->type == SSbuftype::SS_BUF_STREAM
		if (mAvailableSSS.empty()) {
			debug("No free SSources to play env. sound \"%s\"\n", sName.c_str());
			return SSerror::SS_NO_SOURCES;
		} else {
			src = mAvailableSSS.front();
			mAvailableSSS.pop_front();
		}
	}

	// Start playback and register sound.
	err = src->play(buf, gain, origin, repeat);
	if (err == SSerror::SS_NO_ERROR) {
		// Success.
		alSourcei(src->mSource, AL_SOURCE_RELATIVE, AL_TRUE);
		ActiveSound* newSound = new ActiveSound(src,  SSplayback::SS_PLAYING,
												gain, mActiveSounds.size());
		mActiveSounds.push_back(newSound);
		mEnvSounds.push_back(EnvSound(sName, newSound, id));

	} else {
		// Failure.
		debugERROR("Failed starting environmental sound \"%s\".", sName.c_str());
		if (src->getType() == SSsrctype::SS_SRC_LOADED) {
			ASSERT(dynamic_cast<LSoundSource*>(src) != NULL);
			mAvailableLSS.push_back((LSoundSource*)src);
		} else {
			ASSERT(src->getType() == SSsrctype::SS_SRC_STREAM);
			ASSERT(dynamic_cast<SSoundSource*>(src) != NULL);
			mAvailableSSS.push_back((SSoundSource*)src);
		}
	}

	return err;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::stopEnvSound(const Ogre::String& sName)
{
	uint pos(0);
	ActiveSound* as(0);

	/* Check whether "sName" is an active environmental sound. */
	for (pos=0 ; pos < mEnvSounds.size() ; pos++) {
		if (std::get<0>(mEnvSounds[pos]) == sName) {
			as = std::get<1>(mEnvSounds[pos]);
			break;
		}
	}

	if (!as) {
		// No environmental sound by the name "sName"
		return SSerror::SS_NO_BUFFER;
	}

	/* Stop and release SoundSource. */
	as->mSource->stop();
	alSourcei(as->mSource->mSource, AL_BUFFER, AL_NONE);
	alSourcei(as->mSource->mSource, AL_SOURCE_RELATIVE, AL_FALSE);
	if (as->mSource->getType() == SSsrctype::SS_SRC_LOADED) {
		/* LSoundSource */
		ASSERT(dynamic_cast<LSoundSource*>(as->mSource) != NULL);
		mAvailableLSS.push_back((LSoundSource*) as->mSource);
	} else {
		/* SSoundSource */
		ASSERT(as->mSource->getType() == SSsrctype::SS_SRC_STREAM);
		ASSERT(dynamic_cast<SSoundSource*>(as->mSource) != NULL);
		mAvailableSSS.push_back((SSoundSource*) as->mSource);
	}

	/* Desregister environmental sound. */
	mEnvSounds[pos] = mEnvSounds[mEnvSounds.size()-1];
	mEnvSounds.pop_back();
	mActiveSounds[as->mActivId] = mActiveSounds[mActiveSounds.size()-1];
	mActiveSounds[as->mActivId]->mActivId = as->mActivId;
	mActiveSounds.pop_back();
	delete as;

	if (alGetError() == AL_NO_ERROR) {
		return SSerror::SS_NO_ERROR;
	} else {
		return SSerror::SS_INTERNAL_ERROR;
	}
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::restartEnvSound(const Ogre::String& sName)
{
	SSerror err(SSerror::SS_NO_BUFFER);
	ActiveSound* as(0);

	// Check whether "sName" is an active environmental sound.
	for (uint i=0 ; i < mEnvSounds.size() ; i++) {
		if (std::get<0>(mEnvSounds[i]) == sName) {
			as = std::get<1>(mEnvSounds[i]);
			break;
		}
	}

	if (!as) {
		// No environmental sound by the name "sName"
		return err;
	}

	// Try to restart playback from the beginning of the buffer.
	err = as->mSource->restart(as->mVolume,
							   Ogre::Vector3(0.0f,0.0f,0.0f),
							   as->mSource->getRepeat());
	if (err == SSerror::SS_NO_ERROR) {
		// Success
		as->mPlayState = SSplayback::SS_PLAYING;
		as->mGlobalState = SSplayback::SS_NONE;
	} else {
		// Failure
		debugERROR("Failed to restart env. sound \"%s\"", sName.c_str());
		stopEnvSound(sName);
	}

	return err;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::fadeOutEnvSound(const Ogre::String& sName, const Ogre::Real& time,
							  const bool pause)
{
	ActiveSound* as(0);

	// Search environmental sound
	for (int i=0 ; i < mEnvSounds.size() ; i++) {
		if (std::get<0>(mEnvSounds[i]) == sName) {
			as = std::get<1>(mEnvSounds[i]);
			break;
		}
	}

	if (!as) {
		// No environmental sound by the name "sName"
		return SSerror::SS_NO_BUFFER;
	} else if (!((as->mPlayState | as->mGlobalState) & SSplayback::SS_PLAYING)) {
		// Sound state must not be modified by this function
		return SSerror::SS_NO_ERROR;
	}

	// Set times
	as->mFadeTime = time >= 0.0 ? time : 1.0f;
	as->mFadeAccTime = as->mFadeTime;
	// Save current volume
	alGetSourcef(as->mSource->mSource, AL_GAIN, &as->mVolume);
	// Register individual fading state
	as->mPlayState = pause ? SSplayback::SS_FADING_OUT_AND_PAUSE
						   : SSplayback::SS_FADING_OUT;
	as->mGlobalState = SSplayback::SS_NONE;

	return SSerror::SS_NO_ERROR;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::fadeInEnvSound(const Ogre::String& sName, const Ogre::Real& time)
{
	ActiveSound* as(0);

	// Search environmental sound
	for (int i=0 ; i < mEnvSounds.size() ; i++) {
		if (std::get<0>(mEnvSounds[i]) == sName) {
			as = std::get<1>(mEnvSounds[i]);
			break;
		}
	}

	if (!as) {
		// No environmental sound by the name "sName"
		return SSerror::SS_NO_BUFFER;
	} else if (!(( (as->mPlayState | as->mGlobalState) & SSplayback::SS_PAUSED)
				|| (as->mPlayState & (SSplayback::SS_FADING_OUT
									 |SSplayback::SS_FADING_OUT_AND_PAUSE)))) {
		// Sound state must not be modified by this function
		return SSerror::SS_NO_ERROR;
	}

	// Set times
	as->mFadeTime = time >= 0.0 ? time : 1.0f;
	as->mFadeAccTime = 0.0f;
	// Register individual fading state
	as->mPlayState = SSplayback::SS_FADING_IN;
	as->mGlobalState = SSplayback::SS_NONE;

	return SSerror::SS_NO_ERROR;
}



/******************************************************************************/
/******************************************************************************/
/***********************    UNITS' APIS SOUNDS    *****************************/


////////////////////////////////////////////////////////////////////////////////
bool
SoundManager::isActiveAPISound(const Ogre::String& sName) const
{
	for (uint i=0 ; i < mUnitSounds.size() ; i++) {
		if (sName == mUnitSounds[i].first->getCurrentSound()) {
			ASSERT(mUnitSounds[i].second->mSource->isActive());
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
bool
SoundManager::findPlayingAPI (const SoundAPI& sAPI) const
{
	if (sAPI.mActivationIndex < 0
		|| mUnitSounds.size() <= sAPI.mActivationIndex
		|| mUnitSounds[sAPI.mActivationIndex].first != &sAPI) {
		return false;
	} else {
		ALint st(AL_NONE);
		alGetSourcei(mUnitSounds[sAPI.mActivationIndex].second->mSource->mSource,
					 AL_SOURCE_STATE, &st);
		ASSERT((st==AL_PLAYING &&
					(mUnitSounds[sAPI.mActivationIndex].second->mGlobalState
						| SSplayback::SS_PLAYING
						| SSplayback::SS_FADING_IN
						| SSplayback::SS_FADING_OUT
						| SSplayback::SS_FADING_OUT_AND_PAUSE))
				|| (st==AL_PAUSED &&
					(mUnitSounds[sAPI.mActivationIndex].second->mGlobalState
						| SSplayback::SS_PAUSED
						| SSplayback::SS_FADING_OUT_AND_PAUSE)));
		return (st == AL_PLAYING);
	}
}


////////////////////////////////////////////////////////////////////////////////
bool
SoundManager::findActiveAPI (const SoundAPI& sAPI) const
{
	return (sAPI.mActivationIndex >= 0
			&& mUnitSounds.size() > sAPI.mActivationIndex
			&& mUnitSounds[sAPI.mActivationIndex].first == &sAPI);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::playSound(SoundAPI& sAPI,
					    const Ogre::String& sName,
					    const Ogre::Real& gain,
					    bool repeat)
{
	SSerror err = SSerror::SS_NO_ERROR;
	SoundBuffer* buf = NULL;
	SoundSource* src = NULL;
	HashStrBuff::iterator it;

	// Check whether the SoundAPI is already active.
	if (findActiveAPI(sAPI)) {
		return playExistentSound(*mUnitSounds[sAPI.mActivationIndex].second,
								 gain,
								 repeat);
	}

	// New sound: lookup buffer.
	it = mLoadedBuffers.find(sName);
	if (it == mLoadedBuffers.end()) {
		debug("Sound \"%s\" wasn't loaded.\n", sName.c_str());
		return SSerror::SS_FILE_NOT_FOUND;
	} else {
		ASSERT(it->second);
		buf = it->second;
	}

	// Get free source.
	if (buf->type == SSbuftype::SS_BUF_LOADED) {
		if (mAvailableLSS.empty()) {
			debug("No free LSources to play sound \"%s\".\n", sName.c_str());
			return SSerror::SS_NO_SOURCES;
		} else {
			src = mAvailableLSS.front();
			mAvailableLSS.pop_front();
		}
	} else { // buffer->type == SSbuftype::SS_BUF_STREAM
		if (mAvailableSSS.empty()) {
			debug("No free LSources to play sound \"%s\".\n", sName.c_str());
			return SSerror::SS_NO_SOURCES;
		} else {
			src = mAvailableSSS.front();
			mAvailableSSS.pop_front();
		}
	}

	// Sound distance fading characteristics (attenuation factors)
	alSourcef(src->mSource, AL_MAX_DISTANCE, MAX_ATTENUATION_DISTANCE);
	alSourcef(src->mSource, AL_REFERENCE_DISTANCE, MID_ATTENUATION_DISTANCE);

	// Start playback and register sound.
	err = src->play(buf, gain, sAPI.getPosition(), repeat);
	if (err == SSerror::SS_NO_ERROR) {
		// Success.
		ActiveSound* newSound = new ActiveSound(src,
												SSplayback::SS_PLAYING,
												gain,
												mActiveSounds.size());
		mActiveSounds.push_back(newSound);
		sAPI.mActivationIndex = mUnitSounds.size();
		mUnitSounds.push_back(UnitSound(&sAPI, newSound));

	} else {
		// Failure.
		debug("Failed starting unit sound \"%s\" playback.", sName.c_str());
		if (src->getType() == SSsrctype::SS_SRC_LOADED) {
			ASSERT(dynamic_cast<LSoundSource*>(src) != NULL);
			mAvailableLSS.push_back((LSoundSource*)src);
		} else {
			ASSERT(src->getType() == SSsrctype::SS_SRC_STREAM);
			ASSERT(dynamic_cast<SSoundSource*>(src) != NULL);
			mAvailableSSS.push_back((SSoundSource*)src);
		}
	}

	return err;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::pauseSound(const SoundAPI& sAPI)
{
	int idx = sAPI.mActivationIndex;
	if (findPlayingAPI(sAPI) &&
			mUnitSounds[idx].second->mGlobalState != SSplayback::SS_PAUSED) {
		mUnitSounds[idx].second->mSource->pause();
		mUnitSounds[idx].second->mPlayState = SSplayback::SS_PAUSED;
		mUnitSounds[idx].second->mGlobalState = SSplayback::SS_NONE;
	}
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::stopSound(SoundAPI& sAPI)
{
	ActiveSound* as(0);

	if (!findActiveAPI(sAPI)) {
		return SSerror::SS_NO_BUFFER;
	} else {
		as = mUnitSounds[sAPI.mActivationIndex].second;
	}

	// Stop and free source.
	as->mSource->stop();
	alSourcei(as->mSource->mSource, AL_BUFFER, AL_NONE);
	if (as->mSource->getType() == SSsrctype::SS_SRC_LOADED) {
		// LSoundSource
		ASSERT(dynamic_cast<LSoundSource*>(as->mSource) != NULL);
		mAvailableLSS.push_back((LSoundSource*) as->mSource);
	} else { // getType() == SS_SRC_STREAM
		// SSoundSource
		ASSERT(as->mSource->getType() == SSsrctype::SS_SRC_STREAM);
		ASSERT(dynamic_cast<SSoundSource*>(as->mSource) != NULL);
		mAvailableSSS.push_back((SSoundSource*) as->mSource);
	}

	// Unregister unit sound.
	mUnitSounds[sAPI.mActivationIndex] = mUnitSounds[mUnitSounds.size()-1];
	mUnitSounds[sAPI.mActivationIndex].first->mActivationIndex = sAPI.mActivationIndex;
	mUnitSounds.pop_back();
	mActiveSounds[as->mActivId] = mActiveSounds[mActiveSounds.size()-1];
	mActiveSounds[as->mActivId]->mActivId = as->mActivId;
	mActiveSounds.pop_back();
	sAPI.mActivationIndex = -1;
	delete as;

	if (alGetError() == AL_NO_ERROR) {
		return SSerror::SS_NO_ERROR;
	} else {
		return SSerror::SS_INTERNAL_ERROR;
	}
}



////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::restartSound(SoundAPI& sAPI)
{
	SSerror err(SSerror::SS_NO_BUFFER);
	ActiveSound* as(0);

	if (!findActiveAPI(sAPI)) {
		return SSerror::SS_NO_BUFFER;
	} else {
		as = mUnitSounds[sAPI.mActivationIndex].second;
	}

	// Try to restart playback from the beginning of the buffer
	err = as->mSource->restart(as->mVolume,
							   sAPI.getPosition(),
							   as->mSource->getRepeat());
	if (err == SSerror::SS_NO_ERROR) {
		// Success
		as->mPlayState = SSplayback::SS_PLAYING;
		as->mGlobalState = SSplayback::SS_NONE;
	} else {
		// Failure
		debugERROR("Failed to restart unit sound \"%s\"",
					sAPI.getCurrentSound().c_str());
		stopSound(sAPI);
	}

	return err;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::fadeOutSound(SoundAPI& sAPI, const Ogre::Real& time, const bool pause)
{
	ActiveSound* as(0);

	if (!findActiveAPI(sAPI)) {
		return SSerror::SS_NO_BUFFER;
	} else {
		as = mUnitSounds[sAPI.mActivationIndex].second;
	}

	if (!((as->mPlayState | as->mGlobalState) & SSplayback::SS_PLAYING)) {
		// Sound state must not be modified by this function
		return SSerror::SS_NO_ERROR;
	}

	// Set times
	as->mFadeTime = time >= 0.0 ? time : 1.0f;
	as->mFadeAccTime = as->mFadeTime;
	// Save current volume
	alGetSourcef(as->mSource->mSource, AL_GAIN, &as->mVolume);
	// Register individual fading state
	as->mPlayState = pause ? SSplayback::SS_FADING_OUT_AND_PAUSE
						   : SSplayback::SS_FADING_OUT;
	as->mGlobalState = SSplayback::SS_NONE;

	return SSerror::SS_NO_ERROR;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::fadeInSound(SoundAPI& sAPI, const Ogre::Real& time)
{
	ActiveSound* as(0);

	if (!findActiveAPI(sAPI)) {
		return SSerror::SS_NO_BUFFER;
	} else {
		as = mUnitSounds[sAPI.mActivationIndex].second;
	}

	if (!(( (as->mPlayState | as->mGlobalState) & SSplayback::SS_PAUSED)
			|| (as->mPlayState & (SSplayback::SS_FADING_OUT
								 |SSplayback::SS_FADING_OUT_AND_PAUSE)))) {
		// Sound state must not be modified by this function
		return SSerror::SS_NO_ERROR;
	}

	// Set times
	as->mFadeTime = time >= 0.0 ? time : 1.0f;
	as->mFadeAccTime = 0.0f;
	// Register individual fading state
	as->mPlayState = SSplayback::SS_FADING_IN;
	as->mGlobalState = SSplayback::SS_NONE;

	return SSerror::SS_NO_ERROR;
}

}
