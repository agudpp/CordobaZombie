/*
 * FESimpleButton.cpp
 *
 *  Created on: Oct 28, 2013
 *      Author: agustin
 */

#include "FESimpleButton.h"

namespace ui {


////////////////////////////////////////////////////////////////////////////////
void
FESimpleButton::mouseIn(const FEInfo& info)
{
    // change the atlas coords
    if (mCurrentState != State::S_MOUSE_OVER && mEnabled) {
        setAtlasCoords(State::S_MOUSE_OVER);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
FESimpleButton::mouseOut(const FEInfo& info)
{
    // we have to set normal if the button is enabled, Disabled otherwise
    if (mEnabled) {
        setAtlasCoords(State::S_NORMAL);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
FESimpleButton::mouseOver(const FEInfo& info)
{
    if (!mEnabled) {
        // do nothing.
        return;
    }

    // if the button is clicked we have to change its atlas
    if ((mCurrentState != State::S_CLICKED) &&
        info.input.isMousePressed(input::MouseButtonID::MB_Left)) {
        setAtlasCoords(State::S_CLICKED);
        return;
    }

    // check if we release the button so we can emit the signal
    if (info.input.isMouseReleased(input::MouseButtonID::MB_Left)) {
        // call the signal if we have one
        if (mButtonPressedCallback) {
            mButtonPressedCallback(this, Event::E_RIGHT_PRESSED);
        }

        // reset the atlas to be mouseOVER
        setAtlasCoords(State::S_MOUSE_OVER);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
FESimpleButton::onEvent(const FEInfo& info)
{
    // not implemented
    debug("NOT IMPLEMENTED\n");
}

} /* namespace ui */
