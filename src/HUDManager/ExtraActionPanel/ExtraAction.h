/*
 * ExtraAction.h
 *
 *  Created on: 18/06/2012
 *      Author: agustin
 */

#ifndef EXTRAACTION_H_
#define EXTRAACTION_H_

#include <OgreString.h>
#include <OgrePanelOverlayElement.h>

#include "MenuButton.h"


namespace hud {

class ExtraAction : public MenuButton
{
public:
	struct Callback {
		/**
		 * Callback called when the button is clicked.
		 * @param	buttonPressed	{0 = left, 1 = right}
		 */
		virtual void operator()(int buttonPressed) = 0;
	};
public:
	ExtraAction();
	virtual ~ExtraAction();

	/**
	 * Set the overlay to use.
	 */
	void loadFromOverlay(Ogre::OverlayContainer *cont);

	/**
	 * Change the material name
	 * @note This function requires that the overlayElement has been set before
	 */
	void changeMaterial(const Ogre::String &matName);

	/**
	 * Set the callback to use when the button is used
	 */
	void setCallback(Callback *cb);
	inline Callback *getCallback(void);

	/**
	 * This function disable/enable the extra action (clearing the material and
	 * IMenu functionality)
	 */
	void disableAction(void);
	void enableAction(void);
	bool isActionEnable(void);

protected:
	/**
	 * When the button is pressed with right button (after is released)
	 */
	void rightPressed(void);

	/**
	 * When the button is left Pressed
	 */
	void leftPressed(void);

private:
	Callback						*mCallback;
};



inline ExtraAction::Callback *ExtraAction::getCallback(void)
{
	return mCallback;
}

}

#endif /* EXTRAACTION_H_ */
