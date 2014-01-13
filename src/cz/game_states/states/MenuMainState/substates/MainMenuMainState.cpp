/*
 * MainMenuMainState.cpp
 *
 *  Created on: Nov 25, 2013
 *      Author: agustin
 */

#include "MainMenuMainState.h"

#include <functional>

#include <OgreOverlayManager.h>

#include <ResourceGroup.h>
#include <game_states/states/MenuMainState/helper/MainMenuHelper.h>
#include <frontend/FEManager.h>
#include <sound/SoundHandler.h>
#include <sound/SoundManager.h>



// Some internal defines
//
#define MENUMAINSTATE_OVERLAY   "MainMenu"

namespace {

static const char* BUTTONS_NAME[] = {
    "MainMenu/Main/Play",
    "MainMenu/Main/Help",
    "MainMenu/Main/Credits",
    "MainMenu/Main/History",
    "MainMenu/Main/Exit",
};

// define the sounds
static const char* SOUNDS_NAME[] = {
    "fxM1.ogg"
};

static const unsigned int NUM_SOUNDS = sizeof(SOUNDS_NAME) / sizeof(*SOUNDS_NAME);

}



namespace cz {

MainMenuMainState::MainMenuMainState() :
    mOverlay(0)
,   mRetVal(MainMenuSubStateEvent::MMSSE_CONTINUE)
,   mSM(mm::SoundManager::getInstance())
{
    if (!mSM.hasOpenALcontext()) {
        debugWARNING("No sound device set???\n");
    } else {
        for (int i=0 ; i < NUM_SOUNDS ; i++) {
            if (!SOUNDS_NAME[i])
                continue;
            mm::SSerror err = mSM.loadSound(SOUNDS_NAME[i]);
            if (err != mm::SSerror::SS_NO_ERROR)
                debugERROR("Couldn't load sound \"%s\"\n", SOUNDS_NAME[i]);
        }
    }
}

MainMenuMainState::~MainMenuMainState()
{
}

////////////////////////////////////////////////////////////////////////////
void
MainMenuMainState::buttonPressed(ui::FESimpleButton* button,
                                 ui::FESimpleButton::Event event)
{
    if (event != ui::FESimpleButton::Event::E_RIGHT_PRESSED) {
        // nothing to do
        debugWARNING("Event pressed not valid here %d\n", event);
        return;
    }
    if (mButtons.size() < Buttons::B_COUNT) {
        debugERROR("We are calling this method but we haven't built the buttons yet?\n");
        return;
    }
    // Play same sound for every button press
    if (mSM.hasOpenALcontext()) {
        mSM.playEnvSound("fxM1.ogg");
    }

    // play a sound for every button press
    sSoundHandler->soundManager()->playEnvSound("fxM1.ogg", 1.f);

    // now check which was the button pressed
    if (button == &(mButtons[Buttons::B_PLAY])) {
        mRetVal = MainMenuSubStateEvent::MMSSE_PLAY_GAME;
    } else if (button == &(mButtons[Buttons::B_HELP])) {
        // we need to emit this event
        mRetVal = MainMenuSubStateEvent::MMSSE_HELP;
    } else if (button == &(mButtons[Buttons::B_CREDITS])) {
        mRetVal = MainMenuSubStateEvent::MMSSE_CREDITS;
    } else if (button == &(mButtons[Buttons::B_HISTORY])) {
        mRetVal = MainMenuSubStateEvent::MMSSE_HISTORY;
    } else if (button == &(mButtons[Buttons::B_EXIT])) {
        mRetVal = MainMenuSubStateEvent::MMSSE_EXIT_GAME;
    } else {
        ASSERT(false && "We are calling this callback from a button that is not "
            "one of the ones we built!!!");
    }
}

////////////////////////////////////////////////////////////////////////////
//                          Inherited methods                             //
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
bool
MainMenuMainState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    Ogre::String rcsFile = "2D/menu/main/resources.cfg";
    // get the full path
    if (!convertToAbsolute(rcsFile)) {
        debugERROR("Error getting the absolute path for %s\n", rcsFile.c_str());
        return false;
    }

