/*
 * ExtraActionPanel.h
 *
 *  Created on: 16/06/2012
 *      Author: agustin
 */

#ifndef EXTRAACTIONPANEL_H_
#define EXTRAACTIONPANEL_H_

#include <OgreOverlay.h>
#include <OgrePanelOverlayElement.h>
#include <OgreString.h>

#include <vector>

#include "ExtraAction.h"


#define EXTRA_ACTION_PANEL_OVERLAY		"HUD/ActionPanel"
#define EXTRA_ACTION_PANEL_CONTAINTER	"ActionPanel/Main"

namespace hud {

class ExtraActionPanel
{
	typedef std::vector<ExtraAction *>	ElementVec;

public:
	ExtraActionPanel();
	virtual ~ExtraActionPanel();

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
	 * Returns the number of lockers that this manager have
	 */
	inline int getNumLockers(void) const;

	/**
	 * Returns the number of actual actions active
	 */
	int getNumActualActions(void) const;

	/**
	 * Remove some ExtraAction from the panel
	 * @param	pos		The position of the action to remove
	 * 					(0 <= pos < getNumLockers())
	 */
	void removeAction(int pos);

	/**
	 * Add new action to the manager
	 * @param	callback	The callback used to get the events
	 * @param	material	The material of the action
	 * @return	pos			The position of the action or -1 on error
	 */
	int addAction(ExtraAction::Callback *callback, const Ogre::String &material);

private:
	/**
	 *
	 */

private:
	Ogre::Overlay		*mOverlay;
	ElementVec			mElements;
	int					mActualEmpty;
};




inline int ExtraActionPanel::getNumLockers(void) const
{
	return mElements.size();
}

}

#endif /* EXTRAACTIONPANEL_H_ */
