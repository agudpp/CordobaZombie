/*
 * SlidePlayer.cpp
 *
 *  Created on: Nov 29, 2013
 *      Author: agustin
 */

#include "SlidePlayer.h"

#include <cmath>

#include <OgreOverlayManager.h>

#include <debug/DebugUtil.h>
#include <effect_handler/EffectHandler.h>


// useful Macro
#define CHECK_BETWEEN(val,min,max) ((val <= max) && (val >= min))


// helper stuff
namespace {

static float
sin_vel_fun(float x)
{
    x *= 3.1415f;
    return (std::sin(x-(M_PI/2.0f))+1.0f)/2.0f;
}

}


namespace cz {

const ui::SlideOverlayEffect::VelFunction SlidePlayer::SIN_FUN = sin_vel_fun;

////////////////////////////////////////////////////////////////////////////
bool
SlidePlayer::buildFromOverlay(const Ogre::String& overlayName)
{
    // try to get the overaly
    mConfigOverlay = Ogre::OverlayManager::getSingleton().getByName(overlayName);
    if (mConfigOverlay == 0) {
        debugERROR("Overlay %s not found\n", overlayName.c_str());
        return false;
    }

    // get each of the slides
    mCenterSlide = mConfigOverlay->getChild(overlayName + "/CenterSlide");
    mPrevSlide = mConfigOverlay->getChild(overlayName + "/PrevSlide");
    mNextSlide = mConfigOverlay->getChild(overlayName + "/NextSlide");
    if (mCenterSlide == 0 || mPrevSlide == 0 || mNextSlide == 0) {
        debugERROR("We couldn't found the containers needed to work!\n");
        return false;
    }
    static char count = '0';
    mHiddenSlide = static_cast<Ogre::OverlayContainer*>(
        mCenterSlide->clone(overlayName + "hidden" + count));
    ++count;

    // configure the containers
    mConfigOverlay->add2D(mHiddenSlide);
    mCenterSlide->hide();
    mPrevSlide->hide();
    mNextSlide->hide();
    mHiddenSlide->hide();

    return true;
}

////////////////////////////////////////////////////////////////////////////
void
SlidePlayer::init(void)
{
    ASSERT(mCenterSlide);
    ASSERT(mPrevSlide);
    ASSERT(mNextSlide);
    ASSERT(mHiddenSlide);

    if (mSlides.empty()) {
        debugWARNING("No slides to configure?\n");
        return;
    }

    if (mSlides.size() > 0) {
        mCenterSlide->setMaterialName(mSlides[0]);
        mCenterSlide->show();
    }
    if (mSlides.size() > 1) {
        mNextSlide->setMaterialName(mSlides[1]);
        mNextSlide->show();
    }

    mPrevSlide->setMaterialName(mSlides[0]);
    mHiddenSlide->setMaterialName(mSlides[0]);

    mCurrentSlide = 0;
}

////////////////////////////////////////////////////////////////////////////
void
SlidePlayer::configureEffects(void)
{
    mPrevEffect.setOverlayElement(mPrevSlide);
    mCenterEffect.setOverlayElement(mCenterSlide);
    mNextEffect.setOverlayElement(mNextSlide);
    mHiddenEffect.setOverlayElement(mHiddenSlide);

    //And start effects
    ASSERT(mInfo.effectHandler);
    mInfo.effectHandler->add(&mPrevEffect);
    mInfo.effectHandler->add(&mCenterEffect);
    mInfo.effectHandler->add(&mNextEffect);
    mInfo.effectHandler->add(&mHiddenEffect);
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
SlidePlayer::SlidePlayer() :
    mCurrentSlide(0)
,   mConfigOverlay(0)
,   mCenterSlide(0)
,   mPrevSlide(0)
,   mNextSlide(0)
,   mHiddenSlide(0)
,   mMoveDir(MoveDir::NONE)
{

}

////////////////////////////////////////////////////////////////////////////
SlidePlayer::~SlidePlayer()
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
    if (mHiddenSlide) {
        om.destroyOverlayElement(mHiddenSlide);
    }
    // destroy mConfigOverlay
    if (mConfigOverlay) {
        om.destroy(mConfigOverlay);
    }
}

////////////////////////////////////////////////////////////////////////////
bool
SlidePlayer::configure(const SlidePlayer::Info& info)
{
    if (info.effectHandler == 0 || info.velFun == 0) {
        debugERROR("Invalid arguments\n");
        return false;
    }

    mInfo = info;

    // set the function to the effects
    mCenterEffect.setFunction(info.velFun);
    mPrevEffect.setFunction(info.velFun);
    mNextEffect.setFunction(info.velFun);
    mHiddenEffect.setFunction(info.velFun);

    // set the duration
    mCenterEffect.setDuration(info.slideTime);
    mPrevEffect.setDuration(info.slideTime);
    mNextEffect.setDuration(info.slideTime);
    mHiddenEffect.setDuration(info.slideTime);

    if (!buildFromOverlay(info.configOverlayName)) {
        debugERROR("Error building from overlay %s\n", info.configOverlayName.c_str());
        return false;
    }

    // configure the positions
    mCenterTL = core::Vector2(mCenterSlide->getLeft(), mCenterSlide->getTop());
    mPrevTL = core::Vector2(mPrevSlide->getLeft(), mPrevSlide->getTop());
    mNextTL = core::Vector2(mNextSlide->getLeft(), mNextSlide->getTop());
    mHiddenPrevTL = core::Vector2(mPrevSlide->getLeft() -
                                  (mCenterSlide->getLeft() - mPrevSlide->getLeft()),
                                  mPrevSlide->getTop());
    mHiddenNextTL = core::Vector2(mNextSlide->getLeft() *
                                  2.0f - mCenterSlide->getLeft(),
                                  mNextSlide->getTop());

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
SlidePlayer::setSlides(const SlidesVec& slides)
{
    if (slides.empty()) {
        debugWARNING("No slides were set... empty vector\n");
        return true;
    }

    // set the slides
    mSlides.clear();
    mSlides = slides;

    init();

    return true;
}


////////////////////////////////////////////////////////////////////////////
//  Handling the slide player
//

bool
SlidePlayer::next(void)
{
    if (mCurrentSlide >= (mSlides.size()-1)) {
        return false;
    }

    if (!canMove()) {
        // we need to wait
        return true;
    }

    // configure slide effects positions
    ++mCurrentSlide;

    // check if we have to update the effects
    if (mMoveDir != MoveDir::FORWARD) {
        // Hidden (right) slide will become visible
        mHiddenEffect.setTranslationPositions(mHiddenNextTL, mNextTL);
        // left slide will be hidden now
        mPrevEffect.setTranslationPositions(mPrevTL, mHiddenPrevTL);
        // center slide will be moved to the left now
        mCenterEffect.setTranslationPositions(mCenterTL, mPrevTL);
        // Next slide will move to the center
        mNextEffect.setTranslationPositions(mNextTL, mCenterTL);

        mMoveDir = MoveDir::FORWARD;
    }

    // check if we can show more slides
    if ((mCurrentSlide+1) < mSlides.size()) {
        // the hidden slide could be shown so we have to set the new material
        mHiddenSlide->setMaterialName(mSlides[mCurrentSlide+1]);
        mHiddenSlide->show();
    } else {
        // If we don't have more slides
        mHiddenSlide->hide();
    }

    // configure the effects
    configureEffects();

    // update the pointers for the next call
    Ogre::OverlayContainer* aux = mPrevSlide;
    mPrevSlide = mCenterSlide;
    mCenterSlide = mNextSlide;
    mNextSlide = mHiddenSlide;
    mHiddenSlide = aux;
    aux = 0;


    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
SlidePlayer::prev(void)
{
    if (mCurrentSlide == 0) {
        return false;
    }

    if (!canMove()) {
        // we need to wait
        return true;
    }

    // now configure the next slide
    --mCurrentSlide;

    if (mMoveDir != MoveDir::BACKWARD) {
        // left slide will be move to the center
        mPrevEffect.setTranslationPositions(mPrevTL, mCenterTL);
        // center slide will be moved to left
        mCenterEffect.setTranslationPositions(mCenterTL, mNextTL);
        // right slide will be move to hidden position
        mNextEffect.setTranslationPositions(mNextTL, mHiddenNextTL);
        // hidden slide will be moved to the left position
        mHiddenEffect.setTranslationPositions(mHiddenPrevTL, mPrevTL);

        mMoveDir = MoveDir::BACKWARD;
    }

    // check if we have more slides (in the left slide) to show
    if ((static_cast<int>(mCurrentSlide)-1) < 0) {
        // no we have not
        mHiddenSlide->hide();
    } else {
        mHiddenSlide->setMaterialName(mSlides[mCurrentSlide-1]);
        mHiddenSlide->show();
    }

    // configure the effects
    configureEffects();

    // change the pointers
    Ogre::OverlayContainer* aux = mNextSlide;
    mNextSlide = mCenterSlide;
    mCenterSlide = mPrevSlide;
    mPrevSlide = mHiddenSlide;
    mHiddenSlide = aux;

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
SlidePlayer::seek(unsigned int i)
{
    ASSERT(false && "TODO, not needed right now");
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
SlidePlayer::canMove(void) const
{
    ASSERT(mInfo.effectHandler);
    return !(mInfo.effectHandler->contains(&mCenterEffect) ||
        mInfo.effectHandler->contains(&mNextEffect) ||
        mInfo.effectHandler->contains(&mPrevEffect) ||
        mInfo.effectHandler->contains(&mHiddenEffect));
}


} /* namespace cz */
