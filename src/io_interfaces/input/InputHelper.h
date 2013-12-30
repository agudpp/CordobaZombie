/*
 * InputHelper.h
 *
 *  Created on: Jul 22, 2013
 *      Author: agustin
 */

#ifndef INPUTHELPER_H_
#define INPUTHELPER_H_

#include <vector>
#include <algorithm>

#include "InputMouse.h"
#include "InputKeyboard.h"
#include "KeyTracker.h"

namespace input {

class InputHelper {
public:

    // @brief Construct the class with the list of mouse buttons and keyboards
    //        keys we will track
    // @param mouseButtons      The list of mouse buttons ids we want to track
    // @param keyboardKeys      The list of keyboard keys we will track
    //
    InputHelper(const std::vector<input::MouseButtonID>& mouseButtons,
                const std::vector<input::KeyCode>& keyboardKeys);
    InputHelper(){};
    ~InputHelper();

    // @brief Be able to set the keys we will use in runtime
    // @param mouseButtons      The list of mouse buttons ids we want to track
    // @param keyboardKeys      The list of keyboard keys we will track
    //
    void
    setInputConfig(const std::vector<input::MouseButtonID>& mouseButtons,
                   const std::vector<input::KeyCode>& keyboardKeys);

    // @brief Update this class to check all the keyboards and mouse button
    //        that are pressed and released.
    //        You should call this method before checking if a button is pressed
    //        or released. Note also that we will only update the list of buttons
    //        that you assign in the constructor of this class.
    //
    void
    update(void);

    // @brief Check if a particular mouse button is/was pressed / released
    // @param buttonID  The mouse button ID should be passed in the constructor
    //                  of this class.
    //
    inline bool
    isMousePressed(input::MouseButtonID buttonID) const;
    inline bool
    isMouseReleased(input::MouseButtonID buttonID) const;
    inline bool
    wasMousePressed(input::MouseButtonID buttonID) const;

    // @brief Check if a particular keyboard key is/was pressed / released
    // @param key       The key we want to check, it should be passed in the
    //                  constructor list
    //
    inline bool
    isKeyPressed(input::KeyCode key) const;
    inline bool
    isKeyReleased(input::KeyCode key) const;
    inline bool
    wasKeyPressed(input::KeyCode key) const;

private:

    // Typedefs
    //
    typedef std::vector<input::MouseButtonID> MouseIDVec;
    typedef std::vector<input::KeyCode> KeyVec;

    KeyTracker<input::MouseButtonID> mMouseTracker;
    KeyTracker<input::KeyCode> mKeyboardTracker;
    MouseIDVec mMouseIDs;
    KeyVec mKeyIDs;

};




////////////////////////////////////////////////////////////////////////////////
// Inline stuff



inline bool
InputHelper::isMousePressed(input::MouseButtonID buttonID) const
{
    // slow precondition
    ASSERT(std::find(mMouseIDs.begin(), mMouseIDs.end(), buttonID) != mMouseIDs.end());
    return mMouseTracker.isKeyPressed(buttonID);
}
inline bool
InputHelper::isMouseReleased(input::MouseButtonID buttonID) const
{
    // slow precondition
    ASSERT(std::find(mMouseIDs.begin(), mMouseIDs.end(), buttonID) != mMouseIDs.end());
    return mMouseTracker.isKeyReleased(buttonID);
}
inline bool
InputHelper::wasMousePressed(input::MouseButtonID buttonID) const
{
    // slow precondition
    ASSERT(std::find(mMouseIDs.begin(), mMouseIDs.end(), buttonID) != mMouseIDs.end());
    return mMouseTracker.wasKeyPressed(buttonID);
}


inline bool
InputHelper::isKeyPressed(input::KeyCode key) const
{
    // slow precondition
    ASSERT(std::find(mKeyIDs.begin(), mKeyIDs.end(), key) != mKeyIDs.end());
    return mKeyboardTracker.isKeyPressed(key);
}
inline bool
InputHelper::isKeyReleased(input::KeyCode key) const
{
    // slow precondition
    ASSERT(std::find(mKeyIDs.begin(), mKeyIDs.end(), key) != mKeyIDs.end());
    return mKeyboardTracker.isKeyReleased(key);
}
inline bool
InputHelper::wasKeyPressed(input::KeyCode key) const
{
    // slow precondition
    ASSERT(std::find(mKeyIDs.begin(), mKeyIDs.end(), key) != mKeyIDs.end());
    return mKeyboardTracker.wasKeyPressed(key);
}

}

#endif /* INPUTHELPER_H_ */
