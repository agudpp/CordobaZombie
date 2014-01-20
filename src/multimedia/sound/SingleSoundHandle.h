/*
 * SingleSoundHandle.h
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
#ifndef SINGLESOUNDHANDLE_H_
#define SINGLESOUNDHANDLE_H_


#include <OgreString.h>
#include <sound/SoundEnums.h>
#include <sound/SoundHandler.h>

namespace mm {


struct SingleSoundHandle
{
    friend struct SoundHandler::SingleSound;

    SingleSoundHandle(SoundHandler& sh,
                      SoundHandler::SingleSound* ss,
                      int index);
    ~SingleSoundHandle();
    SingleSoundHandle(const SingleSoundHandle& ssh);
    SingleSoundHandle& operator=(const SingleSoundHandle& ssh);
    bool operator==(const SingleSoundHandle& ssh);

    /**
     * @brief   Is this handle currently operational in the sound system?
     * @return  false if handle is stale/invalid | true otherwise
     * @remarks If handle is invalid, it should be deleted via the SoundHandler
     */
    bool
    isValid() const;

    /**
     * @brief Start sound playback
     * @remarks See SoundHandler::startSound() for full descrition
     */
    SSerror
    play();

    /**
     * @brief Pause sound playback
     * @remarks See SoundHandler::pauseSound() for full descrition
     */
    SSerror
    pause();

    /**
     * @brief Stop sound playback
     * @remarks See SoundHandler::stopSound() for full descrition
     */
    SSerror
    stop();

    /**
     * @brief Restart sound playback
     * @remarks See SoundHandler::restartSound() for full descrition
     */
    SSerror
    restart();

    /**
     * @brief Fades sound playback out into silence
     * @remarks See SoundHandler::fadeOutSound() for full descrition
     */
    SSerror
    fadeOut(float time = 1.0, bool pause = true);

    /**
     * @brief Fades back in sound playback
     * @remarks See SoundHandler::fadeInSound() for full descrition
     */
    SSerror
    fadeIn(float time = 1.0);

    /**
     * @brief Retrieve the name of the sound contained
     * @remarks See SoundHandler::getSoundSoundname() for full descrition
     */
    const Ogre::String
    getSoundname() const;

    /**
     * @brief Get the sound playing state
     * @remarks See SoundHandler::getSoundPlayState() for full description
     */
    const SSplayback
    getPlaystate() const;

    /**
     * @brief Set/Get the sound {gain, repeat-on-end}
     *
     * @return
     * Setters: true on success | false if handle is invalid
     * Getters: field value
     *
     * @remarks
     * Getters: optional boolean "found" parameter will be filled with:
     * true    if operation successfull
     * false   if handle is invalid
     */
    bool setRepeat(bool  repeat);
    bool getRepeat(bool* found=0) const;
    bool setGain(float gain);
    float getGain(bool* found=0) const;
    SoundHandler::SingleSound* const getSingleSound(bool* found=0) const;

private:
    const int mIndex;              // Position index inside the handler
    SoundHandler& mSH;              // Reference to the handler
    SoundHandler::SingleSound* mSS; // Reference to sound inside the handler
};



} /* namespace mm */

#endif /* SINGLESOUNDHANDLE_H_ */
