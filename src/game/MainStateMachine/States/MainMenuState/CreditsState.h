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
#include <OgreVector2.h>

#include <vector>

#include "IState.h"
#include "CbMenuButton.h"
#include "MenuButtonEffect.h"
#include "Slide.h"
#include "EffectRestarter.h"




namespace mm_states {

class CreditsState : public IState {
public:
	CreditsState();
	virtual ~CreditsState();


	/**
	 * Callback that receives the CbMenuButton and the Button id:
	 * In this case we have not buttons, so do nothing
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

private:
	/**
	 * Trigger the hiding state
	 */
	void triggerHidingState(void);

	/**
	 * Check input
	 */
	void checkInput(void);


private:

	Ogre::Overlay		*mOverlay;
	Ogre::TextAreaOverlayElement *mTextArea;
	f_e::Slide mSlideEffect;
	Ogre::Vector2 mBeginPos;
	f_e::EffectRestarter mRestarter;


};

}

#endif /* CREDITSSTATE_H_ */
