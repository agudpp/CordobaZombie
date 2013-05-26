/*
 * MainState.cpp
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#include "MainState.h"

#include <OgreOverlay.h>
#include <OgreOverlayManager.h>

#include "DebugUtil.h"
#include "SoundManager.h"

namespace mm_states {

const char *MainState::BUTTONS_NAMES[NUMBER_BUTTONS] =  {
        "NewGameButton",
        "HistoryButton",
        "CreditsButton",
        "ConfigButton",
        "ExitButton"
};


////////////////////////////////////////////////////////////////////////////////
void
MainState::exitState(void)
{
     for(size_t i = 0, size = mMenuButtonsEff.size(); i < size; ++i){
        const bool res = mMenuButtonsEff[i].getEffect()->complement();
        ASSERT(res);
        mMenuButtonsEff[i].getEffect()->start();
    }
    mState = Exiting;
}

////////////////////////////////////////////////////////////////////////////////
void
MainState::checkInput(void)
{
    if(isKeyPressed(input::KC_ESCAPE)){
       exitState();
   }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MainState::MainState() :
        IState("MainState"),
        mReturnEvent(Event::Done)
{


}

MainState::~MainState()
{
	unload();
	// we must clear all the menu Buttons
    for(size_t i = 0, size = mMenuButtonsEff.size(); i < size; ++i){
        ASSERT(mMenuButtonsEff[i].getEffect());
        delete mMenuButtonsEff[i].getEffect();
        delete mMenuButtonsEff[i].getButton();
    }
    mMenuButtonsEff.clear();
}


////////////////////////////////////////////////////////////////////////////////
void
MainState::operator()(CbMenuButton *button, CbMenuButton::ButtonID id)
{
    mm::SSerror err(mm::SSerror::SS_NO_ERROR);
    mm::SoundManager& sMgr(mm::SoundManager::getInstance());

    ASSERT(mMenuButtonsEff.size() == NUMBER_BUTTONS);
    if (mState == Exiting) {
    	// Someone already pressed an escape button, ignore following commands.
    	return;
    }

    if (button == mMenuButtonsEff[NewGameButtonIndex].getButton()){
        mReturnEvent = Event::PlayGame;
        sMgr.stopEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));
    	err = sMgr.playEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));

    } else if (button == mMenuButtonsEff[HistoryButtonIndex].getButton()) {
        mReturnEvent = Event::History;
        sMgr.stopEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));
        err = sMgr.playEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));

    } else if (button == mMenuButtonsEff[CreditsButtonIndex].getButton()) {
        mReturnEvent = Event::Credits;
        sMgr.stopEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));
        err = sMgr.playEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));

    } else if (button == mMenuButtonsEff[ConfigButtonIndex].getButton()) {
        mReturnEvent = Event::Config;
        sMgr.stopEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));
        err = sMgr.playEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));

    } else if (button == mMenuButtonsEff[ExitButtonIndex].getButton()) {
        mReturnEvent = Event::Exit;
        sMgr.stopEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));
        err = sMgr.playEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));

    } else {
        debugERROR("Receiving events from an invalid button!!!\n");
        ASSERT(false);
    }

    ASSERT(err == mm::SSerror::SS_NO_ERROR);
    exitState();
}

////////////////////////////////////////////////////////////////////////////////
void
MainState::load(void)
{
    // check if we had buttons already loaded
    if (!mMenuButtonsEff.empty()){
        // buttons cached!
        return;
    }
    // build the buttons
    std::vector<Ogre::String> buttonNames;
    buttonNames.reserve(NUMBER_BUTTONS);
    for(size_t i = 0; i < NUMBER_BUTTONS; ++i){
        buttonNames.push_back(BUTTONS_NAMES[i]);
    }
    buildButtons(mMenuButtonsEff, buttonNames);

	// Also load the sounds for this state from the config.xml file
	getSoundsFromXML();
}

////////////////////////////////////////////////////////////////////////////////
void
MainState::beforeUpdate(void)
{
    // show the overlay
    Ogre::Overlay *overlay = Ogre::OverlayManager::getSingleton().getByName(
            "MainMenu/MainState");
    ASSERT(overlay);
    overlay->show();

    // reproduce the effect of all the buttons
    for(size_t i = 0, size = mMenuButtonsEff.size(); i < size; ++i){
        ASSERT(mMenuButtonsEff[i].getEffect());
        mMenuButtonsEff[i].getEffect()->start();
        mMenuButtonsEff[i].getEffect()->getElement()->show();
    }

	// Start the background music in looping mode
    mm::SSerror err = mm::SoundManager::getInstance().playEnvSound(
			*mSounds.getSound(SS_BACKGROUND_MUSIC),	// Music filename
			BACKGROUND_MUSIC_VOLUME,				// Playback volume
			true);									// Looping activated
	ASSERT(err == mm::SSerror::SS_NO_ERROR);

    mState = Looping;
    mReturnEvent = Event::Done;
}

////////////////////////////////////////////////////////////////////////////////
void
MainState::update(void)
{
    if (mState == Looping){
        // check input here only
        checkInput();
    } else if (mState == Exiting){
        // check if the buttons get hide
        ASSERT(!mMenuButtonsEff.empty());
        if (mMenuButtonsEff[0].getEffect()->isActive()){
            return;
        }

        // some button stop been active, so we have to exit
        for(size_t i = 0, size = mMenuButtonsEff.size(); i < size; ++i){
            mMenuButtonsEff[i].getButton()->setEnable(false);
        }
        stateFinish(mReturnEvent);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
MainState::unload(void)
{
    // complement the effects again
    for(size_t i = 0, size = mMenuButtonsEff.size(); i < size; ++i){
        mMenuButtonsEff[i].getEffect()->complement();
        mMenuButtonsEff[i].getButton()->resetAtlas();
    }

	// Stop the background music
    mm::SSerror err = mm::SoundManager::getInstance().stopEnvSound(
			*mSounds.getSound(SS_BACKGROUND_MUSIC));
	ASSERT(err == mm::SSerror::SS_NO_ERROR);
}


}
