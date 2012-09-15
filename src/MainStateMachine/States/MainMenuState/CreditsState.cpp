/*
 * CreditsState.cpp
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#include "CreditsState.h"

#include <OgreOverlayManager.h>
#include <OgreFontManager.h>

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
		mState(0),
		mOverlay(0),
		mTextArea(0)
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
	if(b == mButtons[Back].getButton() &&
			id == CbMenuButton::LEFT_BUTTON){
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

	//
	debugWARNING("UGLY workaround to load all the fonts first.. Move this to the"
			" MainMenu loading lines (where we will load all the resources and "
			"every else\n");
	Ogre::ResourceManager::ResourceMapIterator iter =
			Ogre::FontManager::getSingleton().getResourceIterator();
	while (iter.hasMoreElements()) { iter.getNext()->load(); }

	// hide all
	const int size = mButtons.size();
	for(int i = 0; i < size; ++i) mButtons[i].getButton()->setVisible(false);

	// Load all the overlays
	if(!mOverlay){
		Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
		mOverlay = om.getByName("MainMenu/Credits");
		ASSERT(mOverlay);
		mTextArea = reinterpret_cast<Ogre::TextAreaOverlayElement *>(
				mOverlay->getChild("Credits/Text"));
		mTextArea->setCaption("Esto es una prueba\nMas que una prueba es una copadasa\npepe\npap\nperas\n:)");
		mTextArea->setHorizontalAlignment(Ogre::GHA_CENTER);
		mTextArea->setVerticalAlignment(Ogre::GVA_CENTER);
		ASSERT(mTextArea);
		mOverlay->hide();
	}

	// reset flags
	mState = 0;
}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::update(IState::VideoState vs)
{
	switch(vs){
	case IState::ENTERING:
		return;
	case IState::LOOPING:
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
			debugWARNING("Descomentar la linea de abajo cuando tengamos los efectos\n");
//			for(int i = 0; i < size; ++i) {mButtons[i].getEffect()->start();}

			// TODO: reproduce the sounds (fade in) here
			// TODO: show credits
			ASSERT(mOverlay);
			mOverlay->show();
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
			stateFinish(Done);
		}
		break;
		default:
			ASSERT(false);

		}
	}
	case IState::EXITING:
		return;
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
