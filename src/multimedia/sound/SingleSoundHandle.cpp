/*
 * SingleSoundHandle.cpp
 *
 *  Handle returned to the user for manipulation of single sounds.
 *
 *  All playback controls over the sound are provided by this handle.
 *  Creation and destruction of the handle should be performed only through
 *  the Soundhandler, with newSound() and deleteSound() methods respectively.
 *  This handle can also be fed into the SoundHandler instance for indirect
 *  manipulation, though that would be less efficient.
 *
 *   Created on: Jan 19, 2014
 *       Author: Carlos E. Budde
 *      Company: CordobaZombie
 *      License: GPLv3
 */

#include <debug/DebugUtil.h>
#include <sound/SingleSoundHandle.h>

namespace mm {


SingleSoundHandle::SingleSoundHandle(SoundHandler& sh,
                  SoundHandler::SingleSound* ss,
                  int index) :
    mSH(sh)
,   mSS(ss)
,   mIndex(index)
{
    /* Default constructor suffices */
}


SingleSoundHandle::~SingleSoundHandle()
{
    if (getPlaystate() != SSplayback::SS_FINISHED &&
        getPlaystate() != SSplayback::SS_NONE)
        stop();  // Stop playback
    delete mSS; // Release memory
}


SingleSoundHandle::SingleSoundHandle(const SingleSoundHandle& ssh) :
    mSH(ssh.mSH)
,   mSS(ssh.mSS)
,   mIndex(ssh.mIndex)
{
    if (!isValid())
        debugWARNING("Copying invalid SingleSoundHandle!\n");
}


SingleSoundHandle::SingleSoundHandle&
SingleSoundHandle::operator=(const SingleSoundHandle& ssh)
{
    if (this != &ssh) {
        mSH = ssh.mSH;
        mSS = ssh.mSS;
        mIndex = ssh.mIndex;
        if (!isValid())
            debugWARNING("Assigning invalid SingleSoundHandle!\n");
    }
    return *this;
}


bool
SingleSoundHandle::operator==(const SingleSoundHandle& ssh)
{
    return (mSH == ssh.mSH &&
            mSS == ssh.mSS &&
            mIndex == ssh.mIndex);
}


bool
SingleSoundHandle::isValid() const
{
    return mSH.isValidSound(*this);
}


SSerror
SingleSoundHandle::play()
{
    if (isValid())
        return mSH.startSound(*this);
    else
        return SSerror::SS_INVALID_HANDLE;
}


SSerror
SingleSoundHandle::pause()
{
    if (isValid())
        return mSH.pauseSound(*this);
    else
        return SSerror::SS_INVALID_HANDLE;
}


SSerror
SingleSoundHandle::stop()
{
    if (isValid())
        return mSH.stopSound(*this);
    else
        return SSerror::SS_INVALID_HANDLE;
}


SSerror
SingleSoundHandle::restart()
{
    if (isValid())
        return mSH.restartSound(*this);
    else
        return SSerror::SS_INVALID_HANDLE;
}


SSerror
SingleSoundHandle::fadeOut(float time, bool pause)
{
    if (isValid())
        return mSH.fadeOutSound(*this, time, pause);
    else
        return SSerror::SS_INVALID_HANDLE;
}


SSerror
SingleSoundHandle::fadeIn(float time)
{
    if (isValid())
        return mSH.fadeInSound(*this, time);
    else
        return SSerror::SS_INVALID_HANDLE;
}


const Ogre::String
SingleSoundHandle::getSoundname() const
{
    if (isValid())
        return mSH.getSoundSoundname(*this);
    else
        return "";
}


const SSplayback
SingleSoundHandle::getPlaystate() const
{
    if (isValid())
        return mSH.getSoundPlayState(*this);
    else
        return SSplayback::SS_NONE;
}


bool
SingleSoundHandle::setRepeat(bool repeat)
{
    if (isValid())
        return mSH.setSoundRepeat(*this, repeat);
    else
        return false;
}


bool
SingleSoundHandle::getRepeat(bool* found) const
{
    if (isValid()) {
        if (found)
            *found = true;
        return mSH.getSoundRepeat(*this);
    } else {
        if (found)
            *found = false;
        return false;
    }
}


bool
SingleSoundHandle::setGain(float gain)
{
    if (isValid())
        return mSH.setSoundGain(*this, gain);
    else
        return false;
}


float
SingleSoundHandle::getGain(bool* found) const
{
    if (isValid()) {
        if (found)
            *found = true;
        return mSH.getSoundGain(*this);
    } else {
        if (found)
            *found = false;
        return -1.0;
    }
}

SoundHandler::SingleSound* const
SingleSoundHandle::getSingleSound(bool* found=0) const
{
    if (isValid()) {
        if (found)
            *found = true;
        return mSS;
    } else {
        if (found)
            *found = false;
        return 0;
    }
}


} /* namespace mm */
