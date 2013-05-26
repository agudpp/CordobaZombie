/*
 * InputKeys.h
 *
 *  Created on: Mar 31, 2013
 *      Author: agustin
 */

#ifndef INPUTKEYS_H_
#define INPUTKEYS_H_

#include <bitset>

namespace input {

class InputManager;

// Here we let public the possibility to configure the keys used in the game
enum inputID {
    KEY_GROUP_UNITS = 0,
    KEY_MOVE_CAM_UP,
    KEY_MOVE_CAM_DOWN,
    KEY_MOVE_CAM_LEFT,
    KEY_MOVE_CAM_RIGHT,
    KEY_MOVE_CAM_FREE,      // key used to move the cam with the mouse
    KEY_EXIT_GAME,
    KEY_PAUSE_GAME,
    KEY_OPEN_CELLPHONE,

    ///////////////////////////////////////////////////////////////////////
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MIDDLE,


    NUM_KEYS,
    NUM_KEYBOARD_KEYS = NUM_KEYS - 3,
};

class KeyFlags {
public:

    // check if the key is currently pressed
    //
    inline bool isKeyPressed(inputID key) const
    {
        return mCurrentKeys[key];
    }

    // check if the key was pressed in the last frame
    //
    inline bool wasKeyPressed(inputID key) const
    {
        return mLastKeys[key];
    }

    // check if the key was released this frame
    //
    inline bool isKeyReleased(inputID key) const
    {
        return mLastKeys[key] && !mCurrentKeys[key];
    }
private:
    friend InputManager;

    // clear all the values of a key
    //
    inline void clearAll(inputID key)
    {
        mCurrentKeys[key] = mLastKeys[key] = 0;
    }

    // call this method when the key is not pressed anymore
    //
    inline void keyUnpressed(inputID key)
    {
        mLastKeys[key] = mCurrentKeys[key];
        mCurrentKeys[key] = 0;
    }

    // call this method when a key is pressed
    //
    inline void keyPressed(inputID key)
    {
        mLastKeys[key] = mCurrentKeys[key];
        mCurrentKeys[key] = 1;

    }

    // call this method for a specific key and with a specific state
    //
    inline void setKeyNewState(inputID key, bool pressed)
    {
        mLastKeys[key] = mCurrentKeys[key];
        mCurrentKeys[key] = pressed;
    }

private:
    std::bitset<NUM_KEYS> mCurrentKeys;
    std::bitset<NUM_KEYS> mLastKeys;

};

}


#endif /* INPUTKEYS_H_ */
