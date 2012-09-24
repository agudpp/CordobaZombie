/*
 * SoundManager.cpp
 *
 *	This class connects the SoundAPIs from the Units
 *	with the internal SoundSources of the sound system.
 *
 *	It handles the sound system playback by periodically updating all sources.
 *	It's a singleton class.
 *
 *  Created on: May 9, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */


#include "DebugUtil.h"
#include "GlobalObjects.h"

#include "SoundManager.h"

#include "SoundEnums.h"
#include "SoundBuffer.h"
#include "BufferBuilder.h"
#include "SoundSource.h"
#include "LSoundSource.h"
#include "SSoundSource.h"
#include "SoundAPI.h"


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
#define  NUM_PARALLEL_SOUNDS  1<<6  /* 64 */


/**
 ** TODO Actualizar todas las funciones para que consideren fading in/out.
 **/


/**
 ** TODO Actualizar todas las funciones luego de la refactorización
 **      de las estructuras internas EnvSound y ActiveSound.
 **/


////////////////////////////////////////////////////////////////////////////////
SoundManager::SoundManager() : mCam(0)
{
	float ori[6] = {0.0, 0.0, -1.0,	// 'at' vector (i.e. my nose)
			 	    0.0, 1.0, 0.0};	// 'up' vector (i.e. top of head)

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
	for (int i = (int) mActiveSounds.size()-1 ; i >= 0  ; i--) {
		stopSound(*(mActiveSounds[i].sAPI));
		mActiveSounds.pop_back();
	}

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
		delete it->second;
		mLoadedBuffers.erase(it);
	}

	/* Destroy OpenAL context */
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

	return;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::fadeUpdate(SoundManager::EnvSound& s)
{
	ALint playing(AL_NONE);
	float value(0.0f);

	alGetSourcei(s.source->mSource, AL_PLAYING, &playing);
	if (!playing
			&& s.fadeStatus  != SoundManager::FADING_IN
			&& s.globalState != SSplayback::SS_FADING_IN) {
		/* Already paused or stopped, return swiftly. */
		return;
	}

	// FADE_OUT
	if ((s.fadeStatus &
			(SoundManager::FADING_OUT  | SoundManager::FADING_OUT_AND_PAUSE)) ||
		(s.globalState &
			(SSplayback::SS_FADING_OUT | SSplayback::SS_FADING_OUT_AND_PAUSE))) {

		// Slowly dissappear: reduce source's gain value
		s.fadeAccTime -= GLOBAL_TIME_FRAME;
		value = MAX(s.fadeAccTime / s.fadeTime, 0.0f);
		alSourcef(s.source->mSource, AL_GAIN, value);

		if (value <= 0.0f &&
			(s.fadeStatus == SoundManager::FADING_OUT_AND_PAUSE
			|| s.globalState == SSplayback::SS_FADING_OUT_AND_PAUSE)) {
			// Fading finished, and playback pause requested.
			alSourcePause(s.source->mSource);
		}

	// FADE_IN
	} else if (s.fadeStatus == SoundManager::FADING_IN
				|| s.globalState == SSplayback::SS_FADING_IN) {

		// Slowly materialize: increase source's gain value
		s.fadeAccTime += GLOBAL_TIME_FRAME;
		value = MIN(s.fadeAccTime / s.fadeTime, s.volume);
		alSourcef(s.source->mSource, AL_GAIN, value);

		if (!playing) {
			// A previous fade-out paused the sound, we must restart it.
			alSourcePlay(s.source->mSource);
		}

		if (value >= s.volume) {
			// Fading finished
			s.fadeStatus  = SoundManager::FADING_NONE;
			s.globalState = SSplayback::SS_PLAYING;
		}

	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::fadeUpdate(SoundManager::ActiveSound& s)
{
	/** TODO: fadeUpdate(ActiveSound) */
	debugRED("TODO: fadeUpdate(ActiveSound)%s", "\n");
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

	/* First find audio file absolute path */
	Ogre::ResourceGroupManager& resGM = Ogre::ResourceGroupManager::getSingleton();
	Ogre::FileInfoListPtr files = resGM.findResourceFileInfo(
			SOUNDS_RESOURCE_GROUP_NAME, sName);

	if (files.isNull()) {
		debug("%s","Recurso no encontrado.\n");
		return SSerror::SS_FILE_NOT_FOUND;

	} else {
		Ogre::FileInfoList::iterator it;
		for (it = files->begin() ; it < files->end() ; it++) {
			/* Compose audio file absolute path */
			sNameFullPath.append(it->archive->getName()+"/"+sName);
			if (fileExists(sNameFullPath)) {
				debug("\n Loading audio file %s\n", sNameFullPath.c_str());
				break;
			} else {
				sNameFullPath.clear();
			}
		}
		/* Found? */
		if (it == files->end() || sNameFullPath.size() <= 0) {
			debug("%s","Recurso no encontrado.\n");
			return SSerror::SS_FILE_NOT_FOUND;
		}
	}

	/* Try to load audio file into buffer */
	err = BufferBuilder::bufferFromFile(sNameFullPath, &buffer, format, type);

	/* Register new available sound */
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
SoundManager::update()
{
	ActiveSound* as;
	Ogre::Vector3 pos(0.0f,0.0f,0.0f);
	SSplayback st = SSplayback::SS_FINISHED;

	/* Update Listener's position and orientation, if camera was initialized. */
	if (mCam != NULL) {
		/* Position */
		pos = mCam->getDerivedPosition();
		/* Orientation, quaternion notation */
		Ogre::Quaternion quatOri = mCam->getDerivedOrientation();
		/* Orientation, AT/UP vector notation */
		Ogre::Vector3 at, up;
		float ori[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

		/* Transform quaternion to at/up vector orientation.
		 * See: http://goo.gl/s5omf */
		up = (quatOri * Ogre::Vector3::UNIT_Y);
		at = (quatOri * Ogre::Vector3::NEGATIVE_UNIT_Z);

		ori[0] = at.x;
		ori[1] = at.y;
	        ori[2] = at.z;
		ori[3] = up.x;
		ori[4] = up.y;
                ori[5] = up.z;

		alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
		alListenerfv(AL_ORIENTATION, ori);
	}

	/* Update active environmental sounds first. */
	for (int i=0 ; i<mEnvSounds.size() ; i++) {
		st = mEnvSounds[i].source->update();

		if (st == SSplayback::SS_FINISHED) {
			/* Buffer was automatically detached from source.
			 * Erase EnvSound and recycle SoundSource. */
			alSourcei(mEnvSounds[i].source->mSource, AL_SOURCE_RELATIVE, AL_FALSE);

			if (mEnvSounds[i].source->getType() == SSsrctype::SS_SRC_LOADED) {
				/* LSoundSource */
				ASSERT(dynamic_cast<LSoundSource*>(mEnvSounds[i].source) != NULL);
				mAvailableLSS.push_back((LSoundSource*) mEnvSounds[i].source);
			} else { /* getType() == SS_SRC_STREAM */
				/* SSoundSource */
				ASSERT(dynamic_cast<SSoundSource*>(mEnvSounds[i].source) != NULL);
				mAvailableSSS.push_back((SSoundSource*) mEnvSounds[i].source);
			}
			debug("Finished environmental sound \"%s\"\n", mEnvSounds[i].name.c_str());
			mEnvSounds[i] = mEnvSounds[mEnvSounds.size()-1];
			mEnvSounds.pop_back();
			if (mEnvSounds.size() > 0) { i--; }
		}
	}

	/* Update active unit sounds. */
	for (int i=0 ; i<mActiveSounds.size() ; i++) {
		as = &mActiveSounds[i];
		st = as->sSRC->update(as->sAPI->getPosition());

		if (st == SSplayback::SS_FINISHED) {
			/* Buffer was automatically detached from source.
			 * Erase ActiveSound and recycle SoundSource. */

			if (as->sSRC->getType() == SSsrctype::SS_SRC_LOADED) {
				/* LSoundSource */
				ASSERT(dynamic_cast<LSoundSource*>(as->sSRC) != NULL);
				mAvailableLSS.push_back((LSoundSource*) as->sSRC);
			} else { /* getType() == SS_SRC_STREAM */
				/* SSoundSource */
				ASSERT(dynamic_cast<SSoundSource*>(as->sSRC) != NULL);
				mAvailableSSS.push_back((SSoundSource*) as->sSRC);
			}
			mActiveSounds[i] = mActiveSounds[mActiveSounds.size()-1];
			mActiveSounds[i].sAPI->mActivationIndex = i;
			as->sAPI->mActivationIndex = -1;
			mActiveSounds.pop_back();
			if (mActiveSounds.size() > 0) { i--; }
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::globalPause()
{
	for (int i=0 ; i < mEnvSounds.size() ; i++) {
		if (mEnvSounds[i].source->isPlaying()) {
			mEnvSounds[i].source->pause();
			mEnvSounds[i].globalState = SSplayback::SS_PAUSED;
		}
	}
	for (int i=0 ; i < mActiveSounds.size() ; i++) {
		if (mActiveSounds[i].sSRC->isPlaying()) {
			mActiveSounds[i].sSRC->pause();
			mActiveSounds[i].globalState = SSplayback::SS_PAUSED;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::globalPlay()
{
	Ogre::Vector3 origin(0.0f, 0.0f, 0.0f);
	for (int i=0 ; i < mEnvSounds.size() ; i++) {
		if (mEnvSounds[i].globalState  == SSplayback::SS_PAUSED) {
			mEnvSounds[i].source->play(NULL, origin, mEnvSounds[i].source->getRepeat());
			mEnvSounds[i].globalState = SSplayback::SS_PLAYING;
		}
	}
	for (int i=0 ; i < mActiveSounds.size() ; i++) {
		if (mActiveSounds[i].globalState  == SSplayback::SS_PAUSED) {
			mActiveSounds[i].sSRC->play(NULL, origin, mActiveSounds[i].sSRC->getRepeat());
			mActiveSounds[i].globalState = SSplayback::SS_PLAYING;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::globalStop()
{
	for (int i=0 ; i < mEnvSounds.size() ; i++) {
		mEnvSounds[i].source->stop();
		alSourcei(mEnvSounds[i].source->mSource, AL_SOURCE_RELATIVE, AL_FALSE);
		if (mEnvSounds[i].source->getType() == SSsrctype::SS_SRC_LOADED) {
			mAvailableLSS.push_back((LSoundSource*) mEnvSounds[i].source);
		} else {
			ASSERT(mEnvSounds[i].source->getType() == SSsrctype::SS_SRC_STREAM);
			mAvailableSSS.push_back((SSoundSource*) mEnvSounds[i].source);
		}
	}
	for (int i=0 ; i < mActiveSounds.size() ; i++) {
		mActiveSounds[i].sSRC->stop();
		if (mActiveSounds[i].sSRC->getType() == SSsrctype::SS_SRC_LOADED) {
			mAvailableLSS.push_back((LSoundSource*) mActiveSounds[i].sSRC);
		} else {
			ASSERT(mActiveSounds[i].sSRC->getType() == SSsrctype::SS_SRC_STREAM);
			mAvailableSSS.push_back((SSoundSource*) mActiveSounds[i].sSRC);
		}
	}
	mEnvSounds.clear();
	mActiveSounds.clear();
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::globalRestart()
{
	for (int i=0 ; i < mEnvSounds.size() ; i++) {
		mEnvSounds[i].source->restart();
	}
	for (int i=0 ; i < mActiveSounds.size() ; i++) {
		mActiveSounds[i].sSRC->restart(mActiveSounds[i].sAPI->getPosition());
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::globalFadeOut(const Ogre::Real& time, const bool pause)
{
	for (int i=0 ; i < mEnvSounds.size() ; i++) {
		if (mEnvSounds[i].globalState == SSplayback::SS_PLAYING) {
			ALSource src = mEnvSounds[i].source->mSource;
			// Set times
			mEnvSounds[i].fadeTime = time >= 0.0 ? time : 1.0f;
			mEnvSounds[i].fadeAccTime = mEnvSounds[i].fadeTime;
			// Save current volume
			alGetSourcef(src, AL_GAIN, &mEnvSounds[i].volume);
			// Override individual fade
			mEnvSounds[i].fadeStatus = FADING_NONE;
			mEnvSounds[i].globalState = pause ? SSplayback::SS_FADING_OUT_AND_PAUSE
											  : SSplayback::SS_FADING_OUT;
		}
	}
	for (int i=0 ; i < mActiveSounds.size() ; i++) {
		if (mActiveSounds[i].globalState == SSplayback::SS_PLAYING) {
			ALSource src = mActiveSounds[i].sSRC->mSource;
			// Set times
			mActiveSounds[i].fadeTime = time >= 0.0 ? time : 1.0f;
			mActiveSounds[i].fadeAccTime = mActiveSounds[i].fadeTime;
			// Save current volume
			alGetSourcef(src, AL_GAIN, &mActiveSounds[i].volume);
			// Override individual fade
			mActiveSounds[i].fadeStatus = FADING_NONE;
			mActiveSounds[i].globalState = pause ? SSplayback::SS_FADING_OUT_AND_PAUSE
												 : SSplayback::SS_FADING_OUT;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::globalFadeIn(const Ogre::Real& time)
{
	/** TODO: globalFadeIn */
	debugRED("TODO: globalFadeIn%s", "\n");
}



/******************************************************************************/
/******************************************************************************/
/**********************    ENVIRONMENTAL SOUNDS    ****************************/


////////////////////////////////////////////////////////////////////////////////
bool
SoundManager::isPlayingEnvSound(const Ogre::String& sName)
{
	/* Check whether "sName" is a playing environmental sound. */
	for (int i=mEnvSounds.size()-1 ; i>=0 ; i--) {
		if (mEnvSounds[i].name == sName) {
			return mEnvSounds[i].source->isPlaying();
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
bool
SoundManager::isActiveEnvSound(const Ogre::String& sName)
{
	/* Check whether "sName" is an active environmental sound. */
	for (int i=mEnvSounds.size()-1 ; i>=0 ; i--) {
		if (mEnvSounds[i].name == sName) {
			return mEnvSounds[i].source->isActive();
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
bool
SoundManager::getEnvSoundRepeat(const Ogre::String& sName)
{
	/* If the environmental sound "sName" exists, return its repeat value. */
	for (int i=mEnvSounds.size()-1 ; i>=0 ; i--) {
		if (mEnvSounds[i].name == sName) {
			return mEnvSounds[i].source->getRepeat();
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::playEnvSound(const Ogre::String& sName, bool repeat)
{
	SoundBuffer* buf(0);
	SoundSource* src(0);
	Ogre::Vector3 origin(0.0f, 0.0f, 0.0f);
	SSerror err(SSerror::SS_INTERNAL_ERROR);
	HashStrBuff::const_iterator it(mLoadedBuffers.end());

	/* Check whether "sName" is an active environmental sound. */
	for (int i=mEnvSounds.size()-1 ; i>=0 ; i--) {
		if (mEnvSounds[i].name == sName) {
			if (mEnvSounds[i].source->isPlaying()) {
				/* Playing: do nothing. */
				return SSerror::SS_NO_ERROR;
			} else {
				/* Paused: start playback again. */
				ASSERT(mEnvSounds[i].source->isActive());
				err =  mEnvSounds[i].source->play(NULL, origin);
				if (err == SSerror::SS_NO_ERROR) {
					mEnvSounds[i].globalState = SSplayback::SS_PLAYING;
				}
				return err;
			}
		}
	}

	/* Lookup buffer */
	it = mLoadedBuffers.find(sName);
	if (it == mLoadedBuffers.end()) {
		debug("Sound %s wasn't loaded.\n", sName.c_str());
		return SSerror::SS_FILE_NOT_FOUND;
	} else {
		ASSERT(it->second);
		buf = it->second;
	}

	/* Get free source */
	if (buf->type == SSbuftype::SS_BUF_LOADED) {
		if (mAvailableLSS.empty()) {
			debug("No free LSources to play sound %s.\n", sName.c_str());
			return SSerror::SS_NO_SOURCES;
		} else {
			src = mAvailableLSS.front();
			mAvailableLSS.pop_front();
		}
	} else { /* buffer->type == SSbuftype::SS_BUF_STREAM */
		if (mAvailableSSS.empty()) {
			debug("No free LSources to play sound %s.\n", sName.c_str());
			return SSerror::SS_NO_SOURCES;
		} else {
			src = mAvailableSSS.front();
			mAvailableSSS.pop_front();
		}
	}

	alSourcef(src->mSource, AL_GAIN, 0.03f);
	debug("\nStarting environmental sound \"%s\"\n", sName.c_str());

	/* Start playback and register sound. */
	alSourcei (src->mSource, AL_SOURCE_RELATIVE, AL_TRUE);
	err = src->play(buf, origin, repeat);
	if (err == SSerror::SS_NO_ERROR) {
		/* Success */
		mEnvSounds.push_back(EnvSound(sName, src, SSplayback::SS_PLAYING));

	} else {
		/* Failure */
		debug("\nFailed starting sound \"%s\" playback.", sName.c_str());
		alSourcei(src->mSource, AL_SOURCE_RELATIVE, AL_FALSE);
		if (src->getType() == SSsrctype::SS_SRC_LOADED) {
			mAvailableLSS.push_back((LSoundSource*)src);
		} else {
			ASSERT(src->getType() == SSsrctype::SS_SRC_STREAM);
			mAvailableSSS.push_back((SSoundSource*)src);
		}
	}

	return err;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::stopEnvSound(const Ogre::String& sName)
{
	int pos(0);
	SoundSource* src(0);

	/* Check whether "sName" is an active environmental sound. */
	for (pos=mEnvSounds.size()-1 ; pos >= 0 ; pos--) {
		if (mEnvSounds[pos].name == sName) {
			src = mEnvSounds[pos].source;
			break;
		}
	}

	if (src) {
		/* Stop and free source. */
		src->stop();
		alSourcei(src->mSource, AL_SOURCE_RELATIVE, AL_FALSE);
		if (src->getType() == SSsrctype::SS_SRC_LOADED) {
			/* LSoundSource */
			ASSERT(dynamic_cast<LSoundSource*>(src) != NULL);
			mAvailableLSS.push_back((LSoundSource*) src);
		} else { /* getType() == SS_SRC_STREAM */
			/* SSoundSource */
			ASSERT(dynamic_cast<SSoundSource*>(src) != NULL);
			mAvailableSSS.push_back((SSoundSource*) src);
		}
		/* Desregister environmental sound. */
		mEnvSounds[pos] = mEnvSounds[mEnvSounds.size()-1];
		mEnvSounds.pop_back();
	}
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::restartEnvSound(const Ogre::String& sName)
{
	SSerror err = SSerror::SS_NO_BUFFER;
	SoundSource* src(0);

	/* Check whether "sName" is an active environmental sound. */
	for (int i=mEnvSounds.size()-1 ; i>=0 ; i--) {
		if (mEnvSounds[i].name == sName) {
			src = mEnvSounds[i].source;
			err = src->restart();
			if (err == SSerror::SS_NO_BUFFER) {
				mEnvSounds[i].globalState = SSplayback::SS_PLAYING;
			} else {
				/* Failure */
				src->stop();
				debug("\nFailed re-starting sound \"%s\" playback.", sName.c_str());
				alSourcei(src->mSource, AL_SOURCE_RELATIVE, AL_FALSE);
				if (src->getType() == SSsrctype::SS_SRC_LOADED) {
					mAvailableLSS.push_back((LSoundSource*)src);
				} else {
					ASSERT(src->getType() == SSsrctype::SS_SRC_STREAM);
					mAvailableSSS.push_back((SSoundSource*)src);
				}
				mEnvSounds[i] = mEnvSounds[mEnvSounds.size()-1];
				mEnvSounds.pop_back();
			}

			break;
		}
	}

	return err;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::fadeOutEnvSound(const Ogre::String& sName, const Ogre::Real& time,
							  const bool pause)
{
	/** TODO: fadeOutEnvSound */
	debugRED("TODO: fadeOutEnvSound%s", "\n");
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::fadeInEnvSound(const Ogre::String& sName, const Ogre::Real& time)
{
	/** TODO: fadeInEnvSound */
	debugRED("TODO: fadeInEnvSound%s", "\n");
}



/******************************************************************************/
/******************************************************************************/
/***********************    UNITS' APIS SOUNDS    *****************************/


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::playSound(SoundAPI& sAPI,
						const Ogre::String& sName,
						bool repeat)
{
	SSerror err = SSerror::SS_NO_ERROR;
	SoundBuffer* buffer = NULL;
	SoundSource* source = NULL;
	HashStrBuff::iterator it;

	/* Playing something already? */
	if (findPlayingAPI(sAPI)) {
		/* Playing: do nothing. */
		debug("Won't play \"%s\", sAPI was active.\n", sName.c_str());
		return err;

	} else if (findActiveAPI(sAPI)) {
		/* Paused: start playback again. */
		source = mActiveSounds[sAPI.mActivationIndex].sSRC;
		err = source->play(NULL, sAPI.getPosition(), source->getRepeat());
		if (err == SSerror::SS_NO_ERROR) {
			mActiveSounds[sAPI.mActivationIndex].globalState = SSplayback::SS_PLAYING;
		}
		return err;

	} else {
		/* New sound. */
		it = mLoadedBuffers.find(sName);
	}

	/* Lookup buffer */
	if (it == mLoadedBuffers.end()) {
		debug("Sound %s wasn't loaded.\n", sName.c_str());
		return SSerror::SS_FILE_NOT_FOUND;
	} else {
		ASSERT(it->second);
		buffer=it->second;
	}

	/* Get free source */
	if (buffer->type == SSbuftype::SS_BUF_LOADED) {
		if (mAvailableLSS.empty()) {
			debug("No free LSources to play sound %s.\n", sName.c_str());
			return SSerror::SS_NO_SOURCES;
		} else {
			source = mAvailableLSS.front();
			mAvailableLSS.pop_front();
		}
	} else { /* buffer->type == SSbuftype::SS_BUF_STREAM */
		if (mAvailableSSS.empty()) {
			debug("No free LSources to play sound %s.\n", sName.c_str());
			return SSerror::SS_NO_SOURCES;
		} else {
			source = mAvailableSSS.front();
			mAvailableSSS.pop_front();
		}
	}
#ifdef DEBUG
	{
		const Ogre::Vector3 pos(sAPI.getPosition());
		debugBLUE("Comenzando reproducción de \"%s\"\n", sName.c_str());
		fflush(stderr);
		alSourcef(source->mSource, AL_GAIN, 1.0f);
		alSourcef(source->mSource, AL_MAX_DISTANCE, 23.5f);
	}
#endif

	/* Register sound and start playback. */
	err = source->play(buffer, sAPI.getPosition(), repeat);
	if (err == SSerror::SS_NO_ERROR) {
		/* Success */
		sAPI.mActivationIndex = mActiveSounds.size();
		mActiveSounds.push_back(ActiveSound(&sAPI, source, SSplayback::SS_PLAYING));

	} else {
		/* Failure */
		debug("\nFailed starting sound \"%s\" playback.", sName.c_str());
		if (source->getType() == SSsrctype::SS_SRC_LOADED) {
			mAvailableLSS.push_back((LSoundSource*)source);
		} else {
			ASSERT(source->getType() == SSsrctype::SS_SRC_STREAM);
			mAvailableSSS.push_back((SSoundSource*)source);
		}
	}

	return err;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::stopSound(SoundAPI& sAPI)
{
	if (findActiveAPI(sAPI)) {
		ActiveSound* as = &mActiveSounds[sAPI.mActivationIndex];

		/* Stop and free source. */
		as->sSRC->stop();
		if (as->sSRC->getType() == SSsrctype::SS_SRC_LOADED) {
			/* LSoundSource */
			ASSERT(dynamic_cast<LSoundSource*>(as->sSRC) != NULL);
			mAvailableLSS.push_back((LSoundSource*) as->sSRC);
		} else { /* getType() == SS_SRC_STREAM */
			/* SSoundSource */
			ASSERT(dynamic_cast<SSoundSource*>(as->sSRC) != NULL);
			mAvailableSSS.push_back((SSoundSource*) as->sSRC);
		}

		/* Detach SoundAPI from active sounds vector. */
		mActiveSounds[sAPI.mActivationIndex] = mActiveSounds[mActiveSounds.size()-1];
		mActiveSounds[sAPI.mActivationIndex].sAPI->mActivationIndex = sAPI.mActivationIndex;
		sAPI.mActivationIndex = -1;
		mActiveSounds.pop_back();
	}
}



////////////////////////////////////////////////////////////////////////////////
SSerror
SoundManager::restartSound(SoundAPI& sAPI)
{
	SSerror err = SSerror::SS_NO_BUFFER;

	if (findActiveAPI(sAPI)) {
		uint idx = sAPI.mActivationIndex;
		SoundSource* source(mActiveSounds[idx].sSRC);

		err = source->restart(sAPI.getPosition());

		if (err == SSerror::SS_NO_ERROR) {
			/* Success */
			mActiveSounds[idx].globalState = SSplayback::SS_PLAYING;
		} else {
			/* Failure */
			source->stop();
			debug("\nFailed re-starting sound \"%s\" playback.",
					sAPI.getCurrentSound().c_str());
			if (source->getType() == SSsrctype::SS_SRC_LOADED) {
				mAvailableLSS.push_back((LSoundSource*)source);
			} else {
				ASSERT(source->getType() == SSsrctype::SS_SRC_STREAM);
				mAvailableSSS.push_back((SSoundSource*)source);
			}
			mActiveSounds[idx] = mActiveSounds[mActiveSounds.size()-1];
			mActiveSounds[idx].sAPI->mActivationIndex = idx;
			sAPI.mActivationIndex = -1;
			mActiveSounds.pop_back();
		}
	}

	return err;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::fadeOutSound(SoundAPI& sAPI, const Ogre::Real& time, const bool pause)
{
	/** TODO: fadeOutSound */
	debugRED("TODO: fadeOutSound%s", "\n");
}


////////////////////////////////////////////////////////////////////////////////
void
SoundManager::fadeInSound(SoundAPI& sAPI, const Ogre::Real& time)
{
	/** TODO: fadeInSound */
	debugRED("TODO: fadeInSound%s", "\n");
}

