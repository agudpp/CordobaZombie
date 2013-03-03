/*
 * SoundHandler.cpp
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

#include <vector>
#include <string>
#include <strings.h>  // strcasecmp

#include <Common/GlobalObjects/GlobalObjects.h>

#include "SoundHandler.h"
#include "SoundEnums.h"



// SoundHandler's static member initialization.
SoundManager& SoundHandler::sSoundManager = SoundManager::getInstance();


/******************************************************************************/
/*************************    AUXILIARY FUNCTIONS    **************************/
namespace {

inline const char*
errorMessage(SSerror err)
{
	switch (err) {
	case SSerror::SS_NO_ERROR:
		return "operation successfull.\n";
		break;
	case SSerror::SS_NO_MEMORY:
		return "insufficient system memory.\n";
		break;
	case SSerror::SS_NO_SOURCES:
		return "no available sources to start playback.\n";
		break;
	case SSerror::SS_INVALID_FILE:
		return "unsupported/bad audio format.\n";
		break;
	case SSerror::SS_FILE_NOT_FOUND:
		return "sound not loaded in the system.\n";
		break;
	case SSerror::SS_INTERNAL_ERROR:
		return "unspecified internal error.\n";
		break;
	default:
		return "WTF? Unspecified BIG FAT internal error (???)\n";
		break;
	}
	return "";
}

// Playlist states
typedef enum {
	PLAYLIST_STOPPED = 1,
	PLAYLIST_PLAYING = 2,
	PLAYLIST_PAUSED  = 4,
	PLAYLIST_REPEAT  = 8,
	PLAYLIST_RANDOM_ORDER	= 16,
	PLAYLIST_RANDOM_SILENCE	= 32
} _playlists_state;

}


////////////////////////////////////////////////////////////////////////////////
SoundHandler::Playlist::Playlist(const Ogre::String& name) :
	mName(name),
	mCurrent(0),
	mState(PLAYLIST_STOPPED | PLAYLIST_REPEAT),
	mSilence(0.0f),
	mTimeSinceFinish(0.0f)
{}


////////////////////////////////////////////////////////////////////////////////
SoundHandler::Playlist::Playlist(const Ogre::String& name,
								 const std::vector<Ogre::String>& list,
								 bool  repeat,
								 bool  randomOrder,
								 bool  randomSilence,
								 float silence) :
	mName(name),
	mList(list),
	mCurrent(0),
	mSilence(silence),
	mTimeSinceFinish(0.0f)
{
	// Initialize play order (if necessary, will be shuffled on play time)
	mPlayOrder.resize(list.size());
	for (uint i=0 ; i < list.size() ; i++) { mPlayOrder.push_back(i); }
	// Set internal state according to calling parameters
	mState = PLAYLIST_STOPPED
			| (repeat ? PLAYLIST_REPEAT : 0)
			| (randomOrder ? PLAYLIST_RANDOM_ORDER : 0)
			| (randomSilence ? PLAYLIST_RANDOM_SILENCE : 0);
}


////////////////////////////////////////////////////////////////////////////////
SoundHandler::Playlist::Playlist(const Playlist& pl) :
		mName(pl.mName),
		mList(pl.mList),
		mPlayOrder(pl.mPlayOrder),
		mSilence(pl.mSilence),
		mTimeSinceFinish(0.0f)
{
	// We copy the shuffle and repeat state, but not the playing state.
	// We start stopped, and in the first sound of the list.
	mState = (pl.mState & (~(PLAYLIST_PLAYING | PLAYLIST_PAUSED))) | PLAYLIST_STOPPED;
	mCurrent = 0;
}


