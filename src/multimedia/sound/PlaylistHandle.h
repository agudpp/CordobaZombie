/*
 * PlaylistHandle.h
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
#ifndef PLAYLISTHANDLE_H_
#define PLAYLISTHANDLE_H_


#include <vector>
#include <OgreString.h>
#include <sound/SoundEnums.h>
#include <sound/SoundHandler.h>


namespace mm {

struct PlaylistHandle
{
    PlaylistHandle(SoundHandler& sh,
                      SoundHandler::Playlist& p,
                      int index);
    ~PlaylistHandle();
    PlaylistHandle(const PlaylistHandle& ph);
    PlaylistHandle& operator=(const PlaylistHandle& ph);
    bool operator==(const PlaylistHandle& ph);

    /**
     * @brief Is this handle currently operation in the sound system?
     * @return false if handle is stale/invalid | true otherwise
     * @remarks If handle is invalid, it should be deleted via the SoundHandler
     */
    bool
    isValid() const;

    /**
     * @brief Start playlist playback
     * @remarks See SoundHandler::startPlaylist() for full descrition
     */
    SSerror
    play();

    /**
     * @brief Pause playlist playback
     * @remarks See SoundHandler::pausePlaylist() for full descrition
     */
    SSerror
    pause();

    /**
     * @brief Stop playlist playback
     * @remarks See SoundHandler::stopPlaylist() for full descrition
     */
    SSerror
    stop();

    /**
     * @brief Restart playlist playback
     * @remarks See SoundHandler::restartPlaylist() for full descrition
     */
    SSerror
    restart();

    /**
     * @brief Fades playlist playback out into silence
     * @remarks See SoundHandler::fadeOutPlaylist() for full descrition
     */
    SSerror
    fadeOut(float time = 1.0, bool pause = true);

    /**
     * @brief Fades back in playlist playback
     * @remarks See SoundHandler::fadeInPlaylist() for full descrition
     */
    SSerror
    fadeIn(float time = 1.0);

    /**
     * @brief Retrieve the list of sounds contained
     * @remarks See SoundHandler::getPlaylistSoundlist() for full descrition
     */
    const std::vector<Ogre::String>
    getSoundlist() const;

    /**
     * @brief Retrieve the name of the sound currently set
     * @return Current sound name | Empty string on error
     * @remarks Playlist may be in mid-silence
     */
    const Ogre::String
    getCurrentSoundname() const;

    /**
     * @brief Get the playlist playing state
     * @remarks See SoundHandler::getPlaylistPlayState() for full description
     */
    const SSplayback
    getPlaystate() const;

    /**
     * @brief Set/Get the playlist {gain, repeat-on-end, random-order, random-silence}
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
    bool setGain(float gain);
    float getGain(bool* found=0) const;
    bool setRepeat(bool  repeat);
    bool getRepeat(bool* found=0) const;
    bool setRandomOrder(bool  random);
    bool getRandomOrder(bool* found=0) const;
    bool setRandomSilence(bool  random);
    bool getRandomSilence(bool* found=0) const;

private:
    const int mIndex;          // Position index inside the handler
    SoundHandler& mSH;          // Reference to the handler
    SoundHandler::Playlist& mP; // Reference to playlist inside the handler
};

} /* namespace mm */

#endif /* PLAYLISTHANDLE_H_ */
