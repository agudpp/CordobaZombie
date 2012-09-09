/*
 * CreditsState.cpp
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#include "CreditsState.h"

#include "DebugUtil.h"
#include "InputKeyboard.h"
#include "GlobalObjects.h"

namespace mm_states {


////////////////////////////////////////////////////////////////////////////////
void CreditsState::triggerHidingState(void)
{

}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CreditsState::CreditsState() :
		IState("CreditsState"),
		mState(0)
{
	// configure the names of the buttons
	mBtnNames.push_back("Back");
}

CreditsState::~CreditsState()
{
	const int size = mButtons.size();
	for(int i = 0 ; i < size; ++i){
		delete mButtons[i].getEffect();
		delete mButtons[i].getButton();
	}
}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::operator()(CbMenuButton *b, CbMenuButton::ButtonID id)
{
	ASSERT(false); // read todo
	if(b == mButtons[ButtonIndex::Back].getButton() &&
			id == CbMenuButton::ButtonID::LEFT_BUTTON){
		// we just go to the hiding state
		// TODO

	}
}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::load(void)
{
	// we will load all the buttons
	buildButtons(mButtons, mBtnNames);
	ASSERT(mButtons.size() == mBtnNames.size());

	// reset flags
	mState = 0;
}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::update(IState::VideoState vs)
{
	switch(vs){
	case IState::VideoState::ENTERING:
	case IState::VideoState::EXITING:
		return;
	case IState::VideoState::LOOPING:
	{
		if(mState == 0){
			mState = STATE_SHOWING;
		}

		// Internal states here
		switch(mState){
		case STATE_SHOWING:
		{
			// reproduce all the effects in all the buttons
			const int size = mButtons.size();
			for(int i = 0; i < size; ++i) {mButtons[i].getEffect()->start();}

			// TODO: reproduce the sounds (fade in) here
			// TODO: show credits
			++mState;
		}
		break;
		case STATE_LOOP:
		{
			// here we have all the buttons ready and we start to show the
			// credits
		}
		break;
		case STATE_HIDING:
		{
			// hiding... do nothing (wait  the effect callback)
		}
		break;
		case STATE_EXITING:
		{
			// exiting
			stateFinish(Event::Done);
		}
		break;
		default:
			ASSERT(false);

		}
	}
	default:
		ASSERT(false);
	}
}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::unload(void)
{
	const int size = mButtons.size();
	for(int i = 0 ; i < size; ++i){
		delete mButtons[i].getEffect();
		delete mButtons[i].getButton();
	}
	mButtons.clear();
}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::keyPressed(input::KeyCode key)
{
	ASSERT(false);// todo : terminar
	if(key == input::KC_ESCAPE){
		// we have to enter to the exit state.
	}
}

}
