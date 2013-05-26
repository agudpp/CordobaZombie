/*
 * ConfigState.h
 *
 *  Created on: Sep 16, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef CONFIGSTATE_H_
#define CONFIGSTATE_H_

#include <vector>
#include <boost/signal.hpp>

#include <OgreString.h>
#include <OgreOverlay.h>

#include "IState.h"
#include "CbMenuButton.h"
#include "MenuButtonEffect.h"
#include "Alpha.h"


namespace mm_states {


/* NOTE:                                                *
 * For a list of the buttons this State supports        *
 * check the "Supported buttons names list" in the .cpp */


class ConfigState : public IState
{
	// Internal state flags
	enum State {
		STATE_NONE = 0,
		STATE_LOOPING,
		STATE_EXITING,
	};

public:
	ConfigState();
	virtual ~ConfigState();

	/**
	 * @brief
	 * Callback when a menu button is pressed.
	 *
	 * @param
	 *  b: Menu's pressed button
	 * id: Mouse button used (left, middle or right)
	 */
	void
	operator()(CbMenuButton *b, CbMenuButton::ButtonID id);

	/**
	 * @brief
	 * Load all the resources used by this state.
	 *
	 * @remarks
	 * This is the first function to be called for this state,
	 * prior even to "beforeUpdate()"
	 */
	void
	load();

	/**
	 * @brief
	 * Final initializations prior to starting the update() loop.
	 *
	 * @remarks
	 * This function should be called after load() and right before
	 * the first invocation of update() in the main loop.
	 */
	void
	beforeUpdate();

	/**
	 * @brief
	 * Frame update for the state's internal status.
	 *
	 * @remarks
	 * This function is called every frame in the MainState main loop.
	 * All the specific state's logic is located here.
	 */
	void
	update();

	/**
	 * @brief
	 * Unload all the resources used by this state.
	 */
	void
	unload();

	/**
	 * @brief
	 * Begin the state's exit animations (i.e., buttons and panel hiding)
	 */
	void
	exitConfigState();

protected:
	/**
	 * @brief
	 * Check for user keyboard event
	 */
	void
	checkInput();

private:
	// FIXME Eclipse won't recognize this "boost preferred" syntax,
	// but it does recognize the "portable" one (see http://goo.gl/V6AeZ)
	typedef boost::signal<void ()>		actions;  // Buttons actions
	typedef std::vector<Ogre::String>	StrVec;
	typedef std::vector<f_e::MenuButtonEffect>  ButtonsEffectVec;
	typedef std::vector<actions*>		ButtonsActionsVec;

	static StrVec			sButtonsNames;	 // Buttons names
	ButtonsActionsVec		mButtonsActions; // Buttons actions
	ButtonsEffectVec		mButtonsEff;	 // Buttons and their visual effects
	Ogre::OverlayContainer*	mPanel;			 // Image to display
	f_e::OverlayEffect*	mPanelEff;		 // Fading effect for the image
	State					mState;
};


}

#endif /* CONFIGSTATE_H_ */
