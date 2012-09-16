/*
 * CreditsState.h
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#ifndef CREDITSSTATE_H_
#define CREDITSSTATE_H_

#include <OgreString.h>
#include <OgreOverlay.h>
#include <OgreTextAreaOverlayElement.h>

#include <vector>

#include "IState.h"
#include "CbMenuButton.h"
#include "MenuButtonEffect.h"


namespace mm_states {

class CreditsState : public IState {

	enum ButtonIndex {
		Back = 0,
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
	CreditsState();
	virtual ~CreditsState();


	/**
	 * Callback that receives the CbMenuButton and the Button id
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

	/**
	 * This function have to be implemented to receive the input events from the
	 * keyboard (for example, if the user press Escape or other key).
	 * The Logic have to be implemented in the state itself and not in the
	 * MainMenuState
	 * @param	key		The keyboard pressed
	 *
	 * @note	We only will call this function with KeyPress event (only once
	 * 			if the user still pressing the key, this function will be called
	 * 			once).
	 */
	void keyPressed(input::KeyCode key);

private:
	/**
	 * Trigger the hiding state
	 */
	void triggerHidingState(void);


private:
	typedef std::vector<OvEff::MenuButtonEffect>	ButtonsEffectVec;
	typedef std::vector<Ogre::String>				StrVec;

	ButtonsEffectVec	mButtons;
	int					mState;
	StrVec				mBtnNames;
	Ogre::Overlay		*mOverlay;
	Ogre::TextAreaOverlayElement *mTextArea;

};

}

#endif /* CREDITSSTATE_H_ */
