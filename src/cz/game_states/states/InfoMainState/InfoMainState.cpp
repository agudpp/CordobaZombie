/*
 * InfoMainState.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#include "InfoMainState.h"

#include <string>

#include <OgreOverlayManager.h>

#include <debug/DebugUtil.h>
#include <effect_handler/EffectHandler.h>
#include <ResourceHandler.h>


// Local defines
//
#define INFOMAINSTATE_RC_PATH           "2D/sponsors/resources.cfg"
#define INFOMAINSTATE_OVERLAY_NAME      "InfoMainState"
#define INFOMAINSTATE_EFFECT_TIME       1.f
#define INFOMAINSTATE_TIME_SLIDE_SECS   4.f


namespace cz {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
InfoMainState::InfoMainState() :
    mOverlay(0)
,   mAccumTime(0)
{

}

////////////////////////////////////////////////////////////////////////////////
InfoMainState::~InfoMainState()
{
}

////////////////////////////////////////////////////////////////////////////
//                           Inherited methods
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
const MainStateID&
InfoMainState::ID(void) const
{
    static MainStateID id(MainStateID::InfoState);
    return id;
}

////////////////////////////////////////////////////////////////////////////////
bool
InfoMainState::configureState(const MainStateInformation& info)
{
    // we have nothing to do here.
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
InfoMainState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    // get the full resource path of the folder of this state
    std::string fullPath;
    if (!sRcHandler->getResourceFullPath(INFOMAINSTATE_RC_PATH, fullPath)) {
        debugERROR("Error getting the path " INFOMAINSTATE_RC_PATH " \n");
        return false;
    }

    rrh::ResourceGroup rg;
    rg.setOgreResourceFile(fullPath);
    resourceList.push_back(rg);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
InfoMainState::readyToGo(void)
{
    // in any case we want to continue running normally
    mEventInfo = MainStateEvent::EVENT_DONE;

    // get the overlay and each of the slides to be shown
    mOverlay = Ogre::OverlayManager::getSingleton().getByName(INFOMAINSTATE_OVERLAY_NAME);
    if (mOverlay == 0) {
        debugERROR("Error getting the overlay " INFOMAINSTATE_OVERLAY_NAME "\n");
        return false;
    }

    mOverlay->show();
    mSlides.clear();

    // now iterate over all the childrens
    Ogre::Overlay::Overlay2DElementsIterator it = mOverlay->get2DElementsIterator();
    while (it.hasMoreElements()) {
        Ogre::OverlayContainer* cont = it.getNext();
        if (cont == 0) continue;
        mSlides.push_back(cont);
        cont->hide();
    }

    if (mSlides.empty()) {
        debugWARNING("No slides to show?\n");
    } else {
        // show the first one
        mSlides.front()->show();
        mEffect.setOverlayElement(mSlides.front());
    }

    // configure the initial values
    mAccumTime = 0;
    mEffect.setTime(INFOMAINSTATE_EFFECT_TIME);
    mEffect.setTransitionType(ui::AlphaOverlayEffect::Type::FADE_IN);
    mEffect.setLetItHidded(true);
    sCommonHandlers.effectHandler->add(&mEffect);
    mNeedToFade = false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
InfoMainState::update(float timeFrame)
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
    if (mAccumTime < INFOMAINSTATE_TIME_SLIDE_SECS) {
        return true; // we need to wait
    }

    // we finish the time, now we have to check if we have to continue with the
    // next slide, or hide the current one
    if (!mNeedToFade) {
        // hide the current one
        mNeedToFade = true;
        return true;
    }

    // need to switch to the next slide
    if (mSlides.empty()) {
    	// no more slides, return
    	return false;
    }
    mNeedToFade = false;
    Ogre::OverlayContainer* cont = mSlides.front();
    cont->hide();
    mSlides.pop_front(); // remove the current one

    if (mSlides.empty()) {
        // we finish here
        return false;
    }

    // else we need to continue;
    cont = mSlides.front();
    cont->show();
    mAccumTime = 0;
    mEffect.setOverlayElement(cont);
    mEffect.complement();
    sCommonHandlers.effectHandler->add(&mEffect);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
InfoMainState::unload(void)
{
    // we have to destroy all the overlays here
    Ogre::OverlayManager::getSingleton().destroy(mOverlay);
    sCommonHandlers.effectHandler->remove(&mEffect);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
InfoMainState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    // return the same things that we load
    return getResourcesToLoad(resourceList);
}

} /* namespace cz */
