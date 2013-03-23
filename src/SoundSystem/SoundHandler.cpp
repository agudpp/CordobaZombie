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
	PLAYLIST_STOPPED = (1ul<<0),
	PLAYLIST_PLAYING = (1ul<<1),
	PLAYLIST_PAUSED  = (1ul<<2),
	PLAYLIST_REPEAT  = (1ul<<3),
	PLAYLIST_RANDOM_ORDER	= (1ul<<4),
	PLAYLIST_RANDOM_SILENCE	= (1ul<<5),
	PLAYLIST_COUNTING_TIME	= (1ul<<6),
	PLAYLIST_FADING_OUT		= (1ul<<7)
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
	for (uint i=0 ; i < list.size() ; i++) { mPlayOrder[i] = i; }
	// Set internal state according to calling parameters
	mState = PLAYLIST_STOPPED
			| (repeat ? PLAYLIST_REPEAT : 0)
			| (randomOrder ? PLAYLIST_RANDOM_ORDER : 0)
			| (randomSilence ? PLAYLIST_RANDOM_SILENCE : 0);
	mState &= ~PLAYLIST_COUNTING_TIME;
}


////////////////////////////////////////////////////////////////////////////////
SoundHandler::Playlist::Playlist(const Playlist& pl) :
		mName(pl.mName),
		mList(pl.mList),
		mPlayOrder(pl.mPlayOrder),
		mCurrent(pl.mCurrent),
		mState(pl.mState),
		mSilence(pl.mSilence),
		mTimeSinceFinish(0.0f)
{
	// Default cpytor suffices.
}


