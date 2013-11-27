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



// Some internal defines
//
#define MENUMAINSTATE_OVERLAY   "MainMenu"

static const char* BUTTONS_NAME[] = {
    "MainMenu/Main/Play",
    "MainMenu/Main/Help",
    "MainMenu/Main/Credits",
    "MainMenu/Main/History",
    "MainMenu/Main/Exit",
};



namespace cz {

MainMenuMainState::MainMenuMainState() :
    mOverlay(0)
,   mRetVal(MainMenuSubStateEvent::MMSSE_CONTINUE)
{
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

    // now check which was the button pressed
    if (button == &(mButtons[Buttons::B_PLAY])) {
        debugGREEN("PLAY GAME PRESSED!\n");
    } else if (button == &(mButtons[Buttons::B_HELP])) {
        debugGREEN("HELP PRESSED!\n");
        // we need to emit this event
        mRetVal = MainMenuSubStateEvent::MMSSE_HELP;
    } else if (button == &(mButtons[Buttons::B_CREDITS])) {
        debugGREEN("CREDITS PRESSED!\n");
    } else if (button == &(mButtons[Buttons::B_HISTORY])) {
        debugGREEN("HISTORY PRESSED!\n");
    } else if (button == &(mButtons[Buttons::B_EXIT])) {
        debugGREEN("EXIT PRESSED!\n");
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
    debugERROR("TODO: Implement this, for now we will return true for test only\n");
    // Here we have to:
    // 1- Destroy the buttons
    // 2- destroy the overlay.
    ASSERT(mOverlay);
    Ogre::OverlayManager::getSingleton().destroy(mOverlay);
    mOverlay = 0;

    for (ui::FESimpleButton& b : mButtons) {
        if (sCommonHandlers.frontEndManager->contains(&b)) {
            sCommonHandlers.frontEndManager->remove(&b);
        }
    }
    // TODO: remove the bind?
    mButtons.clear();

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
    resourceList.push_back(rg);

    return true;
}

} /* namespace cz */