////////////////////////////////////////////////////////////////////////////////
SoundHandler::Playlist&
SoundHandler::Playlist::operator=(const Playlist& pl)
{
	if (this == &pl) return *this;

	mName = pl.mName;
	mSilence = pl.mSilence;

	mList.resize(pl.mList.size());
	for (uint i=0 ; i < pl.mList.size() ; i++) {
		mList[i] = pl.mList[i];
	}

	mPlayOrder.resize(pl.mPlayOrder.size());
	for (uint i=0 ; i < pl.mPlayOrder.size() ; i++) {
		mPlayOrder[i] = pl.mPlayOrder[i];
	}

	// We copy the shuffle and repeat state, but not the playing state.
	// We start stopped, and in the first sound of the list.
	mState = (pl.mState & (~(PLAYLIST_PLAYING | PLAYLIST_PAUSED))) | PLAYLIST_STOPPED;
	mCurrent = 0;
	mTimeSinceFinish = 0.0f;

	return *this;
}


////////////////////////////////////////////////////////////////////////////////
SoundHandler::Playlist::~Playlist(void)
{
	if (mState & PLAYLIST_PLAYING) {
		if (sSoundManager.isActiveEnvSound(mList[mPlayOrder[mCurrent]])) {
			sSoundManager.stopEnvSound(mList[mPlayOrder[mCurrent]]);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
Ogre::String
SoundHandler::loadSounds(const std::vector<Ogre::String>& list, SSbuftype bufType)
{
	SSerror err(SSerror::SS_NO_ERROR);
	Ogre::String fails("");

	for (uint i=0 ; i < list.size() ; i++) {
		// Check file's sound format, according to extension.
		uint pos = list[i].find_last_of('/') + 1;
		Ogre::String sName = list[i].substr(pos);
		pos = sName.find_last_of(".") + 1;

		if (0 == strcasecmp(sName.substr(pos).c_str(),"WAV")) {
			// Load the file.
			if (bufType == SSbuftype::SS_BUF_LOADED) {
				err = sSoundManager.loadSound(sName, SSformat::SS_WAV,
												SSbuftype::SS_BUF_LOADED);
			} else { // bufType == SSbuftype::SS_BUF_STREAM_WAV
				err = sSoundManager.loadSound(sName, SSformat::SS_WAV,
												SSbuftype::SS_BUF_STREAM_WAV);
			}
		} else if (0 == strcasecmp(sName.substr(pos).c_str(),"OGG")) {
			// Load the file
			if (bufType == SSbuftype::SS_BUF_LOADED) {
				err = sSoundManager.loadSound(sName, SSformat::SS_OGG,
												SSbuftype::SS_BUF_LOADED);
			} else { // bufType == SSbuftype::SS_BUF_STREAM_OGG
				err = sSoundManager.loadSound(sName, SSformat::SS_OGG,
												SSbuftype::SS_BUF_STREAM_OGG);
			}
		} else {
			err = SSerror::SS_INVALID_FILE;
		}

		if (err != SSerror::SS_NO_ERROR) {
			fails += "File: \"" + list[i] + "\" ; Error: " + errorMessage(err);
		}
	}

	return fails;
}


////////////////////////////////////////////////////////////////////////////////
Ogre::String
SoundHandler::unloadSounds(const std::vector<Ogre::String>& list)
{
	Ogre::String fails("");
	for (uint i=0 ; i < list.size() ; i++) {
		if (sSoundManager.isActiveEnvSound(list[i]) ||
			sSoundManager.isActiveAPISound(list[i])) {
			fails += "File: \"" + list[i] + "\" was in use.\n";
		} else {
			sSoundManager.unloadSound(list[i]);
		}
	}
	return fails;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::shutDown(void)
{
	globalStop();
	for (int i=0 ; i < mPlaylists.size() ; i++) {
		deletePlaylist(mPlaylists[i].mName);
	}
	debugRED("SoundHandler shutting down.\n");
	return;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::update()
{
	SSerror err(SSerror::SS_NO_ERROR);

	// Update all internal buffers.
	sSoundManager.update(&mFinishedPlaylists);
	float elapsed = GLOBAL_TIME_FRAME;

	// Attend playlists which finished a sound.
	for (uint i=0 ; i < mFinishedPlaylists.size() ; i++) {
		ASSERT(0 != dynamic_cast<Playlist *>((Playlist*)mFinishedPlaylists[i]));
		Playlist *pl = reinterpret_cast<Playlist*>(mFinishedPlaylists[i]);
		setPlaylistState(pl, PLAYLIST_STOPPED);
		unsetPlaylistState(pl, PLAYLIST_PLAYING | PLAYLIST_PAUSED);
		// Trick for setting the field to 0.0f in the next loop.
		pl->mTimeSinceFinish = -elapsed;
	}
	mFinishedPlaylists.clear();

	// Check for playlist which must start a new sound.
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i].mState & PLAYLIST_STOPPED) {
			mPlaylists[i].mTimeSinceFinish += elapsed;
		}
		if (mPlaylists[i].mTimeSinceFinish >= mPlaylists[i].mSilence) {
			++mPlaylists[i].mCurrent %= mPlaylists[i].mList.size();  // \m/
			err = startPlaylist("", &mPlaylists[i]);
			ASSERT(err == SSerror::SS_NO_ERROR);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
Ogre::String
SoundHandler::newPlaylist(const Ogre::String& name,
						  const std::vector<Ogre::String>& soundsList,
						  bool  repeat,
						  bool  random,
						  float silence)
{
	const char unloadedStr[] = ": sound not loaded in the system.\n";
	Ogre::String fails("");
	std::vector<Ogre::String> list;

	if (existsPlaylist(name)) {
		return ("Playlist \"" + name + "\" exists!\n");
	}

	// Check requested sounds are available.
	for (uint i=0 ; i < soundsList.size() ; i++) {
		if (sSoundManager.isSoundLoaded(soundsList[i])) {
			// Sound OK, register.
			list.push_back(soundsList[i]);
		} else {
			// Inexistent sound, report error.
			fails += soundsList[i] + unloadedStr;
			debugWARNING("File \"%s\"%s", soundsList[i].c_str(), unloadedStr);
		}
	}

	mPlaylists.push_back(Playlist(name, list, repeat, random, (silence<0.0f), silence));

	return fails;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::deletePlaylist(const Ogre::String& name)
{
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i].mName == name) {
			// Playlist found: stop any active playback.
			Playlist* pl = &mPlaylists[i];
			Ogre::String sound("");
			if (pl->mList.empty()) {
				goto deletion;
			} else {
				sound = pl->mList[pl->mPlayOrder[pl->mCurrent]];
			}
			if (sSoundManager.isActiveEnvSound(sound)) {
				SSerror err = sSoundManager.stopEnvSound(sound);
				ASSERT(err == SSerror::SS_NO_ERROR);
				unsetPlaylistState(pl, PLAYLIST_PLAYING);
				setPlaylistState(pl, PLAYLIST_STOPPED);
			}
			deletion:
			// Delete playlist.
			mPlaylists[i] = mPlaylists[mPlaylists.size()-1];
			mPlaylists.pop_back();
			return;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::startPlaylist(const Ogre::String& name, Playlist *plp)
{
	SSerror err(SS_NO_ERROR);
	Playlist *pl(0);
	if (plp) {
		pl = plp;
	} else {
		pl = getPlaylist(name);
	}

	if (!pl) {
		return SSerror::SS_FILE_NOT_FOUND;
	} else if (pl->mState & PLAYLIST_PLAYING) {
		// Playlist is already playing: nothig to do here.
		return SSerror::SS_NO_ERROR;
	} else if (pl->mState & PLAYLIST_STOPPED) {
		// Playlist had been stopped: restart from the begginning.
		pl->mCurrent = 0;
	}

	// If we're at the beginning and random order was chosen,
	// shuffle the play order.
	if ((pl->mCurrent == 0) && (pl->mState & PLAYLIST_RANDOM_ORDER)) {
		std::random_shuffle(pl->mPlayOrder.begin(), pl->mPlayOrder.end());
	}
	// If random silence was chosen, well, then randomize the fucking thing!
	if (pl->mState & PLAYLIST_RANDOM_SILENCE) {
		pl->mSilence = MIN_SILENCE +
					(std::rand()/((float)RAND_MAX))*(MAX_SILENCE-MIN_SILENCE);
	}
	// Start current sound as a (registered) Environmental Sound.
	err = sSoundManager.playEnvSound(pl->mList[pl->mPlayOrder[pl->mCurrent]],
									 DEFAULT_ENV_GAIN,
									 pl->mState & PLAYLIST_REPEAT,
									 (void *)pl);

	if (err == SSerror::SS_NO_ERROR) {
		// Success
		setPlaylistState(pl, PLAYLIST_PLAYING);
		unsetPlaylistState(pl, PLAYLIST_PAUSED | PLAYLIST_STOPPED);
		return err;
	} else {
		// Failure
		debugWARNING("Playlist \"%s\" failed to start: %s", pl->mName.c_str(),
					errorMessage(err));
		setPlaylistState(pl, PLAYLIST_STOPPED);
		unsetPlaylistState(pl, PLAYLIST_PLAYING | PLAYLIST_PAUSED);
		return SSerror::SS_INTERNAL_ERROR;
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::pausePlaylist(const Ogre::String& name)
{
	Playlist *pl = getPlaylist(name);
	if (pl) {
		sSoundManager.pauseEnvSound(pl->mList[pl->mPlayOrder[pl->mCurrent]]);
		setPlaylistState(pl, PLAYLIST_PAUSED);
		unsetPlaylistState(pl, PLAYLIST_PLAYING | PLAYLIST_STOPPED);
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::stopPlaylist(const Ogre::String& name)
{
	Playlist *pl = getPlaylist(name);
	if (pl) {
		SSerror err = sSoundManager.stopEnvSound(
						pl->mList[pl->mPlayOrder[pl->mCurrent]]);
		if (err == SS_NO_ERROR) {
			// Success
			pl->mTimeSinceFinish = 0.0f;
			setPlaylistState(pl, PLAYLIST_STOPPED);
			unsetPlaylistState(pl, PLAYLIST_PLAYING | PLAYLIST_PAUSED);
		} else {
			// Failure
			debugERROR("Playlist \"%s\" failed to stop: %s", pl->mName.c_str(),
						errorMessage(err));
			unsetPlaylistState(pl, PLAYLIST_PLAYING
								 | PLAYLIST_PAUSED
								 | PLAYLIST_STOPPED);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::restartPlaylist(const Ogre::String& name)
{
	Playlist *pl = getPlaylist(name);
	if (!pl) {
		return SSerror::SS_FILE_NOT_FOUND;
	}

	SSerror err = sSoundManager.restartEnvSound(
					pl->mList[pl->mPlayOrder[pl->mCurrent]]);

	if (err == SSerror::SS_NO_ERROR) {
		// Success
		setPlaylistState(pl, PLAYLIST_PLAYING);
		unsetPlaylistState(pl, PLAYLIST_STOPPED | PLAYLIST_PAUSED);
		return err;
	} else {
		// Failure
		debugERROR("Playlist \"%s\" failed to restart: %s", pl->mName.c_str(),
					errorMessage(err));
		unsetPlaylistState(pl, PLAYLIST_PLAYING
							 | PLAYLIST_PAUSED
							 | PLAYLIST_STOPPED);
		return SSerror::SS_INTERNAL_ERROR;
	}
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::fadeOutPlaylist(const Ogre::String& name,
							  const Ogre::Real& time,
							  const bool pause)
{
	Playlist *pl = getPlaylist(name);
	if (!pl) {
		return SSerror::SS_FILE_NOT_FOUND;
	}

	SSerror err = sSoundManager.fadeOutEnvSound(
					pl->mList[pl->mPlayOrder[pl->mCurrent]],
					time,
					pause);

	if (err != SSerror::SS_NO_ERROR) {
		// Failure
		debugERROR("Playlist \"%s\" fade-out failed: %s", pl->mName.c_str(),
					errorMessage(err));
		unsetPlaylistState(pl, PLAYLIST_PLAYING
							 | PLAYLIST_PAUSED
							 | PLAYLIST_STOPPED);
		return SSerror::SS_INTERNAL_ERROR;
	}

	return err;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::fadeInPlaylist(const Ogre::String& name,
							 const Ogre::Real& time)
{
	Playlist *pl = getPlaylist(name);
	if (!pl) {
		return SSerror::SS_FILE_NOT_FOUND;
	}

	SSerror err = sSoundManager.fadeInEnvSound(
					pl->mList[pl->mPlayOrder[pl->mCurrent]], time);

	if (err != SSerror::SS_NO_ERROR) {
		// Failure
		debugERROR("Playlist \"%s\" fade-in failed: %s", pl->mName.c_str(),
					errorMessage(err));
		unsetPlaylistState(pl, PLAYLIST_PLAYING
							 | PLAYLIST_PAUSED
							 | PLAYLIST_STOPPED);
		return SSerror::SS_INTERNAL_ERROR;
	}

	return err;
}


////////////////////////////////////////////////////////////////////////////////
SSplayback
SoundHandler::getPlaylistPlayState(const Ogre::String& name) const
{
	const Playlist *pl = getPlaylist(name);
	if (pl) {
		if (pl->mName == name) {
			if (pl->mState & PLAYLIST_PLAYING) {
				// Playing
				ASSERT(!(pl->mState & (PLAYLIST_PAUSED | PLAYLIST_STOPPED)));
				return SSplayback::SS_PLAYING;

			} else if (pl->mState & PLAYLIST_PAUSED) {
				// Paused
				ASSERT(!(pl->mState & (PLAYLIST_PLAYING | PLAYLIST_STOPPED)));
				return SSplayback::SS_PAUSED;

			} else if (pl->mState & PLAYLIST_STOPPED) {
				// Stopped
				ASSERT(!(pl->mState & (PLAYLIST_PLAYING | PLAYLIST_PAUSED)));
				return SSplayback::SS_FINISHED;

			} else {
				// WTF?
				debugERROR("Inconsistent playing state for playlist \"%s\"\n",
							pl->mName.c_str());
				return SSplayback::SS_NONE;
			}
		}
	}
	// Playlist not found
	return SSplayback::SS_NONE;
}


////////////////////////////////////////////////////////////////////////////////
bool
SoundHandler::setPlaylistRepeat(const Ogre::String& name, bool repeat)
{
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i].mName == name) {
			mPlaylists[i].mState |= repeat ? PLAYLIST_REPEAT : 0;
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
bool
SoundHandler::getPlaylistRepeat(const Ogre::String& name, bool* found) const
{
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i].mName == name) {
			if (found) { *found = true; };
			return mPlaylists[i].mState & PLAYLIST_REPEAT;
		}
	}
	if (found) { *found = false; };
	return false;
}



////////////////////////////////////////////////////////////////////////////////
bool
SoundHandler::setPlaylistRandomOrder(const Ogre::String& name, bool random)
{
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i].mName == name) {
			mPlaylists[i].mState |= random ? PLAYLIST_RANDOM_ORDER : 0;
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
bool
SoundHandler::getPlaylistRandomOrder(const Ogre::String& name, bool* found) const
{
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i].mName == name) {
			if (found) { *found = true; };
			return mPlaylists[i].mState & PLAYLIST_RANDOM_ORDER;
		}
	}
	if (found) { *found = false; };
	return false;
}


////////////////////////////////////////////////////////////////////////////////
bool
SoundHandler::setPlaylistRandomSilence(const Ogre::String& name, bool random)
{
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i].mName == name) {
			mPlaylists[i].mState |= random ? PLAYLIST_RANDOM_SILENCE : 0;
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
bool
SoundHandler::getPlaylistRandomSilence(const Ogre::String& name, bool* found) const
{
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i].mName == name) {
			if (found) { *found = true; };
			return mPlaylists[i].mState & PLAYLIST_RANDOM_SILENCE;
		}
	}
	if (found) { *found = false; };
	return false;
}