////////////////////////////////////////////////////////////////////////////////
SoundHandler::Playlist&
SoundHandler::Playlist::operator=(const Playlist& pl)
{
	if (this == &pl) return *this;

	mName = pl.mName;

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
	mCurrent = pl.mCurrent;
	mState = pl.mState;
	mSilence = pl.mSilence;
	mTimeSinceFinish = pl.mTimeSinceFinish;

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
		deletePlaylist(mPlaylists[i]->mName);
	}
	debugRED("SoundHandler shutting down.\n");
	return;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::update()
{
	Playlist *pl(0);
	SSerror err(SSerror::SS_NO_ERROR);

	// Update all internal buffers.
	sSoundManager.update(&mFinishedPlaylists, &mPausedPlaylists);
	float elapsed = GLOBAL_TIME_FRAME;

	// Attend playlists which finished a sound.
	for (uint i=0 ; i < mFinishedPlaylists.size() ; i++) {
		if (!dynamic_cast<Playlist *>((Playlist*)mFinishedPlaylists[i])) {
			// Not one of our sounds
			continue;
		}
		pl = reinterpret_cast<Playlist*>(mFinishedPlaylists[i]);
		setPlaylistState(pl, PLAYLIST_STOPPED | PLAYLIST_COUNTING_TIME);
		unsetPlaylistState(pl, PLAYLIST_PLAYING | PLAYLIST_PAUSED);
		// Trick for resetting the field to 0 in the final loop.
		pl->mTimeSinceFinish = -elapsed;
	}
	mFinishedPlaylists.clear();

	// Update internal state of paused playlists
	for (uint i=0 ; i < mPausedPlaylists.size() ; i++) {
		if (!dynamic_cast<Playlist *>((Playlist*)mPausedPlaylists[i])) {
			// Not one of our sounds
			continue;
		}
		pl = reinterpret_cast<Playlist*>(mPausedPlaylists[i]);
		if (pl->mState & PLAYLIST_PAUSED) continue;  // Already updated
		setPlaylistState(pl, PLAYLIST_PAUSED);
		unsetPlaylistState(pl, PLAYLIST_PLAYING
							 | PLAYLIST_STOPPED
							 | PLAYLIST_COUNTING_TIME);
	}
	mPausedPlaylists.clear();

	// Check for playlists which must start a new sound.
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		pl = mPlaylists[i];
		if (( pl->mState & PLAYLIST_STOPPED) &&
			( pl->mState & PLAYLIST_COUNTING_TIME) &&
			!(pl->mState & PLAYLIST_FADING_OUT)) {
			pl->mTimeSinceFinish += elapsed;
			if (pl->mTimeSinceFinish >= pl->mSilence) {
				// Move one track forward
				++pl->mCurrent %= pl->mList.size();  // =D
				err = startPlaylist("", pl);
				ASSERT(err == SSerror::SS_NO_ERROR);
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::globalPause()
{
	// First let the manager update the sound sources
	sSoundManager.globalPause();
	// Now check the results
	for (uint i = 0 ; i < mPlaylists.size() ; i++) {
		Playlist *pl = mPlaylists[i];
		ASSERT(pl);
		if (pl->mList.empty()) continue;
		Ogre::String sound = pl->mList[pl->mPlayOrder[pl->mCurrent]];
		if (sSoundManager.isActiveEnvSound(sound) &&
			!sSoundManager.isPlayingEnvSound(sound)) {
			setPlaylistState(pl, PLAYLIST_PAUSED);
			unsetPlaylistState(pl, PLAYLIST_PLAYING
								 | PLAYLIST_STOPPED
								 | PLAYLIST_COUNTING_TIME);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::globalPlay()
{
	// First let the manager update the sound sources
	sSoundManager.globalPlay();
	// Now check the results
	for (uint i = 0 ; i < mPlaylists.size() ; i++) {
		Playlist *pl = mPlaylists[i];
		ASSERT(pl);
		if (pl->mList.empty()) continue;
		Ogre::String sound = pl->mList[pl->mPlayOrder[pl->mCurrent]];
		if (sSoundManager.isPlayingEnvSound(sound)) {
			// The source started playback, reflect that in state.
			setPlaylistState(pl, PLAYLIST_PLAYING
							   | PLAYLIST_COUNTING_TIME);
			unsetPlaylistState(pl, PLAYLIST_PAUSED
								 | PLAYLIST_STOPPED);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::globalStop()
{
	// First let the manager update the sound sources
	sSoundManager.globalStop();
	// Now check the results
	for (uint i = 0 ; i < mPlaylists.size() ; i++) {
		Playlist *pl = mPlaylists[i];
		ASSERT(pl);
		if (pl->mList.empty()) continue;
		Ogre::String sound = pl->mList[pl->mPlayOrder[pl->mCurrent]];
		ASSERT(!sSoundManager.isActiveEnvSound(sound));
		setPlaylistState(pl, PLAYLIST_STOPPED);
		unsetPlaylistState(pl, PLAYLIST_PLAYING
							 | PLAYLIST_PAUSED
							 | PLAYLIST_COUNTING_TIME);
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::globalRestart()
{
	// First let the manager update the sound sources
	sSoundManager.globalRestart();
	// Now check the results
	for (uint i = 0 ; i < mPlaylists.size() ; i++) {
		Playlist *pl = mPlaylists[i];
		ASSERT(pl);
		if (pl->mList.empty()) continue;
		Ogre::String sound = pl->mList[pl->mPlayOrder[pl->mCurrent]];
		if (sSoundManager.isPlayingEnvSound(sound)) {
			// The source restarted playback, reflect that in state.
			setPlaylistState(pl, PLAYLIST_PLAYING
							   | PLAYLIST_COUNTING_TIME);
			unsetPlaylistState(pl, PLAYLIST_PAUSED
								 | PLAYLIST_STOPPED);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::globalFadeOut(const Ogre::Real& time, const bool pause)
{
	// First let the manager update the sound sources
	sSoundManager.globalFadeOut(time, pause);
	// Now avoid future wake ups
	for (uint i = 0 ; i < mPlaylists.size() ; i++) {
		ASSERT(mPlaylists[i]);
		if (mPlaylists[i]->mList.empty()) continue;
		setPlaylistState(mPlaylists[i], PLAYLIST_FADING_OUT);
		unsetPlaylistState(mPlaylists[i], PLAYLIST_COUNTING_TIME);
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::globalFadeIn(const Ogre::Real& time)
{
	// First let the manager update the sound sources
	sSoundManager.globalFadeIn(time);
	// Now check the results
	for (uint i = 0 ; i < mPlaylists.size() ; i++) {
		Playlist *pl = mPlaylists[i];
		ASSERT(pl);
		if (pl->mList.empty()) continue;
		Ogre::String sound = pl->mList[pl->mPlayOrder[pl->mCurrent]];
		if (sSoundManager.isPlayingEnvSound(sound)) {
			// The source restarted playback, reflect that in state.
			setPlaylistState(pl, PLAYLIST_PLAYING
							   | PLAYLIST_COUNTING_TIME);
			unsetPlaylistState(pl, PLAYLIST_PAUSED
								 | PLAYLIST_STOPPED);
		} else if (pl->mState & (PLAYLIST_FADING_OUT)) {
			// Paused during silence, start counting time again
			setPlaylistState(pl, PLAYLIST_COUNTING_TIME);
			unsetPlaylistState(pl, PLAYLIST_FADING_OUT);
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

	mPlaylists.push_back(new Playlist(name,
									  list,
									  repeat,
									  random,
									  (silence<0.0f),
									  silence));
	return fails;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::deletePlaylist(const Ogre::String& name)
{
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i]->mName == name) {
			// Playlist found: stop any active playback.
			Playlist* pl = mPlaylists[i];
			Ogre::String sound("");
			if (pl->mList.empty()) {
				goto deletion;
			} else {
				sound = pl->mList[pl->mPlayOrder[pl->mCurrent]];
			}
			if (sSoundManager.isActiveEnvSound(sound)) {
				SSerror err = sSoundManager.stopEnvSound(sound);
				ASSERT(err == SSerror::SS_NO_ERROR);
			}
			deletion:
			// Delete playlist.
			mPlaylists[i] = mPlaylists[mPlaylists.size()-1];
			mPlaylists.pop_back();
			delete pl;
			return;
		}
	}
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
		if (mPlaylists[i]->mName == name) {
			if (repeat) {
				setPlaylistState(mPlaylists[i], PLAYLIST_REPEAT);
			} else {
				unsetPlaylistState(mPlaylists[i], PLAYLIST_REPEAT);
			}
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
		if (mPlaylists[i]->mName == name) {
			if (found) { *found = true; };
			return mPlaylists[i]->mState & PLAYLIST_REPEAT;
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
		if (mPlaylists[i]->mName == name) {
			if (random) {
				setPlaylistState(mPlaylists[i], PLAYLIST_RANDOM_ORDER);
			} else {
				unsetPlaylistState(mPlaylists[i], PLAYLIST_RANDOM_ORDER);
			}
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
		if (mPlaylists[i]->mName == name) {
			if (found) { *found = true; };
			return mPlaylists[i]->mState & PLAYLIST_RANDOM_ORDER;
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
		if (mPlaylists[i]->mName == name) {
			if (random) {
				setPlaylistState(mPlaylists[i], PLAYLIST_RANDOM_SILENCE);
			} else {
				unsetPlaylistState(mPlaylists[i], PLAYLIST_RANDOM_SILENCE);
			}
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
		if (mPlaylists[i]->mName == name) {
			if (found) { *found = true; };
			return mPlaylists[i]->mState & PLAYLIST_RANDOM_SILENCE;
		}
	}
	if (found) { *found = false; };
	return false;
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::startPlaylist(const Ogre::String& name, Playlist *plp)
{
	SSerror err(SS_NO_ERROR);
	Playlist *pl = plp ? plp : getPlaylist(name);

	if (!pl) {
		return SSerror::SS_FILE_NOT_FOUND;
	} else if (pl->mState & PLAYLIST_PLAYING) {
		// Playlist is already playing: nothig to do here.
		return SSerror::SS_NO_ERROR;
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
									 false,  // NEVER repeat individual sounds
									 (void *)pl);

	if (err == SSerror::SS_NO_ERROR) {
		// Success
		setPlaylistState(pl, PLAYLIST_PLAYING | PLAYLIST_COUNTING_TIME);
		unsetPlaylistState(pl, PLAYLIST_PAUSED | PLAYLIST_STOPPED);
		return err;
	} else {
		// Failure
		debugWARNING("Playlist \"%s\" failed to start: %s", pl->mName.c_str(),
					errorMessage(err));
		setPlaylistState(pl, PLAYLIST_STOPPED);
		unsetPlaylistState(pl, PLAYLIST_PLAYING
							 | PLAYLIST_PAUSED
							 | PLAYLIST_COUNTING_TIME);
		return SSerror::SS_INTERNAL_ERROR;
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::pausePlaylist(const Ogre::String& name)
{
	const Ogre::String *sound(0);
	Playlist *pl = getPlaylist(name);

	if (pl) {
		sound = &pl->mList[pl->mPlayOrder[pl->mCurrent]];
		if (sSoundManager.isPlayingEnvSound(*sound)) {
			sSoundManager.pauseEnvSound(*sound);
		}
		setPlaylistState(pl, PLAYLIST_PAUSED);
		unsetPlaylistState(pl, PLAYLIST_PLAYING
							 | PLAYLIST_STOPPED
							 | PLAYLIST_COUNTING_TIME);
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::stopPlaylist(const Ogre::String& name)
{
	SSerror err(SSerror::SS_NO_ERROR);
	const Ogre::String *sound(0);
	Playlist *pl = getPlaylist(name);

	if (pl) {
		sound = &pl->mList[pl->mPlayOrder[pl->mCurrent]];
		if (sSoundManager.isActiveEnvSound(*sound)) {
			err = sSoundManager.stopEnvSound(*sound);
		}
		if (err == SSerror::SS_NO_ERROR) {
			// Success
			pl->mCurrent = 0;
			pl->mTimeSinceFinish = 0.0f;
			setPlaylistState(pl, PLAYLIST_STOPPED);
			unsetPlaylistState(pl, PLAYLIST_PLAYING
								 | PLAYLIST_PAUSED
								 | PLAYLIST_COUNTING_TIME
								 | PLAYLIST_FADING_OUT);
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
	stopPlaylist(name);
	return startPlaylist(name);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::fadeOutPlaylist(const Ogre::String& name,
							  const Ogre::Real& time,
							  const bool pause)
{
	SSerror err(SSerror::SS_NO_ERROR);
	Playlist *pl = getPlaylist(name);

	if (!pl) {
		return SSerror::SS_FILE_NOT_FOUND;
	} else if (pl->mState & PLAYLIST_PLAYING) {
		err = sSoundManager.fadeOutEnvSound(
						pl->mList[pl->mPlayOrder[pl->mCurrent]],
						time,
						pause);
	}
	setPlaylistState(pl, PLAYLIST_FADING_OUT);
	unsetPlaylistState(pl, PLAYLIST_COUNTING_TIME);

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
	const Ogre::String *sound(0);
	Playlist *pl = getPlaylist(name);

	if (!pl) {
		return SSerror::SS_FILE_NOT_FOUND;
	} else if (pl->mList.empty()) {
		return SSerror::SS_NO_ERROR;
	} else {
		sound = &pl->mList[pl->mPlayOrder[pl->mCurrent]];
	}

	SSerror err = sSoundManager.fadeInEnvSound(*sound, time);
	if (err != SSerror::SS_NO_ERROR) {
		// Failure
		debugERROR("Playlist \"%s\" fade-in failed: %s", pl->mName.c_str(),
					errorMessage(err));
		unsetPlaylistState(pl, PLAYLIST_PLAYING
							 | PLAYLIST_PAUSED
							 | PLAYLIST_STOPPED);
		return SSerror::SS_INTERNAL_ERROR;
	}

	if (sSoundManager.isPlayingEnvSound(*sound)) {
		// Playback was restarted by SoundManager.
		setPlaylistState(pl, PLAYLIST_PLAYING
						   | PLAYLIST_COUNTING_TIME);
		unsetPlaylistState(pl, PLAYLIST_PAUSED
							 | PLAYLIST_STOPPED
							 | PLAYLIST_FADING_OUT);
	} else if (pl->mState & (PLAYLIST_FADING_OUT)) {
		// Playlist was paused during silence, start counting time again.
		setPlaylistState(pl, PLAYLIST_COUNTING_TIME);
		unsetPlaylistState(pl, PLAYLIST_FADING_OUT);
	}

	return err;
}

