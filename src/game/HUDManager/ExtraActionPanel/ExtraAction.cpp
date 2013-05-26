/*
 * ExtraAction.cpp
 *
 *  Created on: 18/06/2012
 *      Author: agustin
 */

#include <OgreOverlayManager.h>
#include <OgrePanelOverlayElement.h>

#include "DebugUtil.h"
#include "ExtraAction.h"


namespace hud {

ExtraAction::ExtraAction() :
mCallback(0)
{
}

ExtraAction::~ExtraAction()
{

}



void ExtraAction::loadFromOverlay(Ogre::OverlayContainer *panel)
{
	ASSERT(panel);
	ASSERT(!mCont); // ensure that we only initialize once

	// now configure the button
	configureAll(panel);
	disableAction();
}


/**
 * Change the material name
 * @note This function requires that the overlayElement has been set before
 */
void ExtraAction::changeMaterial(const Ogre::String &matName)
{
	ASSERT(mCont);
	updateMaterial(matName);
	enableAction();
}

/**
 * Set the callback to use when the button is used
 */
void ExtraAction::setCallback(Callback *cb)
{
	mCallback = cb;
}


void ExtraAction::disableAction(void)
{
	ASSERT(mCont);
//	mCont->setMaterialName("");
	setEnable(false);
	setActive(false);
}
void ExtraAction::enableAction(void)
{
	ASSERT(mCont);
	setActive(true);
	setEnable(true);
}
bool ExtraAction::isActionEnable(void)
{
	return isActive();
}

/**
 * When the button is pressed with right button (after is released)
 */
void ExtraAction::rightPressed(void)
{
	if(mCallback) {(*mCallback)(1);}
}

/**
 * When the button is left Pressed
 */
void ExtraAction::leftPressed(void)
{
	if(mCallback) {(*mCallback)(0);}
}

}
