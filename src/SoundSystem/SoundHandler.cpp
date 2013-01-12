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
#include <strings.h>  // strcasecmp

#include "SoundEnums.h"
#include "SoundHandler.h"



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
		return "Operation successfull.";
		break;
	case SSerror::SS_NO_MEMORY:
		return "Insufficient system memory.\n";
		break;
	case SSerror::SS_INVALID_FILE:
		return "Unsupported/bad audio format.\n";
		break;
	case SSerror::SS_FILE_NOT_FOUND:
		return "File not found.\n";
		break;
	case SSerror::SS_INTERNAL_ERROR:
		return "Unspecified internal error.\n";
		break;
	default:
		return "Unspecified BIG internal error (???)\n";
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
	mSilence(0.0f)
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
	mSilence(silence)
{
	// Initialize play order
	mPlayOrder.resize(list.size());
	for (uint i=0 ; i < list.size() ; i++) { mPlayOrder.push_back(i); }
	// Set internal state according to calling parameters
	mState = PLAYLIST_STOPPED
			| (repeat ? PLAYLIST_REPEAT : 0)
			| (randomOrder ? PLAYLIST_RANDOM_ORDER : 0)
			| (randomSilence ? PLAYLIST_RANDOM_SILENCE : 0);
	// If random was chosen, shuffle the play order
	if (randomOrder) {
		std::random_shuffle(mPlayOrder.begin(),mPlayOrder.end());
	}
}


////////////////////////////////////////////////////////////////////////////////
SoundHandler::Playlist::Playlist(const Playlist& pl) :
		mName(pl.mName),
		mList(pl.mList),
		mPlayOrder(pl.mPlayOrder),
		mSilence(pl.mSilence)
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
void
SoundHandler::unloadSounds(const std::vector<Ogre::String>& list)
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::shutDown(void)
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::update()
{
	sSoundManager.update();
	for (uint i=0 ; i < mPlaylists.size() ; i++) {

	}
	/* TODO:
	 *	¤ call SoundManager.update()
	 *	¤ update all playlists (i.e., control timers and stuff)
	 */
}


////////////////////////////////////////////////////////////////////////////////
Ogre::String
SoundHandler::newPlaylist(const Ogre::String& name,
						  const std::vector<Ogre::String>& soundsList,
						  bool  repeat,
						  bool  random,
						  float silence)
{
	const char unloadedStr[] = ": Sound not loaded in the system\n";
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
				pl->mState &= ~PLAYLIST_PLAYING;
				pl->mState |= PLAYLIST_STOPPED;
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
SoundHandler::startPlaylist(const Ogre::String& name) const
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::pausePlaylist(const Ogre::String& name) const
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
void
SoundHandler::stopPlaylist(const Ogre::String& name) const
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::restartPlaylist(const Ogre::String& name) const
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::fadeOutPlaylist(const Ogre::String& name,
							  const Ogre::Real& time,
							  const bool pause) const
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
SSerror
SoundHandler::fadeInPlaylist(const Ogre::String& name,
							 const Ogre::Real& time) const
{
	// TODO
	debugWARNING("TODO\n");ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
SSplayback
SoundHandler::getPlaylistPlayState(const Ogre::String& name)
{
	for (uint i=0 ; i < mPlaylists.size() ; i++) {
		if (mPlaylists[i].mName == name) {
			if (mPlaylists[i].mState & PLAYLIST_PLAYING) {
				// Playing
				ASSERT(!(mPlaylists[i].mState & (PLAYLIST_PAUSED | PLAYLIST_STOPPED)));
				return SSplayback::SS_PLAYING;

			} else if (mPlaylists[i].mState & PLAYLIST_PAUSED) {
				// Paused
				ASSERT(!(mPlaylists[i].mState & (PLAYLIST_PLAYING | PLAYLIST_STOPPED)));
				return SSplayback::SS_PAUSED;

			} else if (mPlaylists[i].mState & PLAYLIST_STOPPED) {
				// Stopped
				ASSERT(!(mPlaylists[i].mState & (PLAYLIST_PLAYING | PLAYLIST_PAUSED)));
				return SSplayback::SS_FINISHED;

			} else {
				// WTF?
				debugERROR("Inconsistent playing state for playlist \"%s\"\n",
							mPlaylists[i].mName.c_str());
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
