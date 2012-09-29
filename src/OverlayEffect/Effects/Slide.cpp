/*
 * Slide.cpp
 *
 *  Created on: 23/08/2012
 *      Author: agustin
 *
 */

#include "Slide.h"

#include "DebugUtil.h"
#include "Ogre.h"
#include "GlobalObjects.h"

// internal structures
namespace {
class LinealFun : public OvEff::Slide::VelFunction {
public:
    virtual float operator()(float x) const
    {
        return x;
    }
};
static LinealFun linealFunc;
}


namespace OvEff {

const Slide::VelFunction *Slide::LINEAL_FUNCTION = &linealFunc;

Slide::Slide():
		mFun(0)
{

}

Slide::~Slide()
{

}

////////////////////////////////////////////////////////////////////////////////
void Slide::enter(void)
{
	ASSERT(mElement);
	mActualPos = mOrig;
	mElement->setPosition(mOrig.x, mOrig.y);
	mAccumTime = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////
void Slide::update(void)
{

	mAccumTime += GLOBAL_TIME_FRAME;

	// Get the time percentage lapsed to pass to the velocity function
	const Ogre::Real tPercent = mAccumTime*mTimeLapse;

	ASSERT(tPercent >= 0.0f);

	// check if we have finish
	if(tPercent >= 1.0f){
		// set the position at the destiny position
		mElement->setPosition(mDest.x, mDest.y);
		stop();
		return;
	}

	// we have to continue moving... get the postion percent and move
	float xPercent = tPercent;
	if(mFun){
		xPercent = (*mFun)(tPercent);
	}
	mActualPos = mOrig + mTransVec * xPercent;

	// move the element
	mElement->setPosition(mActualPos.x, mActualPos.y);
}

////////////////////////////////////////////////////////////////////////////////
void Slide::exit(void)
{
	// we dont do nothing...
}

}
