/*
 * ConfigState.cpp
 *
 *  Created on: Sep 16, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#include <boost/signal.hpp>
#include <boost/bind.hpp>

#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>

#include "ConfigState.h"
#include "OverlayEffect.h"
#include "OverlayEffectBuilder.h"
#include "DebugUtil.h"
#include "SoundEnums.h"
#include "SoundManager.h"
#include "SoundFamilyTable.h"


/**
 * Supported buttons names list:
 *		names of the supported buttons, as they are to be found
 *		inside the XML configuration file (case insensitive)
 */
namespace { /* Unnamed namespace provides internal linkage. */

#define  MAX_NFUN  1  // Maximum # of functions mapped to each button

	// Buttons names
	const char* buttonsNamesList[] = {
			"ExitButton"
	};

	// Actions mapped to each button name (follows buttons names order)
	typedef void (mm_states::ConfigState::*memberFun)(void);
	const memberFun buttonsActionsList[][MAX_NFUN] = {
			{&mm_states::ConfigState::exitConfigState}
	};
}



namespace mm_states {

ConfigState::StrVec	ConfigState::sButtonsNames;

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
		for (uint j=0 ; j < MAX_NFUN ; j++) {
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
ConfigState::operator()(CbMenuButton *b, CbMenuButton::ButtonID id)
{
    mm::SSerror err(mm::SSerror::SS_NO_ERROR);
    mm::SoundManager& sMgr(mm::SoundManager::getInstance());

    if (mState == STATE_EXITING) {
    	// Someone already pressed an escape button, ignore following commands.
    	return;
    }

	for (uint i=0 ; i < mButtonsEff.size() ; i++) {
		if (b == mButtonsEff[i].getButton()) {
			sMgr.stopEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));
			err = sMgr.playEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));
			ASSERT(err == mm::SSerror::SS_NO_ERROR);
			mButtonsActions[i]->operator()();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
void
ConfigState::load()
{
	Ogre::OverlayManager& om(Ogre::OverlayManager::getSingleton());
	Ogre::Overlay* overlay(0);
	Ogre::String effectName("Alpha");
	f_e::OverlayEffectBuilder oeb;
	const TiXmlElement* img(0);

	if (mState == STATE_LOOPING) {
		debugWARNING("load() called from wrong state\n");
		return;
	} else {
		mState = STATE_LOOPING;
	}

	// Construct the members once, and keep them cached until destruction.
	if (!mButtonsEff.size()) {

		// First load the sounds for this state from the config.xml file
		getSoundsFromXML();

		// Create the buttons, and make them invisible.
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
		mPanel = overlay->getChild("ConfigState/KeyConfigPanel");
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
		}
		img = img->FirstChildElement("KeyConfigPanel");
		if (!img) {
			debugERROR("No \"PanelEffect\" sub-section inside the ConfigState "
						"section of the MainMenu configurations XML file.\n");
			return;
		}
		mPanelEff = oeb.createOverlayEffect(
					*(img->FirstChildElement("OverlayEffect")), &effectName);
		ASSERT(mPanelEff);

		// Finally map the KeyConfig panel to the effect.
		mPanelEff->setElement(mPanel);
	}

	return;
}


////////////////////////////////////////////////////////////////////////////////
void
ConfigState::beforeUpdate()
{
	ASSERT(mPanel);
	ASSERT(mPanelEff);

	// Display the KeyConfig panel and buttons via their effects.
	mPanel->show();
	mPanelEff->start();
	for (uint i=0 ; i < mButtonsEff.size() ; i++) {
		mButtonsEff[i].getButton()->getContainer()->show();
		mButtonsEff[i].getEffect()->start();
	}

	// Start the background music in looping mode.
	mm::SSerror err = mm::SoundManager::getInstance().playEnvSound(
			*mSounds.getSound(SS_BACKGROUND_MUSIC),	// Music filename
			BACKGROUND_MUSIC_VOLUME,				// Playback volume
			true);									// Looping activated
	ASSERT(err == mm::SSerror::SS_NO_ERROR);
}


////////////////////////////////////////////////////////////////////////////////
void
ConfigState::update()
{
	if (mState == STATE_LOOPING) {
		checkInput();

	} else if (mState == STATE_EXITING) {
		for (uint i=0 ; i < mButtonsEff.size() ; i++) {
			if (mButtonsEff[i].getEffect()->isActive()) {
				return;  // Can't quit yet.
			} else {
				mButtonsEff[i].getButton()->setEnable(false);
			}
		}
		if (mPanelEff->isActive()) {
			return;  // Can't quit yet.
		} else {
			mPanel->hide();
		}
		// Everything's over now, and we've lost.
		stateFinish(mm_states::Event::Done);
	}

	return;
}


////////////////////////////////////////////////////////////////////////////////
void
ConfigState::exitConfigState()
{
	bool success(false);

	mState = STATE_EXITING;

	for (uint i=0 ; i < mButtonsEff.size() ; i++) {
		// Hide buttons.
		success = mButtonsEff[i].getEffect()->complement();
		ASSERT(success);
		mButtonsEff[i].getEffect()->start();
	}

	// Hide KeyConfig panel.
	success = mPanelEff->complement();
	ASSERT(success);
	mPanelEff->start();
}


////////////////////////////////////////////////////////////////////////////////
void
ConfigState::unload()
{
	bool success(false);

	ASSERT(mState == STATE_EXITING);
	ASSERT(sButtonsNames.size() == mButtonsEff.size());
	ASSERT(sButtonsNames.size() == mButtonsActions.size());

	// Members memory release was relocated inside the class' destructor.
	// Members are kept cached until destruction.

	for (uint i=0 ; i < mButtonsEff.size() ; i++) {
		// Set buttons to reappear fresh next time we're called.
		success = mButtonsEff[i].getEffect()->complement();
		mButtonsEff[i].getButton()->resetAtlas();
		ASSERT(success);
	}

	// Set KeyConfig panel to reappear next time we're called.
	success = mPanelEff->complement();
	ASSERT(success);

	// Stop the background music.
	mm::SSerror err = mm::SoundManager::getInstance().stopEnvSound(
			*mSounds.getSound(SS_BACKGROUND_MUSIC));
	ASSERT(err == mm::SSerror::SS_NO_ERROR);
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
