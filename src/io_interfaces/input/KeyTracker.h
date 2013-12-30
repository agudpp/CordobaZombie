/*
 * KeyTracker.h
 *
 *  Created on: Jul 22, 2013
 *      Author: agustin
 */

#ifndef KEYTRACKER_H_
#define KEYTRACKER_H_


#include <vector>

namespace input {

// Helper class to track a list of keys to check if there was pressed / released
//

template <typename ID_TYPE>
class KeyTracker {
public:

    // @brief Constructor with the size of the keys/buttons we will track
    // @param numKeys   The number of keys we will track
    //
    KeyTracker(unsigned int numKeys) :
        mCurrentKeys(numKeys, false)
    ,   mLastKeys(numKeys, false)
    {
    }

    KeyTracker()
    {
    }

    // @brief Set the size of the keys to be used
    inline void
    setNumKeysToTrack(unsigned int numKeys)
    {
        mCurrentKeys.resize(numKeys, false);
        mLastKeys.resize(numKeys, false);
    }

    // check if the key is currently pressed
    //
    inline bool isKeyPressed(ID_TYPE key) const
    {
        return mCurrentKeys[static_cast<unsigned int>(key)];
    }

    // check if the key was pressed in the last frame
    //
    inline bool wasKeyPressed(ID_TYPE key) const
    {
        return mLastKeys[static_cast<unsigned int>(key)];
    }

    // check if the key was released this frame
    //
    inline bool isKeyReleased(ID_TYPE key) const
    {
        return mLastKeys[static_cast<unsigned int>(key)] &&
                !mCurrentKeys[static_cast<unsigned int>(key)];
    }

    // clear all the values of a key
    //
    inline void clearAll(ID_TYPE key)
    {
        mCurrentKeys[static_cast<unsigned int>(key)] =
                mLastKeys[static_cast<unsigned int>(key)] = 0;
    }

    // call this method when the key is not pressed anymore
    //
    inline void keyUnpressed(ID_TYPE key)
    {
        mLastKeys[static_cast<unsigned int>(key)] =
                mCurrentKeys[static_cast<unsigned int>(key)];
        mCurrentKeys[static_cast<unsigned int>(key)] = 0;
    }

    // call this method when a key is pressed
    //
    inline void keyPressed(ID_TYPE key)
    {
        mLastKeys[static_cast<unsigned int>(key)] =
                mCurrentKeys[static_cast<unsigned int>(key)];
        mCurrentKeys[static_cast<unsigned int>(key)] = 1;

    }

    // call this method for a specific key and with a specific state
    //
    inline void setKeyNewState(ID_TYPE key, bool pressed)
    {
        mLastKeys[static_cast<unsigned int>(key)] =
                mCurrentKeys[static_cast<unsigned int>(key)];
        mCurrentKeys[static_cast<unsigned int>(key)] = pressed;
    }

private:
    std::vector<bool> mCurrentKeys;
    std::vector<bool> mLastKeys;

};

}

#endif /* KEYTRACKER_H_ */
