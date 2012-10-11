/*
 * ConfigState.cpp
 *
 *  Created on: Sep 16, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#include "ConfigState.h"
#include "DebugUtil.h"
#include <boost/signal.hpp>
#include <boost/bind.hpp>


/**
 * Supported buttons names list:
 *		names of the supported buttons, as they are to be found
 *		inside the XML configuration file (case insensitive)
 */
namespace { /* Unnamed namespace provides internal linkage. */

#define  MAX_NFUN  1  // Maximum # of functions mapped to each button

	// Buttons names
	const char* buttonsNamesList[] = {
			"Back"
	};

	// Actions mapped to each button name (follows buttons names order)
	typedef void (mm_states::ConfigState::*memberFun)(void);
	const memberFun buttonsActionsList[][MAX_NFUN] = {
			{&mm_states::ConfigState::exitConfigState}
	};
}



namespace mm_states {

////////////////////////////////////////////////////////////////////////////////
ConfigState::ConfigState() :
		IState("ConfigState"),  // State name: "ConfigState"
		mPanel(0),
		mPanelEff(0),
		mState(STATE_NONE)
{
	if (!buttonsNamesList || !(*buttonsNamesList)) {
		debugWARNING("ConfigMenu has no buttons\n");
		return;
	}

	/* This code is inefficient as the static vector of the class
	 * is reinitialized each time an object is created.
	 * However, few creations are expected. */
	sButtonsNames.clear();
	for ( uint i=0
		; i < sizeof(buttonsNamesList) / sizeof(buttonsNamesList[0])
		; i++) {

		// Register button name
		sButtonsNames.push_back(buttonsNamesList[i]);

		// Register mapped actions
		actions* acts = new actions;
		for ( uint j=0
			; j < sizeof(buttonsActionsList[i]) / sizeof(buttonsActionsList[i][0])
			; j++) {
			if (buttonsActionsList[i][j]) {
				// If some function was mapped, connect it to the button.
				acts->connect(boost::bind(buttonsActionsList[i][j], this));
			}
		}
		mButtonsActions.push_back(acts);
	}
}


////////////////////////////////////////////////////////////////////////////////
ConfigState::~ConfigState()
{
	// Everything should have been released in the unload() function,
	// nothing left to do here.
}


////////////////////////////////////////////////////////////////////////////////
void
ConfigState::operator()(CbMenuButton *b, CbMenuButton::ButtonID id)
{
	for (uint i=0 ; i < mButtonsEff.size() ; i++) {
		if (b == mButtonsEff[i].getButton()) {
			mButtonsActions[i]->operator()();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
ConfigState::load()
{
	Ogre::Real (*parseReal)(const Ogre::String&, Ogre::Real);
	Ogre::OverlayManager& om(Ogre::OverlayManager::getSingleton());
	Ogre::Overlay* overlay(0);
	OvEff::OverlayEffectBuilder oeb;
	TiXmlElement* img(0);

	if (mState != STATE_NONE) {
		return;
	} else {
		mState = STATE_LOOPING;
	}

	// Create all the buttons, and make them invisible.
	buildButtons(mButtonsEff, sButtonsNames);
	ASSERT(mButtonsEff.size() == sButtonsNames.size());
	for (uint i=0 ; i < mButtonsEff.size() ; i++) {
		mButtonsEff[i].getButton()->setVisible(false);
	}

	// Load the KeyConfig panel, and make it invisible.
	overlay = om.getByName("MainMenu/ConfigState");
	if (!overlay) {
		debugERROR("No \"MainMenu/ConfigState\" field in the "
				"overlays.overlay file \n");
		return;
	}
	mPanel = overlay->getChild("ConfigState/KeyConfig");
	if (!mPanel) {
		debugERROR("No \"KeyConfig\" element in the \"MainMenu/ConfigState\" "
					"field, inside the overlays.overlay file\n");
		return;
	}
	overlay->show();
	mPanel->hide();

	// Read image fading effect from configuration XML file.
	img = getXmlElement();
	if (!img) {
		debugERROR("No MainMenu configurations XML file!.\n");
		return;
	} else {
		img = img->FirstChildElement("Config");
		if (!img) {
			debugERROR("No \"Config\" section in the MainMenu "
						"configurations XML file.\n");
			return;
		}
	}
	mPanelEff = oeb.createOverlayEffect(*img, &Ogre::String("Alpha"));

	// Finally map the KeyConfig panel to the effect.
	mPanelEff->setElement(mPanel);

	return;
}


////////////////////////////////////////////////////////////////////////////////
void
ConfigState::beforeUpdate()
{
	ASSERT(mPanel);
	ASSERT(mPanelEff);

	// Display the KeyConfig panel and buttons via their effects.
	mPanelEff->start();
	for (uint i=0 ; i < mButtonsEff.size() ; i++) {
		mButtonsEff[i].getEffect()->start();
	}
}


////////////////////////////////////////////////////////////////////////////////
void
ConfigState::update()
{
	if (mState == STATE_LOOPING) {
		checkInput();
	} else if (mState != STATE_EXITING) {
		return;  // Nothing to do
	}
	for (uint i=0 ; i < mButtonsEff.size() ; i++) {
		if (mButtonsEff[i].getEffect()->isActive()) {
			return;  // Can't quit yet
		}
	}
	if (mPanelEff->isActive()) {
		return;  // Can't quit yet
	}
	// Everything's over now, and we've lost.
	stateFinish(mm_states::Event::Done);
}


////////////////////////////////////////////////////////////////////////////////
void
ConfigState::exitConfigState()
{
	bool success(false);
	mState = STATE_EXITING;
	for (uint i=0 ; i < mButtonsEff.size() ; i++) {
		// Hide buttons
		success = mButtonsEff[i].getEffect()->complement();
		ASSERT(success);
		mButtonsEff[i].getEffect()->start();
	}
	// Hide KeyConfig panel
	success = mPanelEff->complement();
	ASSERT(success);
	mPanelEff->start();
}


////////////////////////////////////////////////////////////////////////////////
void
ConfigState::unload()
{
	ASSERT(mState == STATE_EXITING);
	ASSERT(sButtonsNames.size() == mButtonsEff.size());
	ASSERT(sButtonsNames.size() == mButtonsActions.size());

	for (uint i=0 ; i < mButtonsEff.size() ; i++) {
		delete mButtonsEff[i].getButton();
		delete mButtonsEff[i].getEffect();
		delete mButtonsActions[i];
	}
	mButtonsEff.clear();
	mButtonsActions.clear();
	sButtonsNames.clear();

	delete mPanelEff;
	delete mPanel;
}


////////////////////////////////////////////////////////////////////////////////
void
ConfigState::checkInput()
{
    if(isKeyPressed(input::KC_ESCAPE)) {
        exitConfigState();
    }
}


}
