/*
 * MainMenuHistoryState.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: agustin
 */

#include "MainMenuHistoryState.h"

#include <functional>
#include <sstream>

#include <OgreOverlayManager.h>
#include <OgreMaterialManager.h>

#include <effect_handler/EffectHandler.h>
#include <frontend/FEManager.h>
#include <game_states/states/MenuMainState/helper/MainMenuHelper.h>
#include <input/InputHelper.h>



// useful defines
//
#define HISTORYSTATE_OVERLAY_NAME       "HistoryMenu"

namespace {

static const char* BUTTONS_NAME[] = {
    "MainMenu/History/Next",
    "MainMenu/History/Prev",
    "MainMenu/History/Back",
};

static const char* BASE_SLIDE_NAME = "HistoryMenu/Slide";

}

namespace cz {

////////////////////////////////////////////////////////////////////////////////
void
MainMenuHistoryState::getSlides(SlidePlayer::SlidesVec& slides)
{
    slides.clear();
    Ogre::MaterialManager& mm = Ogre::MaterialManager::getSingleton();

    std::stringstream ss;
    unsigned int count = 1;
    for (; count <= slides.max_size(); ++count) {
        ss.clear(); ss.str("");
        ss << BASE_SLIDE_NAME;
        ss << count;
        const std::string& matName = ss.str();
        Ogre::MaterialPtr mat = mm.getByName(matName);
        if (mat.isNull()) {
            break;
        }

        // else we have to add this name to the vector
        slides.push_back(matName);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MainMenuHistoryState::MainMenuHistoryState() :
    mOverlay(0)
,   mRetVal(MainMenuSubStateEvent::MMSSE_CONTINUE)
{
}

////////////////////////////////////////////////////////////////////////////////
MainMenuHistoryState::~MainMenuHistoryState()
{
}

////////////////////////////////////////////////////////////////////////////////
void
MainMenuHistoryState::buttonPressed(ui::FESimpleButton* button,
                                    ui::FESimpleButton::Event event)
{
    ASSERT(mButtons.size() == Buttons::COUNT);

    if (button == &(mButtons[Buttons::BACK])) {
        // we have to go out
        mRetVal = MainMenuSubStateEvent::MMSSE_DONE;
    } else if (button == &(mButtons[Buttons::NEXT_SLIDE])) {
        // move to the next slide
        mSlidePlayer.next();
    } else if (button == &(mButtons[Buttons::PREV_SLIDE])) {
        // move to the prev slide
        mSlidePlayer.prev();
    }
}

////////////////////////////////////////////////////////////////////////////
//                          Inherited methods                             //
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHistoryState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    Ogre::String rcsFile = "2D/menu/historia/resources.cfg";
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
MainMenuHistoryState::load(void)
{
    // now we have to configure the slide player, all the buttons and the slides
    //


    // get the overlay where the buttons are
    mOverlay = Ogre::OverlayManager::getSingleton().getByName(HISTORYSTATE_OVERLAY_NAME);
    if (mOverlay == 0) {
        debugERROR("We couldn't get the overlay " HISTORYSTATE_OVERLAY_NAME "\n");
        return false;
    }

    // buttons
    mButtons.resize(Buttons::COUNT);
    for (unsigned int i = 0; i < Buttons::COUNT; ++i) {
        // configure the button
        if (!MainMenuHelper::configureButton(mButtons[i], mOverlay, BUTTONS_NAME[i])) {
            debugERROR("Error configuring button %s\n", BUTTONS_NAME[i]);
            return false;
        }

        // set the callback
        mButtons[i].setButtonPressedCallback(
            std::bind(&MainMenuHistoryState::buttonPressed,
                      this,
                      std::placeholders::_1,
                      std::placeholders::_2));
    }

    // now configure the slide player
    SlidePlayer::Info info;
    info.slideTime = 1.f;
    info.effectHandler = sCommonHandlers.effectHandler;
    if (!mSlidePlayer.configure(info)) {
        debugERROR("Error configuring the slide player\n");
        return false;
    }

    // set the slides, we will try to get all the slides starting from
    // BASE_SLIDE_NAME + 1... BASE_SLIDE_NAME + N. Until we check some
    // material doesn't exists
    //
    SlidePlayer::SlidesVec slides;
    getSlides(slides);
    if (slides.empty()) {
        debugERROR("No slides found with base name %s\n", BASE_SLIDE_NAME);
        return false;
    }
    // set the slides
    if (!mSlidePlayer.setSlides(slides)) {
        debugERROR("Error setting the slides!\n");
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHistoryState::show(void)
{
    // show the buttons
    mOverlay->show();
    for (ui::FESimpleButton& b : mButtons) {
        b.setButtonState(ui::FESimpleButton::State::S_NORMAL);
        sCommonHandlers.frontEndManager->add(&b);
    }

    // reset the slide player to be with the first slide
    mSlidePlayer.reset();
    mSlidePlayer.setVisible(true);

    // we will continue as default
    mRetVal = MainMenuSubStateEvent::MMSSE_CONTINUE;

    return true;
}

////////////////////////////////////////////////////////////////////////////
MainMenuSubStateEvent
MainMenuHistoryState::update(float timeFrame)
{
    // now we have to check if we press escape then we go out
    // we will check until we press escape to go back
    if (sCommonHandlers.inputHelper->isKeyReleased(input::KeyCode::KC_ESCAPE)) {
        return MainMenuSubStateEvent::MMSSE_DONE;
    }

    // check if we want to move the slides using the keyboard
    //
    if (sCommonHandlers.inputHelper->isKeyReleased(input::KeyCode::KC_LEFT)) {
        mSlidePlayer.prev();
    } else if (sCommonHandlers.inputHelper->isKeyReleased(input::KeyCode::KC_RIGHT)) {
        mSlidePlayer.next();
    }

    return mRetVal;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHistoryState::hide(void)
{
    // we have to remove the buttons and hide the overlay
    ASSERT(mOverlay);
    ASSERT(mButtons.size() > 0);
    mOverlay->hide();
    for (ui::FESimpleButton& b : mButtons) {
        sCommonHandlers.frontEndManager->remove(&b);
    }

    mSlidePlayer.setVisible(false);

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHistoryState::unload(void)
{
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

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuHistoryState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    if (!getResourcesToLoad(resourceList)) {
        debugERROR("Calling from here!\n");
        return false;
    }
    return true;
}

} /* namespace cz */
