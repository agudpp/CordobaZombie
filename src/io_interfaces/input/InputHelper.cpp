/*
 * InputHelper.cpp
 *
 *  Created on: Jul 22, 2013
 *      Author: agustin
 */

#include "InputHelper.h"

#include <types/basics.h>

namespace input {

InputHelper::InputHelper(const std::vector<input::MouseButtonID>& mouseButtons,
                         const std::vector<input::KeyCode>& keyboardKeys) :
    mMouseTracker(static_cast<unsigned int>(
            *std::max_element(mouseButtons.begin(), mouseButtons.end())))
,   mKeyboardTracker(static_cast<unsigned int>(
        *std::max_element(keyboardKeys.begin(), keyboardKeys.end())))
,   mMouseIDs(mouseButtons)
,   mKeyIDs(keyboardKeys)
{

}

InputHelper::~InputHelper()
{
    // do nothing
}

void
InputHelper::update(void)
{
    // capture mouse and keyaboard
    input::Mouse::capture();
    input::Keyboard::capture();

    // update each mouse and keyboard key
    for (core::size_t i = 0, size = mKeyIDs.size(); i < size; ++i) {
        mKeyboardTracker.setKeyNewState(mKeyIDs[i],
                                        input::Keyboard::isKeyDown(mKeyIDs[i]));
    }
    for (core::size_t i = 0, size = mMouseIDs.size(); i < size; ++i) {
        mMouseTracker.setKeyNewState(mMouseIDs[i],
                                     input::Mouse::isMouseDown(mMouseIDs[i]));
    }
}

}
