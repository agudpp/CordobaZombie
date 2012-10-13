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
    ASSERT(mMenuButtonsEff.size() == NUMBER_BUTTONS);

    if (button == mMenuButtonsEff[NewGameButtonIndex].getButton()){
        mReturnEvent = Event::PlayGame;
        exitState();
    } else if (button == mMenuButtonsEff[HistoryButtonIndex].getButton()) {
        mReturnEvent = Event::History;
        exitState();
    } else if (button == mMenuButtonsEff[CreditsButtonIndex].getButton()) {
        mReturnEvent = Event::Credits;
        exitState();
    } else if (button == mMenuButtonsEff[ConfigButtonIndex].getButton()) {
        mReturnEvent = Event::Config;
        exitState();
    } else if (button == mMenuButtonsEff[ExitButtonIndex].getButton()) {
        mReturnEvent = Event::Exit;
        exitState();
    } else {
        debugERROR("Receiving events from an invalid button!!!\n");
        ASSERT(false);
    }
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
}


}
