/*
 * SlideOverlayEffect.cpp
 *
 *  Created on: Nov 28, 2013
 *      Author: agustin
 */

#include "SlideOverlayEffect.h"


// internal structures
namespace {

float fun_lineal_vel(float x)
{
    return x;
}

}


namespace ui {

const SlideOverlayEffect::VelFunction SlideOverlayEffect::LINEAL_FUNCTION = fun_lineal_vel;



////////////////////////////////////////////////////////////////////////////////
SlideOverlayEffect::SlideOverlayEffect() :
    mFun(&LINEAL_FUNCTION)
{

}
////////////////////////////////////////////////////////////////////////////////
SlideOverlayEffect::~SlideOverlayEffect()
{
}

////////////////////////////////////////////////////////////////////////////////
void
SlideOverlayEffect::beforeStart(void)
{
    ASSERT(overlayElement());
    mActualPos = mOrig;
    mElement->setPosition(mOrig.x, mOrig.y);
    mAccumTime = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////
bool
SlideOverlayEffect::update(float timeFrame)
{
    mAccumTime += timeFrame;

    // Get the time percentage lapsed to pass to the velocity function
    const Ogre::Real tPercent = mAccumTime*mTimeLapse;

    ASSERT(tPercent >= 0.0f);

    // check if we have finish
    if(tPercent >= 1.0f){
        // set the position at the destiny position
        overlayElement()->setPosition(mDest.x, mDest.y);
        return false;
    }

    // we have to continue moving... get the postion percent and move
    float xPercent = tPercent;
    if(mFun){
        xPercent = (*mFun)(tPercent);
    }
    mActualPos = mOrig + mTransVec * xPercent;

    // move the element
    overlayElement()->setPosition(mActualPos.x, mActualPos.y);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
SlideOverlayEffect::afterFinish(void)
{
    // nothing to do
}

////////////////////////////////////////////////////////////////////////////////
bool
SlideOverlayEffect::complement()
{
    core::Vector2 tmp(mOrig);
    mOrig = mDest;
    mDest = tmp;
    mTransVec = mDest-mOrig;
    return true;
}

} /* namespace ui */
