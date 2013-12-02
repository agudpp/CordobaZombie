/*
 * MainMenuCreditsState.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: agustin
 */

#include "MainMenuCreditsState.h"

#include <OgreOverlayManager.h>

#include <effect_handler/EffectHandler.h>
#include <input/InputHelper.h>
#include <input/InputKeyboard.h>



// Some useful defines
//
#define CREDITSSTATE_OVERLAY_NAME       "CreditsMenu"
#define CREDITSSTATE_OVERLAY_IMG_CONT   "MainMenu/CreditsMenu/Credits"
#define CREDITSSTATE_SLIDE_TIME_SECS    12.f

namespace cz {

MainMenuCreditsState::MainMenuCreditsState() :
    mOverlay(0)
,   mCreditsCont(0)
{
    mSlideEffect.setDuration(CREDITSSTATE_SLIDE_TIME_SECS);
    mSlideEffect.setFunction(&ui::SlideOverlayEffect::LINEAL_FUNCTION);
}

MainMenuCreditsState::~MainMenuCreditsState()
{
}


////////////////////////////////////////////////////////////////////////////
//                          Inherited methods                             //
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
bool
MainMenuCreditsState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    Ogre::String rcsFile = "2D/menu/creditos/resources.cfg";
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
MainMenuCreditsState::load(void)
{
    // now we need to load the overlay with the image we want to scroll
    mOverlay = Ogre::OverlayManager::getSingleton().getByName(CREDITSSTATE_OVERLAY_NAME);
    if (mOverlay == 0) {
        debugERROR("No overlay found with name " CREDITSSTATE_OVERLAY_NAME "\n");
        return false;
    }

    // now get the image container
    mCreditsCont = mOverlay->getChild(CREDITSSTATE_OVERLAY_IMG_CONT);
    if (mCreditsCont == 0) {
        debugERROR("Credits image not found with name "
            CREDITSSTATE_OVERLAY_IMG_CONT "\n");
        return false;
    }

    // get the origin and destiny positions for the effect
    mOrig.y = 1.f;
    mOrig.x = mCreditsCont->getLeft();

    mDest.x = mCreditsCont->getLeft();
    mDest.y = -mCreditsCont->getHeight() - 0.1;

    // configure the effect
    mSlideEffect.setOverlayElement(mCreditsCont);

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuCreditsState::show(void)
{
    ASSERT(mOverlay);
    ASSERT(mCreditsCont);

    // configure the effect to be reproduced
    mSlideEffect.setTranslationPositions(mOrig, mDest);
    sCommonHandlers.effectHandler->add(&mSlideEffect);

    // show them
    mOverlay->show();
    mCreditsCont->show();

    return true;
}

////////////////////////////////////////////////////////////////////////////
MainMenuSubStateEvent
MainMenuCreditsState::update(float timeFrame)
{
    // we will check until we press escape to go back
    if (sCommonHandlers.inputHelper->isKeyReleased(input::KeyCode::KC_SPACE) ||
        sCommonHandlers.inputHelper->isMouseReleased(input::MouseButtonID::MB_Left)) {
        debugERROR("We need to change this to escape\n");
        return MainMenuSubStateEvent::MMSSE_DONE;
    }

    // check if the effect already finish it
    if (!sCommonHandlers.effectHandler->contains(&mSlideEffect)) {
        // we need to reset the state
        mSlideEffect.setTranslationPositions(mOrig, mDest);
        sCommonHandlers.effectHandler->add(&mSlideEffect);
    }

    // continue normally
    return MainMenuSubStateEvent::MMSSE_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuCreditsState::hide(void)
{
    ASSERT(mOverlay);

    mOverlay->hide();
    sCommonHandlers.effectHandler->remove(&mSlideEffect);

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuCreditsState::unload(void)
{
    // destroy the containers and overlays
    if (mCreditsCont) {
        Ogre::OverlayManager::getSingleton().destroyOverlayElement(mCreditsCont);
        mCreditsCont = 0;
    }
    if (mOverlay) {
        Ogre::OverlayManager::getSingleton().destroy(mOverlay);
        mOverlay = 0;
    }

    // remove the effect just in case
    sCommonHandlers.effectHandler->remove(&mSlideEffect);
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
MainMenuCreditsState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    // we will unload the same resources that we load
    if (!getResourcesToLoad(resourceList)) {
        debugERROR("Error getting the resources to \"load/unload\" from here\n");
        return false;
    }
    return true;
}

} /* namespace cz */
