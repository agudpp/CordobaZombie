/*
 * PlaylistHandle.cpp
 *
 *  Handle returned to the user for manipulation of playlists of sounds.
 *
 *  All playback controls over the playlist are provided by this handle.
 *  Creation and destruction of the handle should be performed only through
 *  the Soundhandler, with newPlaylist() and deletePlaylist() methods resp.
 *  This handle can also be fed into the SoundHandler instance for direct
 *  manipulation, though that would be less efficient.
 *
 *   Created on: Jan 19, 2014
 *       Author: Carlos E. Budde
 *      Company: CordobaZombie
 *      License: GPLv3
 */

#include <debug/DebugUtil.h>
#include <sound/PlaylistHandle.h>

namespace mm {

PlaylistHandle::PlaylistHandle(SoundHandler& sh,
                  SoundHandler::Playlist* p,
                  int index) :
    mSH(sh)
,   mP(p)
,   mIndex(index)
{
    /* Default constructor suffices */
}


PlaylistHandle::~PlaylistHandle()
{
    if (getPlaystate() != SSplayback::SS_FINISHED &&
        getPlaystate() != SSplayback::SS_NONE)
        stop();  // Stop playback
    delete mP; // Release memory
}


PlaylistHandle::PlaylistHandle(const PlaylistHandle& ssh) :
    mSH(ssh.mSH)
,   mP(ssh.mP)
,   mIndex(ssh.mIndex)
{
    if (!isValid())
        debugWARNING("Copying invalid PlaylistHandle!\n");
}


PlaylistHandle::PlaylistHandle&
PlaylistHandle::operator=(const PlaylistHandle& ssh)
{
    if (this != &ssh) {
        mSH = ssh.mSH;
        mP = ssh.mP;
        mIndex = ssh.mIndex;
        if (!isValid())
            debugWARNING("Assigning invalid PlaylistHandle!\n");
    }
    return *this;
}


bool
PlaylistHandle::operator==(const PlaylistHandle& ssh)
{
    return (mSH == ssh.mSH &&
            mP == ssh.mP &&
            mIndex == ssh.mIndex);
}


bool
PlaylistHandle::isValid() const
{
    return mSH.isValidPlaylist(*this);
}


SSerror
PlaylistHandle::play()
{
    if (isValid())
        return mSH.startPlaylist(*this);
    else
        return SSerror::SS_INVALID_HANDLE;
}


SSerror
PlaylistHandle::pause()
{
    if (isValid())
        return mSH.pausePlaylist(*this);
    else
        return SSerror::SS_INVALID_HANDLE;
}


SSerror
PlaylistHandle::stop()
{
    if (isValid())
        return mSH.stopPlaylist(*this);
    else
        return SSerror::SS_INVALID_HANDLE;
}


SSerror
PlaylistHandle::restart()
{
    if (isValid())
        return mSH.restartPlaylist(*this);
    else
        return SSerror::SS_INVALID_HANDLE;
}


SSerror
PlaylistHandle::fadeOut(float time, bool pause)
{
    if (isValid())
        return mSH.fadeOutPlaylist(*this, time, pause);
    else
        return SSerror::SS_INVALID_HANDLE;
}


SSerror
PlaylistHandle::fadeIn(float time)
{
    if (isValid())
        return mSH.fadeInPlaylist(*this, time);
    else
        return SSerror::SS_INVALID_HANDLE;
}


const std::vector<Ogre::String>
PlaylistHandle::getSoundlist() const
{
    if (isValid())
        return mSH.getPlaylistSoundlist(*this);
    else {
        std::vector<Ogre::String> v;
        return v;
    }
}


const Ogre::String
PlaylistHandle::getCurrentSoundname() const
{
    if (isValid()) {
        return "TODO!!!\n";  // TODO
    } else
        return "";
}


const SSplayback
PlaylistHandle::getPlaystate() const
{
    if (isValid())
        return mSH.getPlaylistPlayState(*this);
    else
        return SSplayback::SS_NONE;
}


bool
PlaylistHandle::setGain(float gain)
{
    if (isValid())
        return mSH.setPlaylistGain(*this, gain);
    else
        return false;
}


float
PlaylistHandle::getGain(bool* found) const
{
    if (isValid()) {
        if (found)
            *found = true;
        return mSH.getPlaylistGain(*this);
    } else {
        if (found)
            *found = false;
        return -1.0;
    }
}



bool
PlaylistHandle::setRepeat(bool repeat)
{
    if (isValid())
        return mSH.setPlaylistRepeat(*this, repeat);
    else
        return false;
}


bool
PlaylistHandle::getRepeat(bool* found) const
{
    if (isValid()) {
        if (found)
            *found = true;
        return mSH.getPlaylistRepeat(*this);
    } else {
        if (found)
            *found = false;
        return false;
    }
}


bool
PlaylistHandle::setRandomOrder(bool random)
{
    if (isValid())
        return mSH.setPlaylistRandomOrder(*this, random);
    else
        return false;
}


bool
PlaylistHandle::getRandomOrder(bool* found) const
{
    if (isValid()) {
        if (found)
            *found = true;
        return mSH.getPlaylistRandomOrder(*this);
    } else {
        if (found)
            *found = false;
        return false;
    }
}


bool
PlaylistHandle::setRandomSilence(bool random)
{
    if (isValid())
        return mSH.setPlaylistRandomSilence(*this, random);
    else
        return false;
}


bool
PlaylistHandle::getRandomSilence(bool* found) const
{
    if (isValid()) {
        if (found)
            *found = true;
        return mSH.getPlaylistRandomSilence(*this);
    } else {
        if (found)
            *found = false;
        return false;
    }
}

SoundHandler::Playlist* const
PlaylistHandle::getPlaylist(bool* found=0) const
{
    if (isValid()) {
        if (found)
            *found = true;
        return mP;
    } else {
        if (found)
            *found = false;
        return 0;
    }
}


} /* namespace mm */
