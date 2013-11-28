/*
 * FESimpleButtonEffect.cpp
 *
 *  Created on: Nov 28, 2013
 *      Author: agustin
 */

#include "FESimpleButtonEffect.h"

namespace ui {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
FESimpleButtonEffect::FESimpleButtonEffect() :
    mElement(0)
,   mEffect(0)
{
    // TODO Auto-generated constructor stub

}
////////////////////////////////////////////////////////////////////////////////
FESimpleButtonEffect::~FESimpleButtonEffect()
{
    // TODO Auto-generated destructor stub
}

////////////////////////////////////////////////////////////////////////////////
void
FESimpleButtonEffect::beforeStart(void)
{
    ASSERT(mElement);
    ASSERT(mEffect);

    // dissable the element
    mElement->setActive(false);
    mEffect->setOverlayElement(mElement->overlayContainer());
    mEffect->beforeStart();
}

////////////////////////////////////////////////////////////////////////////////
bool
FESimpleButtonEffect::update(float timeFrame)
{
    // update the effect
    return mEffect->update(timeFrame);
}

////////////////////////////////////////////////////////////////////////////////
void
FESimpleButtonEffect::afterFinish(void)
{
    // finish the effect
    mEffect->afterFinish();

    // activate the element again
    mElement->updateDimensionsFromOverlay();
    mElement->setActive(true);

}

} /* namespace ui */