    // set the resource group
    rrh::ResourceGroup rg;
    rg.setOgreResourceFile(rcsFile);
    resourceList.push_back(rg);

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuMainState::load(void)
{
    // get the overlay
    mOverlay = Ogre::OverlayManager::getSingleton().getByName(MENUMAINSTATE_OVERLAY);
    if (mOverlay == 0) {
        debugERROR("Error getting the overlay " MENUMAINSTATE_OVERLAY " found\n");
        return false;
    }
    // hide the overlay
    mOverlay->hide();

    // now parse all the buttons
    mButtons.resize(Buttons::B_COUNT);
    for (unsigned int i = 0; i < Buttons::B_COUNT; ++i) {
        // configure the button
        if (!MainMenuHelper::configureButton(mButtons[i], mOverlay, BUTTONS_NAME[i])) {
            debugERROR("Error configuring button %s\n", BUTTONS_NAME[i]);
            return false;
        }

        // set the callback
        mButtons[i].setButtonPressedCallback(
            std::bind(&MainMenuMainState::buttonPressed,
                      this,
                      std::placeholders::_1,
                      std::placeholders::_2));
    }

    // we will load the sounds here
    //
    ASSERT(sSoundHandler);
    ASSERT(sSoundHandler->soundManager());
    mm::SoundManager* soundMngr = sSoundHandler->soundManager();
    for (unsigned int i = 0; i < NUM_SOUNDS; ++i) {
        if (SOUNDS_NAME[i]) {
            mm::SSerror err = soundMngr->loadSound(SOUNDS_NAME[i]);
            // this we will let it pass, as not critical for now.
            if (err != mm::SSerror::SS_NO_ERROR) {
                debugERROR("Error loading sound %s.\n", SOUNDS_NAME[i]);
            }
        }
    }

    // adding the needed sources
    if (sSoundHandler->addDirectSources(NUM_SOUNDS) != mm::SSerror::SS_NO_ERROR) {
        debugERROR("Error creating directSources (%d)\n", NUM_SOUNDS);
        // this is not critical so we will not abort the execution now.
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuMainState::show(void)
{
    if (!mOverlay || mButtons.empty()) {
        debugERROR("We haven't set the overlay!? or we have no buttons?\n");
        return false;
    }

    // we need to show all the buttons and activate all of them
    mOverlay->show();
    for (ui::FESimpleButton& b : mButtons) {
        b.setButtonState(ui::FESimpleButton::State::S_NORMAL);
        sCommonHandlers.frontEndManager->add(&b);
    }

    mRetVal = MainMenuSubStateEvent::MMSSE_CONTINUE;

    return true;
}

////////////////////////////////////////////////////////////////////////////
MainMenuSubStateEvent
MainMenuMainState::update(float timeFrame)
{
    return mRetVal;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuMainState::hide(void)
{
    // we have to remove the buttons and hide the overlay
    ASSERT(mOverlay);
    ASSERT(mButtons.size() > 0);
    mOverlay->hide();
    for (ui::FESimpleButton& b : mButtons) {
        sCommonHandlers.frontEndManager->remove(&b);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuMainState::unload(void)
{
    // Here we have to:
    // 1- Destroy the buttons
    // 2- destroy the overlay.
    ASSERT(mOverlay);
    // TODO: issue #224 Ogre::OverlayManager::getSingleton().destroy(mOverlay);
    mOverlay->hide();
    mOverlay = 0;

    for (ui::FESimpleButton& b : mButtons) {
        if (sCommonHandlers.frontEndManager->contains(&b)) {
            sCommonHandlers.frontEndManager->remove(&b);
        }
    }
    // TODO: remove the bind?
    mButtons.clear();

    // unload the sounds here
    for (unsigned int i = 0; i < NUM_SOUNDS; ++i) {
        if (SOUNDS_NAME[i] != 0) {
            sSoundHandler->soundManager()->unloadSound(SOUNDS_NAME[i]);
        }
    }


    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuMainState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    Ogre::String rcsFile = "2D/menu/main/resources.cfg";
    // get the full path
    if (!convertToAbsolute(rcsFile)) {
        debugERROR("Error getting the absolute path for %s\n", rcsFile.c_str());
        return false;
    }

    // set the resource group
    rrh::ResourceGroup rg;
    rg.setOgreResourceFile(rcsFile);
//    resourceList.push_back(rg);

    return true;
}

} /* namespace cz */
