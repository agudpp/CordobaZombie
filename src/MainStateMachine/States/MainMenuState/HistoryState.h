/*
 * HistoryState.h
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#ifndef HISTORYSTATE_H_
#define HISTORYSTATE_H_


#include <boost/signal.hpp>
#include <vector>

#include <OgreString.h>

#include "SlidePlayer.h"
#include "IState.h"
#include "CbMenuButton.h"
#include "MenuButtonEffect.h"

namespace mm_states {

class HistoryState : public IState {

	enum ButtonIndex {
		Back = 0,
		Prev,
		Next
	};

	// flags
	enum Flags {
		NONE = 0,
		STATE_SHOWING,
		STATE_LOOP ,
		STATE_HIDING,
		STATE_EXITING,
	};

public:
	HistoryState();
	virtual ~HistoryState();

	/**
	 * Callback that receives the CbMenuButton and the mouse Button id
	 */
	void operator()(CbMenuButton *, CbMenuButton::ButtonID id);

	/**
	 * Function called to load all the resources used by the state.
	 * This function is called before we start the entering transition range
	 */
	void load(void);

	/**
	 * Function called right before we start the main loop (update()).
	 * This function is called once and after that we start calling the update
	 * function.
	 */
	void beforeUpdate(void);

	/**
	 * This function is called every frame, here we have to implement all the
	 * state logic.
	 */
	void update(void);
	/**
	 * Function called once the state will be closed, so we have to unload all
	 * the resources used by this class.
	 */
	void unload(void);

	/*
	 * Call back function to let buttons effect tell us when they have done
	 * hiding.
	 */
	void operator()(OvEff::OverlayEffect::EventID id);

private:
	/*
	 * Hide buttons as first step to start exiting from this state. When the
	 * buttons end hiding, a callback is captured to exit the state.
	 */
	void hideToExit(void);

	/**
	 * Check input
	 */
	void checkInput(void);


private:
	typedef std::vector<OvEff::MenuButtonEffect>	ButtonsEffectVec;
	typedef std::vector<Ogre::String>				StrVec;

	ButtonsEffectVec	mButtons;
	StrVec				mBtnNames;
	int					mState;
	SlidePlayer			*mSlidePlayer;
};

}

#endif /* HISTORYSTATE_H_ */
