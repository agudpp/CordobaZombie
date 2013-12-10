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

#include "SoundHandler.h"
#include "SoundEnums.h"


namespace mm {


// SoundHandler's static member initialization.
SoundManager& SoundHandler::sSoundManager = SoundManager::getInstance();


/******************************************************************************/
/************************    LOCAL AUXILIARY STUFF    *************************/
namespace {

// Playlist state bit-flags
typedef enum {
	// Playback state
	PLAYLIST_STOPPED 		= (1ul<<0),
	PLAYLIST_PLAYING 		= (1ul<<1),
	PLAYLIST_PAUSED  		= (1ul<<2),
	PLAYLIST_SILENCE		= (1ul<<3),
	PLAYLIST_GLOBAL_MODE	= (1ul<<4),
	// Playback settings
	PLAYLIST_REPEAT  		= (1ul<<5),
	PLAYLIST_RANDOM_ORDER	= (1ul<<6),
	PLAYLIST_RANDOM_SILENCE	= (1ul<<7)
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
	mCurrent(0u),
	mState(PLAYLIST_STOPPED),
	mSilence(silence),
	mTimeSinceFinish(0.0f)
{
	// Initialize play order (if necessary, will be shuffled on play time)
	mPlayOrder.resize(list.size());
	for (unsigned int i=0 ; i < list.size() ; i++) { mPlayOrder[i] = i; }
	// Set internal state according to calling parameters
	mState |= (repeat ? PLAYLIST_REPEAT : 0)
			| (randomOrder ? PLAYLIST_RANDOM_ORDER : 0)
			| (randomSilence ? PLAYLIST_RANDOM_SILENCE : 0);
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
	for (unsigned int i=0 ; i < pl.mList.size() ; i++) {
		mList[i] = pl.mList[i];
	}

	mPlayOrder.resize(pl.mPlayOrder.size());
	for (unsigned int i=0 ; i < pl.mPlayOrder.size() ; i++) {
		mPlayOrder[i] = pl.mPlayOrder[i];
	}

	// We copy the shuffle and repeat state, but not the playing state.
	// We start stopped, and in the first sound of the list.
	mCurrent = pl.mCurrent;
	mState   = pl.mState;
	mSilence = pl.mSilence;
	mTimeSinceFinish = pl.mTimeSinceFinish;

	return *this;
}


////////////////////////////////////////////////////////////////////////////////
SoundHandler::Playlist::~Playlist(void)
{
	if ((mState & PLAYLIST_PLAYING) && (mList.size() > 0)) {
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

	for (unsigned int i=0 ; i < list.size() ; i++) {
		// Check file's sound format, according to extension.
		unsigned int pos = list[i].find_last_of('/') + 1;
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
			fails += "  \""+ list[i] +"\", with error: "+ SSenumStr(err) +"\n";
		}
	}

	if (!fails.empty()) fails = "Some files failed to load:\n" + fails;
	return fails;
}


////////////////////////////////////////////////////////////////////////////////
Ogre::String
SoundHandler::unloadSounds(const std::vector<Ogre::String>& list)
{
	Ogre::String fails("");
	for (unsigned int i=0 ; i < list.size() ; i++) {
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
SoundHandler::update(const float globalTimeFrame)
{
	Playlist *pl(0);
	SSerror err(SSerror::SS_NO_ERROR);

	// Update all internal buffers.
	sSoundManager.update(globalTimeFrame, &mFinishedPlaylists, &mPausedPlaylists);

	// Attend playlists which finished a sound.
	for (unsigned int i=0 ; i < mFinishedPlaylists.size() ; i++) {
		if (!dynamic_cast<Playlist *>((Playlist*)mFinishedPlaylists[i])) {
			// Not one of our sounds
			continue;
		}
		pl = static_cast<Playlist*>(mFinishedPlaylists[i]);
		checkPlaylistState(pl);
		// Playlist was playing, and now enters silence.
		ASSERT(!sSoundManager.isActiveEnvSound(pl));
		ASSERT(pl->mState & PLAYLIST_PLAYING);
		setPlaylistState(pl, PLAYLIST_SILENCE);
		unsetPlaylistState(pl, PLAYLIST_PLAYING);
		// Was globally fading out?
		if (pl->mState & PLAYLIST_GLOBAL_MODE) {
			// then register as globally paused during silence.
			setPlaylistState(pl, PLAYLIST_PAUSED);
		}
		// Trick for resetting the field to 0 in the final loop.
		pl->mTimeSinceFinish = -globalTimeFrame;
	}
	mFinishedPlaylists.clear();

	// Update internal state of paused playlists
	for (unsigned int i=0 ; i < mPausedPlaylists.size() ; i++) {
		if (!dynamic_cast<Playlist *>((Playlist*)mPausedPlaylists[i])) {
			// Not one of our sounds.
			continue;
		}
		pl = static_cast<Playlist*>(mPausedPlaylists[i]);
		checkPlaylistState(pl);
		// Playlist was playing, and now it's paused.
		ASSERT(!sSoundManager.isPlayingEnvSound(pl));
		ASSERT(pl->mState & PLAYLIST_PLAYING);
		setPlaylistState(pl, PLAYLIST_PAUSED);
		unsetPlaylistState(pl, PLAYLIST_PLAYING);
	}
	mPausedPlaylists.clear();

	// Check for playlists which must start a new sound.
	for (unsigned int i=0 ; i < mPlaylists.size() ; i++) {
		pl = mPlaylists[i];
		if ((pl->mState & PLAYLIST_PAUSED) || !(pl->mState & PLAYLIST_SILENCE))
			continue;  // Irrelevant for current check.
		pl->mTimeSinceFinish += globalTimeFrame;  // Add up elapsed time.
		if (pl->mTimeSinceFinish >= pl->mSilence) {
			// Move one track forward.
			++pl->mCurrent %= pl->mList.size();  // =D
			// Mimic we're stopped, so we can invoke "startPlaylist"
			setPlaylistState(pl, PLAYLIST_STOPPED);
			unsetPlaylistState(pl, PLAYLIST_SILENCE);
			err = startPlaylist(pl->mName, pl);
			if (err != SSerror::SS_NO_ERROR) {
				debugERROR("Playlist \"%s\" failed after silence: %s.\n",
						pl->mName.c_str(), SSenumStr(err));
				stopPlaylist(pl->mName, pl);
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::globalPause()
{
	// First let the manager update the sound sources.
	sSoundManager.globalPause();

	// Now update playlists internal state.
	for (unsigned int i = 0 ; i < mPlaylists.size() ; i++) {
		Playlist *pl = mPlaylists[i];
		checkPlaylistState(pl);
		if (pl->mList.empty()) continue;  // Empty playlist: nothing to do.

		if (pl->mState & PLAYLIST_PLAYING) {
			// Playlist was playing: register as globally paused.
			unsetPlaylistState(pl, PLAYLIST_PLAYING);
			setPlaylistState(pl, PLAYLIST_PAUSED | PLAYLIST_GLOBAL_MODE);
			ASSERT(!sSoundManager.isPlayingEnvSound(pl));

		} else if ( (pl->mState & PLAYLIST_SILENCE) &&
					!(pl->mState & PLAYLIST_PAUSED)) {
			// Playlist was in silence: register as globally paused.
			setPlaylistState(pl, PLAYLIST_PAUSED | PLAYLIST_GLOBAL_MODE);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::globalPlay()
{
	// First let the manager update the sound sources.
	sSoundManager.globalPlay();

	// Now update playlists internal state.
	for (unsigned int i = 0 ; i < mPlaylists.size() ; i++) {
		Playlist *pl = mPlaylists[i];
		checkPlaylistState(pl);
		if (pl->mList.empty()) continue;  // Empty playlist: nothing to do.

		// Playback state info (i.e. discard playback settings)
		unsigned int pst = pl->mState & ~(PLAYLIST_REPEAT
											|PLAYLIST_RANDOM_ORDER
											|PLAYLIST_RANDOM_SILENCE);
		if (pst == (PLAYLIST_PAUSED | PLAYLIST_GLOBAL_MODE)) {
			// Playlist restarted playing.
			unsetPlaylistState(pl, PLAYLIST_PAUSED | PLAYLIST_GLOBAL_MODE);
			setPlaylistState(pl, PLAYLIST_PLAYING);
			ASSERT(sSoundManager.isPlayingEnvSound(pl));

		} else if (pst == (PLAYLIST_SILENCE
						   |PLAYLIST_PAUSED
						   |PLAYLIST_GLOBAL_MODE)) {
			// Playlist was in silence: deregister as globally paused.
			unsetPlaylistState(pl, PLAYLIST_PAUSED | PLAYLIST_GLOBAL_MODE);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::globalStop()
{
	// First let the manager update the sound sources
	sSoundManager.globalStop();
	// Now update playlists internal state.
	for (unsigned int i = 0 ; i < mPlaylists.size() ; i++) {
		Playlist *pl = mPlaylists[i];
		checkPlaylistState(pl);
		if (pl->mList.empty()) continue;  // Empty playlist: nothing to do.
		// Set inactive state.
		setPlaylistState(pl, PLAYLIST_STOPPED);
		unsetPlaylistState(pl, PLAYLIST_PLAYING
							 | PLAYLIST_PAUSED
							 | PLAYLIST_SILENCE
							 | PLAYLIST_GLOBAL_MODE);
		ASSERT(!sSoundManager.isActiveEnvSound(pl));
		pl->mTimeSinceFinish = 0.0f;
		pl->mCurrent = 0u;
	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::globalRestart()
{
	std::vector<Playlist*> toRestart;
	toRestart.reserve(mPlaylists.size());  // Avoid reallocations.

	// First stop currently active playlists.
	for (unsigned int i = 0 ; i < mPlaylists.size() ; i++) {
		Playlist *pl = mPlaylists[i];
		checkPlaylistState(pl);
		if (pl->mList.empty()) continue;  // Empty playlist: nothing to do.
		if (!(pl->mState & PLAYLIST_STOPPED)) {
			stopPlaylist(pl->mName, pl);
			toRestart.push_back(pl);
		}
	}

	// Now use the manager to update general system sounds.
	sSoundManager.globalRestart();

	// Finally restart previously stopped playlists.
	for (unsigned int i = 0 ; i < toRestart.size() ; i++) {
		startPlaylist(toRestart[i]->mName, toRestart[i]);
	}
	toRestart.clear();

	return;
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::globalFadeOut(const Ogre::Real& time, const bool pause)
{
	// First let the manager update the sound sources.
	sSoundManager.globalFadeOut(time, pause);
	// Now update playlists internal state.
	for (unsigned int i = 0 ; i < mPlaylists.size() ; i++) {
		Playlist *pl = mPlaylists[i];
		checkPlaylistState(pl);
		if (pl->mList.empty()) continue;  // Empty playlist: nothing to do.
		if (pl->mState & PLAYLIST_PLAYING) {
			setPlaylistState(mPlaylists[i], PLAYLIST_GLOBAL_MODE);
		}

	}
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::globalFadeIn(const Ogre::Real& time)
{
	// First let the manager update the sound sources
	sSoundManager.globalFadeIn(time);

	// Now update playlists internal state.
	for (unsigned int i = 0 ; i < mPlaylists.size() ; i++) {
		Playlist *pl = mPlaylists[i];
		checkPlaylistState(pl);
		if (pl->mList.empty()) continue;  // Empty playlist: nothing to do.

		if (!(pl->mState & PLAYLIST_GLOBAL_MODE)) {
			// Playlist shouldn't be affected by this function.
			continue;

		} else if (pl->mState & PLAYLIST_PLAYING) {
			// Under global fade-out: unset "global" status.
			unsetPlaylistState(pl, PLAYLIST_GLOBAL_MODE);

		} else if (pl->mState & PLAYLIST_SILENCE) {
			// Globally paused during silence: unset "global"+"pause" status.
			unsetPlaylistState(pl, PLAYLIST_PAUSED | PLAYLIST_GLOBAL_MODE);

		} else {
			// Globally paused during playback: playback resumed.
			ASSERT(pl->mState & PLAYLIST_PAUSED);
			ASSERT(sSoundManager.isPlayingEnvSound(
					pl->mList[pl->mPlayOrder[pl->mCurrent]]));
			unsetPlaylistState(pl, PLAYLIST_PAUSED);
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
		return ("Some playlist called \"" + name + "\" already exists!\n");
	} else {
		list.reserve(soundsList.size());
	}

	// Check requested sounds are available.
	for (unsigned int i=0 ; i < soundsList.size() ; i++) {
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
	Ogre::String sound("");
	for (unsigned int i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i]->mName != name)
			continue;
		// else: playlist found
		Playlist* pl = mPlaylists[i];
		if (pl->mList.empty()) {
			goto deletion;
		}
		// Stop any active playback.
		sound = (pl->mList[pl->mPlayOrder[pl->mCurrent]]);
		if (sSoundManager.isActiveEnvSound(sound)) {
			SSerror err = sSoundManager.stopEnvSound(sound);
			ASSERT(err == SSerror::SS_NO_ERROR);
		}
		deletion:
			mPlaylists[i] = mPlaylists[mPlaylists.size()-1];
			mPlaylists.pop_back();
			delete pl;
			return;
	}
}



////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::startPlaylist(const Ogre::String& name, Playlist *plp)
{
	SSerror err(SSerror::SS_NO_ERROR);
	Playlist *pl = plp ? plp : getPlaylist(name);

	if (!pl) {
		return SSerror::SS_FILE_NOT_FOUND;  // Playlist doesn't exist.
	} else {
		checkPlaylistState(pl);  // Check internal state consistency
	}

	if (!(pl->mState & (PLAYLIST_PAUSED | PLAYLIST_STOPPED))) {
		// Illegal operation.
		debugWARNING("Illegal operation attempted on playlist \"%s\":\n"
				"only paused/stopped playlists can be started. "
				"Nothing will be done.\n", pl->mName.c_str());
		return SSerror::SS_ILLEGAL_OPERATION;

	} else if (pl->mState & PLAYLIST_PLAYING) {
		// Playlist is already playing: nothing to do.
		debugWARNING("Playlist \"%s\" is already playing.\n", pl->mName.c_str());
		return SSerror::SS_NO_ERROR;

	} else if (pl->mState & PLAYLIST_SILENCE) {
		// Playlist in the middle of a silence.
		unsetPlaylistState(pl, PLAYLIST_PAUSED);
		unsetPlaylistState(pl, PLAYLIST_GLOBAL_MODE);  // Just in case
		return SSerror::SS_NO_ERROR;

	} else if (pl->mList.size() < 1) {
		// Playlist is empty: nothing to do.
		//debugWARNING("Empty playlist, won't start any playback.\n");
		return SSerror::SS_NO_ERROR;
	}

	// If we're at the beginning and random order was chosen,
	// shuffle the play order.
	if ((pl->mCurrent == 0) && (pl->mState & PLAYLIST_RANDOM_ORDER)) {
		std::random_shuffle(pl->mPlayOrder.begin(), pl->mPlayOrder.end());
	}
	// If random silence was chosen, well then, you know...
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
		pl->mTimeSinceFinish = 0.0f;
		setPlaylistState(pl, PLAYLIST_PLAYING);
		unsetPlaylistState(pl, PLAYLIST_PAUSED
							 | PLAYLIST_STOPPED
							 | PLAYLIST_SILENCE
							 | PLAYLIST_GLOBAL_MODE);
	} else {
		// Failure
		debugWARNING("Playlist \"%s\" failed to start: %s\n",
					pl->mName.c_str(), SSenumStr(err));
		setPlaylistState(pl, PLAYLIST_STOPPED);
		unsetPlaylistState(pl, PLAYLIST_PLAYING
							 | PLAYLIST_PAUSED
							 | PLAYLIST_SILENCE
							 | PLAYLIST_GLOBAL_MODE);
		err = SSerror::SS_INTERNAL_ERROR;
	}

	checkPlaylistState(pl);
	return err;
}



////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::pausePlaylist(const Ogre::String& name, Playlist *plp)
{
	const Ogre::String *sound(0);
	Playlist *pl = plp ? plp : getPlaylist(name);

	if (!pl) {
		debugWARNING("Playlist \"%s\" doesn't exist.\n", name.c_str());
		return;
	} else {
		checkPlaylistState(pl);  // Check internal state consistency
	}

	if (pl->mState & (PLAYLIST_PAUSED | PLAYLIST_STOPPED)) {
		// Illegal operation.
		debugWARNING("Illegal operation attempted on playlist \"%s\":\n"
				"paused/stopped playlists can't be paused. "
				"Nothing will be done.\n", pl->mName.c_str());
		return;

	} else if (pl->mState & PLAYLIST_SILENCE) {
		// Playlist in the middle of a silence.
		setPlaylistState(pl, PLAYLIST_PAUSED);
		return;

	} else if (pl->mList.size() < 1) {
		// Playlist is empty: nothing to do.
		//debugWARNING("Empty playlist, won't pause any playback.\n");
		return;
	}

	sound = &pl->mList[pl->mPlayOrder[pl->mCurrent]];

	if (!sSoundManager.isPlayingEnvSound(pl)) {
		debugERROR("Playlist \"%s\" isn't playing any sound, "
				"but the internal state says \"playing\"\n", pl->mName.c_str());
	} else {
		// All fine
		sSoundManager.pauseEnvSound(*sound, pl);
	}

	setPlaylistState(pl, PLAYLIST_PAUSED);
	unsetPlaylistState(pl, PLAYLIST_PLAYING | PLAYLIST_GLOBAL_MODE);

	checkPlaylistState(pl);
	return;
}



////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::stopPlaylist(const Ogre::String& name, Playlist *plp)
{
	Playlist *pl = plp ? plp : getPlaylist(name);

	if (!pl) {
		debugWARNING("Playlist \"%s\" doesn't exist.\n", name.c_str());
		return;
	} else {
		checkPlaylistState(pl);  // Check internal state consistency
	}

	if (pl->mList.size() < 1) {
		// Playlist is empty: nothing to do.
		//debugWARNING("Empty playlist, won't stop any playback.\n");
		return;

	} else if (pl->mState & PLAYLIST_STOPPED) {
		// Illegal operation.
		debugWARNING("Illegal operation attempted on playlist \"%s\":\n"
				"stopped playlists can't be stopped. "
				"Nothing will be done.\n", pl->mName.c_str());
		return;

	} else if ((pl->mState & PLAYLIST_PLAYING) ||
		((pl->mState & PLAYLIST_PAUSED) && !(pl->mState & PLAYLIST_SILENCE))) {
		// Playlist has an active sound: stop it.
		const Ogre::String *sound = &pl->mList[pl->mPlayOrder[pl->mCurrent]];
		if (!sSoundManager.isActiveEnvSound(pl)) {
			debugERROR("Playlist \"%s\" hasn't any active sound (%s), but the "
					"internal state says \"playing/paused\"\n",
					pl->mName.c_str(), sound->c_str());
		} else {
			SSerror err = sSoundManager.stopEnvSound(*sound, pl);
			if (err == SSerror::SS_NO_BUFFER) {
				// No EnvSound by the name "sound" was playing...
				// strange, but not really a problem at this stage.
				debugERROR("Playlist \"%s\" was playing an inexistent "
						"EnvSound... weird!\n", pl->mName.c_str());
			}
		}
	}

	pl->mCurrent = 0;
	pl->mTimeSinceFinish = 0.0f;
	setPlaylistState(pl, PLAYLIST_STOPPED);
	unsetPlaylistState(pl, PLAYLIST_PLAYING
						 | PLAYLIST_PAUSED
						 | PLAYLIST_SILENCE
						 | PLAYLIST_GLOBAL_MODE);

	checkPlaylistState(pl);
	return;
}



////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::fadeOutPlaylist(const Ogre::String& name,
								  const Ogre::Real& time,
								  const bool pause)
{
	const Ogre::String *sound(0);
	SSerror err(SSerror::SS_NO_ERROR);
	Playlist *pl = getPlaylist(name);

	if (!pl) {
		debugWARNING("Playlist \"%s\" doesn't exist.\n", name.c_str());
		return SSerror::SS_FILE_NOT_FOUND;
	} else {
		checkPlaylistState(pl);  // Check internal state consistency
	}

	if (pl->mList.size() < 1) {
		// Playlist is empty: nothing to do.
		//debugWARNING("Empty playlist, won't fade-out anything.\n");
		return SSerror::SS_NO_ERROR;

	} else if (!(pl->mState & PLAYLIST_PLAYING)) {
		// Illegal operation.
		debugWARNING("Illegal operation attempted on playlist \"%s\":\n"
				"only playing playlists can be faded-out. "
				"Nothing will be done.\n", pl->mName.c_str());
		return SSerror::SS_ILLEGAL_OPERATION;

	} else if (pl->mState & PLAYLIST_GLOBAL_MODE) {
		// Illegal operation.
		debugWARNING("Illegal operation attempted on playlist \"%s\":\n"
				"globally faded playlists can't be individually "
				"faded-out.\nNothing will be done.\n", pl->mName.c_str());
		return SSerror::SS_ILLEGAL_OPERATION;
	}

	sound = &pl->mList[pl->mPlayOrder[pl->mCurrent]];

	if (!sSoundManager.isPlayingEnvSound(*sound)) {
		debugERROR("Playlist \"%s\" isn't playing any sound, but the "
				"internal state says \"playing\"\n", pl->mName.c_str());
		goto fail;
	}

	err = sSoundManager.fadeOutEnvSound(*sound, time, pause, pl);

	if (err != SSerror::SS_NO_ERROR) {
		debugERROR("Playlist \"%s\" fade-out failed: %s",
				pl->mName.c_str(), SSenumStr(err));
		goto fail;
	}

	// Success
	checkPlaylistState(pl);
	return err;

	fail:
		sSoundManager.stopEnvSound(*sound);
		pl->mCurrent = 0;
		pl->mTimeSinceFinish = 0.0f;
		setPlaylistState(pl, PLAYLIST_STOPPED);
		unsetPlaylistState(pl, PLAYLIST_PLAYING
							 | PLAYLIST_PAUSED
							 | PLAYLIST_SILENCE
							 | PLAYLIST_GLOBAL_MODE);
		return SSerror::SS_INTERNAL_ERROR;
}



////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::fadeInPlaylist(const Ogre::String& name,
								 const Ogre::Real& time)
{
	SSerror err(SSerror::SS_NO_ERROR);
	Playlist *pl = getPlaylist(name);

	if (!pl) {
		debugWARNING("Playlist \"%s\" doesn't exist.\n", name.c_str());
		return SSerror::SS_FILE_NOT_FOUND;
	} else {
		checkPlaylistState(pl);  // Check internal state consistency
	}

	if (pl->mList.size() < 1) {
		// Playlist is empty: nothing to do.
		//debugWARNING("Empty playlist, won't fade-in anything.\n");
		return SSerror::SS_NO_ERROR;

	} else if (pl->mState & PLAYLIST_GLOBAL_MODE) {
		// Illegal operation.
		debugWARNING("Illegal operation attempted on playlist \"%s\":\n"
				"globally modified playlists can't be individually faded-in.\n"
				"Nothing will be done.\n", pl->mName.c_str());
		return SSerror::SS_ILLEGAL_OPERATION;

	} else if (pl->mState & PLAYLIST_SILENCE) {
		// Illegal operation.
		debugWARNING("Illegal operation attempted on playlist \"%s\":\n"
				"during silence, playlists can't be individually faded-in.\n"
				"Nothing will be done.\n", pl->mName.c_str());
		return SSerror::SS_ILLEGAL_OPERATION;

	} else if (pl->mState & PLAYLIST_STOPPED) {
		// Playlist is not active: we must start it first.
		err = startPlaylist(name, pl);
		if (err != SSerror::SS_NO_ERROR) {
			debugERROR("Couldn't start playlist \"%s\" fade-in: %s.\n",
						pl->mName.c_str(), SSenumStr(err));
			goto fail;
		}
		// Pause playback, so we can start gradually.
		sSoundManager.pauseEnvSound(pl->mList[pl->mPlayOrder[pl->mCurrent]]);
	}

	err = sSoundManager.fadeInEnvSound(pl->mList[pl->mPlayOrder[pl->mCurrent]],
									   time,
									   pl);
	if (err != SSerror::SS_NO_ERROR) {
		debugERROR("Playlist \"%s\" fade-in failed: %s.\n",
					pl->mName.c_str(), SSenumStr(err));
		sSoundManager.stopEnvSound(pl->mList[pl->mPlayOrder[pl->mCurrent]]);
		goto fail;
	}

	// Success
	setPlaylistState(pl, PLAYLIST_PLAYING);
	unsetPlaylistState(pl, PLAYLIST_PAUSED | PLAYLIST_STOPPED);
	checkPlaylistState(pl);
	return err;

	fail:
		pl->mCurrent = 0;
		pl->mTimeSinceFinish = 0.0f;
		setPlaylistState(pl, PLAYLIST_STOPPED);
		unsetPlaylistState(pl, PLAYLIST_PLAYING
							 | PLAYLIST_PAUSED
							 | PLAYLIST_SILENCE
							 | PLAYLIST_GLOBAL_MODE);
		return SSerror::SS_INTERNAL_ERROR;
}


////////////////////////////////////////////////////////////////////////////////
SSplayback
SoundHandler::getPlaylistPlayState(const Ogre::String& name) const
{
	const Playlist *pl = getPlaylist(name);

	if (!pl) {
		return SSplayback::SS_NONE;  // Playlist not found
	} else {
		checkPlaylistState(pl);
	}

	if (pl->mState & PLAYLIST_PLAYING) {
		// Playing
		if (pl->mState & PLAYLIST_GLOBAL_MODE) {
			debug("Playlist \"%s\" is globally fading out.\n", name.c_str());
		}
		return SSplayback::SS_PLAYING;

	} else if (pl->mState & PLAYLIST_PAUSED) {
		// Paused
		if (pl->mState & PLAYLIST_SILENCE) {
			debug("Playlist \"%s\" in middle of silence.\n", name.c_str());
		}
		return SSplayback::SS_PAUSED;

	} else if (pl->mState & PLAYLIST_SILENCE) {
		// Playing, but in the middle of silence
		debug("Playlist \"%s\" in middle of silence.\n", name.c_str());
		return SSplayback::SS_PLAYING;

	} else if (pl->mState & PLAYLIST_STOPPED) {
		// Stopped
		return SSplayback::SS_FINISHED;

	} else {
		// WTF?
		debugERROR("Inconsistent playing state for playlist \"%s\"\n",
					pl->mName.c_str());
		return SSplayback::SS_NONE;
	}
}



////////////////////////////////////////////////////////////////////////////////
bool
SoundHandler::setPlaylistRepeat(const Ogre::String& name, bool repeat)
{
	for (unsigned int i=0 ; i < mPlaylists.size() ; i++) {
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
	for (unsigned int i=0 ; i < mPlaylists.size() ; i++) {
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
	for (unsigned int i=0 ; i < mPlaylists.size() ; i++) {
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
	for (unsigned int i=0 ; i < mPlaylists.size() ; i++) {
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
	for (unsigned int i=0 ; i < mPlaylists.size() ; i++) {
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
	for (unsigned int i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i]->mName == name) {
			if (found) { *found = true; };
			return mPlaylists[i]->mState & PLAYLIST_RANDOM_SILENCE;
		}
	}
	if (found) { *found = false; };
	return false;
}


////////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
void
SoundHandler::checkPlaylistState(const Playlist *pl) const
{
	ASSERT(pl);
	// Get playback state information only (i.e. discard playback settings)
	unsigned int pst = pl->mState & ~( PLAYLIST_REPEAT
									  | PLAYLIST_RANDOM_ORDER
									  | PLAYLIST_RANDOM_SILENCE);
	// Any playlist should *ONLY* be in *ONE* of the following states
	ASSERT(1==(pst == PLAYLIST_STOPPED)
			+ (pst == PLAYLIST_PLAYING)
			+ (pst == PLAYLIST_PAUSED)
			+ (pst == PLAYLIST_SILENCE)
			+ (pst == (PLAYLIST_PLAYING	| PLAYLIST_GLOBAL_MODE))
			+ (pst == (PLAYLIST_PAUSED	| PLAYLIST_GLOBAL_MODE))
			+ (pst == (PLAYLIST_PAUSED	| PLAYLIST_SILENCE))
			+ (pst == (PLAYLIST_PAUSED	| PLAYLIST_GLOBAL_MODE
										| PLAYLIST_SILENCE)));
}
// else "checkPlaylistState(pl)" is defined in the header file
#endif

}
