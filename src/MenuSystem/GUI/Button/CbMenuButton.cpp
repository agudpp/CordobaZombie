/*
 * CbMenuButton.cpp
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#include "CbMenuButton.h"

CbMenuButton::CbMenuButton() :
mCb(0)
{

}

CbMenuButton::~CbMenuButton()
{

}

/**
 * When the button is pressed with right button (after is released)
 */
void CbMenuButton::rightPressed(void)
{
	if(mCb) (*mCb)(this, RIGHT_BUTTON);
}

/**
 * When the button is left Pressed
 */
void CbMenuButton::leftPressed(void)
{
	if(mCb) (*mCb)(this, LEFT_BUTTON);
}
