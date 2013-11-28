/*
 * MainMenuHelpState.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: agustin
 */

#include "MainMenuHelpState.h"

#include <frontend/FEManager.h>
#include <game_states/states/MenuMainState/helper/MainMenuHelper.h>
#include <input/InputHelper.h>


// Some internal defines
//
#define HELPMENU_OVERLAY    "HelpMenu"
#define HELPMENU_BACK_BTN   "MainMenu/Help/Back"
#define HELPMENU_INFO_IMG   "MainMenu/Help/Back"


namespace cz {

////////////////////////////////////////////////////////////////////////////
void
MainMenuHelpState::letsGoOut(void)
{
    // now we have to go back to the main state
    mRetEvent = MainMenuSubStateEvent::MMSSE_DONE;
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
MainMenuHelpState::MainMenuHelpState() :
    mOverlay(0)
,   mInfoImg(0)
,   mRetEvent(MainMenuSubStateEvent::MMSSE_CONTINUE)
{

}
////////////////////////////////////////////////////////////////////////////
MainMenuHelpState::~MainMenuHelpState()
{
}

////////////////////////////////////////////////////////////////////////////
void
MainMenuHelpState::buttonPressed(ui::FESimpleButton* button,
                                 ui::FESimpleButton::Event event)
{
    if (event != ui::FESimpleButton::Event::E_RIGHT_PRESSED) {
        // nothing to do
        debugWARNING("Event pressed not valid here %d\n", event);
        return;
    }
    if (button != &mBackButton) {
        debugERROR("Invalid callback call! we only expect the back button!\n");
        return;
    }

    // go out
    letsGoOut();
}

////////////////////////////////////////////////////////////////////////////
//                          Inherited methods                             //
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHelpState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    Ogre::String rcsFile = "2D/menu/ayuda/resources.cfg";
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
MainMenuHelpState::load(void)
{
    // get the overlay
    mOverlay = Ogre::OverlayManager::getSingleton().getByName(HELPMENU_OVERLAY);
    if (mOverlay == 0) {
        debugERROR("Error getting the overlay " HELPMENU_OVERLAY " found\n");
        return false;
    }
    // hide the overlay
    mOverlay->hide();

    // get the info image
    mInfoImg = mOverlay->getChild(HELPMENU_INFO_IMG);
    if (mInfoImg == 0) {
        debugERROR("Information image " HELPMENU_INFO_IMG " not found\n");
        return false;
    }

    // now parse the back button
    if (!MainMenuHelper::configureButton(mBackButton,
                                         mOverlay,
                                         HELPMENU_BACK_BTN)) {
        debugERROR("Error configuring button %s\n", HELPMENU_BACK_BTN);
        return false;
    }

    // set the callback
    mBackButton.setButtonPressedCallback(std::bind(&MainMenuHelpState::buttonPressed,
                                                   this,
                                                   std::placeholders::_1,
                                                   std::placeholders::_2));

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHelpState::show(void)
{
    if (!mOverlay || !mInfoImg) {
        debugERROR("The information wasn't configured!\n");
        return false;
    }
    // show everything
    mOverlay->show();
    mInfoImg->show();

    // add the back button
    mBackButton.setButtonState(ui::FESimpleButton::State::S_NORMAL);
    sCommonHandlers.frontEndManager->add(&mBackButton);

    // set the default values
    mRetEvent = MainMenuSubStateEvent::MMSSE_CONTINUE;

    return true;
}

////////////////////////////////////////////////////////////////////////////
MainMenuSubStateEvent
MainMenuHelpState::update(float timeFrame)
{
    // check the input here
    if (sCommonHandlers.inputHelper->isKeyReleased(input::KeyCode::KC_SPACE)) {
        debugERROR("We need to change this with ESCAPE key\n");
        letsGoOut();
    }

    return mRetEvent;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHelpState::hide(void)
{
    // hide buttons and everything, also remove the button
    ASSERT(mOverlay);
    ASSERT(mInfoImg);
    mOverlay->hide();
    sCommonHandlers.frontEndManager->remove(&mBackButton);
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHelpState::unload(void)
{
    // unload the overlay and remove the button
    ASSERT(mOverlay);
    Ogre::OverlayManager::getSingleton().destroy(mOverlay);
    mOverlay = 0;
    mInfoImg = 0;

    if (sCommonHandlers.frontEndManager->contains(&mBackButton)) {
        sCommonHandlers.frontEndManager->remove(&mBackButton);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHelpState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    Ogre::String rcsFile = "2D/menu/ayuda/resources.cfg";
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
