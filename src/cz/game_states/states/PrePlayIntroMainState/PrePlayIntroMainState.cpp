/*
 * PrePlayIntroMainState.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#include "PrePlayIntroMainState.h"


#include <OgreOverlayManager.h>

#include <ResourceHandler.h>
#include <effect_handler/EffectHandler.h>




#define PREPLAYINTROSTATE_RC_PAH        "2D/pre_game/resources.cfg"
#define PREPLAYINTROSTATE_OVERLAY       "PrePlayIntroMainState"
#define PREPLAYINTROSTATE_SLIDE_CONT    "PrePlayIntroMainState/Info"
#define PREPLAYINTROSTATE_EFFECT_SECS   1.f
#define PREPLAYINTROSTATE_SHOWTIME_SECS 5.f


namespace cz {

PrePlayIntroMainState::PrePlayIntroMainState() :
    mOverlay(0)
,   mSlide(0)
{

}

PrePlayIntroMainState::~PrePlayIntroMainState()
{

}

////////////////////////////////////////////////////////////////////////////
//                           Inherited methods
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
const MainStateID&
PrePlayIntroMainState::ID(void) const
{
    static MainStateID id = MainStateID::PrePlayingState;
    return id;
}

////////////////////////////////////////////////////////////////////////////////
bool
PrePlayIntroMainState::configureState(const MainStateInformation& info)
{
    // we have nothing to do here.
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PrePlayIntroMainState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    // get the full resource path of the folder of this state
    std::string fullPath;
    if (!sRcHandler->getResourceFullPath(PREPLAYINTROSTATE_RC_PAH, fullPath)) {
        debugERROR("Error getting the path " PREPLAYINTROSTATE_RC_PAH " \n");
        return false;
    }

    rrh::ResourceGroup rg;
    rg.setOgreResourceFile(fullPath);
    resourceList.push_back(rg);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PrePlayIntroMainState::readyToGo(void)
{
    // in any case we want to continue running normally
    mEventInfo = MainStateEvent::EVENT_DONE;

    // get the overlay and each of the slides to be shown
    mOverlay = Ogre::OverlayManager::getSingleton().getByName(PREPLAYINTROSTATE_OVERLAY);
    if (mOverlay == 0) {
        debugERROR("Error getting the overlay " PREPLAYINTROSTATE_OVERLAY "\n");
        return false;
    }

    mOverlay->show();

    // get the image
    mSlide = mOverlay->getChild(PREPLAYINTROSTATE_SLIDE_CONT);
    if (mSlide == 0) {
        debugERROR("Overlay container " PREPLAYINTROSTATE_SLIDE_CONT " not found\n");
        return false;
    }

    // configure the initial values
    mAccumTime = 0;
    mEffect.setTime(PREPLAYINTROSTATE_EFFECT_SECS);
    mEffect.setTransitionType(ui::AlphaOverlayEffect::Type::FADE_IN);
    mEffect.setOverlayElement(mSlide);
    sCommonHandlers.effectHandler->add(&mEffect);
    mNeedToFade = false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PrePlayIntroMainState::update(float timeFrame)
{
    // if we are playing the current effect then we will do nothing
    if (sCommonHandlers.effectHandler->contains(&mEffect)) {
        return true;
    }

    // now check if we was just fading in then we need to fade out first
    if (mNeedToFade &&
        mEffect.transitionType() == ui::AlphaOverlayEffect::Type::FADE_IN) {
        // we need to fade out
        mEffect.complement();
        sCommonHandlers.effectHandler->add(&mEffect);
        return true;
    }

    // now we will be iterating over all the slides until we have no more slides
    mAccumTime += timeFrame;
    if (mAccumTime < PREPLAYINTROSTATE_SHOWTIME_SECS) {
        return true; // we need to wait
    }

    // we finish the time, now we have to check if we have to continue with the
    // next slide, or hide the current one
    if (!mNeedToFade) {
        // hide the current one
        mNeedToFade = true;
        return true;
    }

    // we finish
    return false;
}

////////////////////////////////////////////////////////////////////////////////
bool
PrePlayIntroMainState::unload(void)
{
    // we have to destroy all the overlays here
    Ogre::OverlayManager::getSingleton().destroy(mOverlay);
    sCommonHandlers.effectHandler->remove(&mEffect);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PrePlayIntroMainState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    // return the same things that we load
    if (!getResourcesToLoad(resourceList)) {
        debugERROR("Calling from here!\n");
        return false;
    }
    return true;
}

} /* namespace cz */
