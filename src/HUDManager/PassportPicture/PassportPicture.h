/*
 * PassportPicture.h
 *
 *  Created on: 16/06/2012
 *      Author: agustin
 */

#ifndef PASSPORTPICTURE_H_
#define PASSPORTPICTURE_H_

#include <OgreOverlay.h>
#include <OgrePanelOverlayElement.h>
#include <OgreString.h>

#include "HudElement.h"

#define PASSPORT_PICTURE_CONT		"HUD/PSP/Passport"
#define PASSPORT_PICTURE_OVERLAY	"HUD/PassportPicture"

class PassportPicture : public HudElement
{
public:
	PassportPicture();
	virtual ~PassportPicture();

	/**
	 * Load/Unload all the resources of the HudElement
	 */
	void load(void);
	void unload(void);

	/**
	 * Hide/Show the element
	 */
	void show(void);
	void hide(void);

	/**
	 * Change the passport picture (set the texture name)
	 */
	void changePicture(const Ogre::String &textureName);

private:

	Ogre::PanelOverlayElement	*mPanel;
};

#endif /* PASSPORTPICTURE_H_ */
