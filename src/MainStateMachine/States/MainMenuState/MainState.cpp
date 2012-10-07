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
////////////////////////////////////////////////////////////////////////////////
MainState::MainState() :
        IState("MainState"),
        mReturnEvent(Event::Done)
{


}

MainState::~MainState()
{
	unload();
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
    // we can assume we have no buttons loaded
    if (!mMenuButtonsEff.empty()){
        debugERROR("We are trying to load two times the same buttons?\n");
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
}

////////////////////////////////////////////////////////////////////////////////
void
MainState::update(void)
{
    if (mState == Looping){
        // do nothing... ?
    } else if (mState == Exiting){
        // check if the buttons get hide
        ASSERT(!mMenuButtonsEff.empty());
        if (!mMenuButtonsEff[0].getEffect()->isActive()){
            // some button stop been active, so we have to exit
            stateFinish(mReturnEvent);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void
MainState::unload(void)
{
    // we must clear all the menu Buttons
    for(size_t i = 0, size = mMenuButtonsEff.size(); i < size; ++i){
        ASSERT(mMenuButtonsEff[i].getEffect());
        delete mMenuButtonsEff[i].getEffect();
        delete mMenuButtonsEff[i].getButton();
    }
    debugWARNING("Probably here we are not deleting the resources used by "
            "the button (for example the overlays), and if we are not unloading"
            " this resources in a posterior stage, this will be in the main "
            "gameloop and this isn't right\n");
}

////////////////////////////////////////////////////////////////////////////////
void
MainState::keyPressed(input::KeyCode key)
{
    if(key == input::KC_ESCAPE){
        exitState();
    }
}

}
