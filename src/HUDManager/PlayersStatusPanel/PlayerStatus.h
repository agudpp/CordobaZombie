/*
 * PlayerStatus.h
 *
 *  Created on: 18/06/2012
 *      Author: agustin
 */

#ifndef PLAYERSTATUS_H_
#define PLAYERSTATUS_H_

#include <OgreOverlayContainer.h>

#include "MenuButton.h"
#include "GeneralTypedefs.h"
#include "LoadingBar.h"
#include "PlayerUnit.h"

class PlayerUnit;

class PlayerStatus : public MenuButton
{
public:
	typedef GenericFunctor<void, PlayerStatus *>	Callback;

private:
	// Define the middle class used to update the actual Bar using player units
	// events
	class BarUpdater : public PlayerCallback {
	public:
		BarUpdater();
		~BarUpdater();
		/**
		 * The functor virtual function (receive messages when the player's life
		 * changes)
		 */
		virtual void operator()(int);

		// members
		LoadingBar			loadingBar;
		PlayerUnit			*unit;
		LoadingBar			*bigBar;
	};

public:
	PlayerStatus();
	virtual ~PlayerStatus();

	/**
	 * Assign the OverlayContainer to use (panel) to retrieve the LoadingBar
	 * panel and use it.
	 * @param 	pu		The player unit to show the life
	 * @param	oc		OverlayContainer to use, it should have a container
	 * 					with the substring name "LifeBar".
	 * @param	cb		The callback to be used for advise when the item is
	 * 					selected
	 * @param	bigB	The Big Bar (or global bar) to be handled by this unit.
	 */
	void configure(PlayerUnit *gu, Ogre::OverlayContainer *oc, Callback *cb,
			LoadingBar *bigB);

	/**
	 * Getters functions
	 */
	inline PlayerUnit *getPlayerUnit(void);
	inline Callback *getCallback(void);

	/**
	 * Functions used to show/hide the element
	 */
	void show(void);
	void hide(void);

	/**
	 * Advise to this playerStatus that is the "one" to show the Main Life bar
	 */
	void useMainLifeBar(void);

	/**
	 * Function to set the PlayerStatusImage as selected or normal
	 */
	inline void setSelected(bool sel);


protected:
	/**
	 * When the button is pressed with right button (after is released)
	 */
	virtual void rightPressed(void);

	/**
	 * When the button is left Pressed
	 */
	virtual void leftPressed(void);

private:
	Callback			*mCallback;
	BarUpdater			mBarUpdater;

};



inline PlayerUnit *PlayerStatus::getPlayerUnit(void)
{
	return mBarUpdater.unit;
}
inline PlayerStatus::Callback *PlayerStatus::getCallback(void)
{
	return mCallback;
}

inline void PlayerStatus::setSelected(bool sel)
{
	if(sel) setActionType(ACTION_SELECTED);
	else setActionType(ACTION_NORMAL);
}



#endif /* PLAYERSTATUS_H_ */
