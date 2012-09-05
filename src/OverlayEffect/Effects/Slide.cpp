/*
 * Slide.cpp
 *
 *  Created on: 23/08/2012
 *      Author: agustin
 *
 */

#include "Slide.h"

#include "DebugUtil.h"

namespace OvEff {

Slide::Slide()
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
}

////////////////////////////////////////////////////////////////////////////////
void Slide::update(void)
{
	// get first the x position in the translation function
	const Ogre::Real xTrans = (mActualPos - mOrig).squaredLength() * mInvTotalSqrLen;
	ASSERT(xTrans >= 0.0f);

	// check if we have finish
	if(xTrans >= 1.0f){
		// set the position at the destiny position
		mElement->setPosition(mDest.x, mDest.y);
		stop();
		return;
	}

	// we have to continue moving... get the velocity
	mActualPos += mTransVec * mFun(xTrans);

	// move the element
	mElement->setPosition(mActualPos.x, mActualPos.y);
}

////////////////////////////////////////////////////////////////////////////////
void Slide::exit(void)
{
	// we dont do nothing...
}

}
