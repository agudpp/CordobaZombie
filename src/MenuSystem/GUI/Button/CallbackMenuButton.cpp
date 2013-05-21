/*
 * CallbackMenuButton.cpp
 *
 *  Created on: May 4, 2013
 *      Author: agustin
 */

#include "CallbackMenuButton.h"

CallbackMenuButton::CallbackMenuButton()
{
}

CallbackMenuButton::~CallbackMenuButton()
{
}

/**
 * When the button is pressed with right button (after is released)
 */
void
CallbackMenuButton::rightPressed(void)
{
    mSignal(this, ButtonID::RIGHT_BUTTON);
}

/**
 * When the button is left Pressed
 */
void
CallbackMenuButton::leftPressed(void)
{
    mSignal(this, ButtonID::LEFT_BUTTON);
}
