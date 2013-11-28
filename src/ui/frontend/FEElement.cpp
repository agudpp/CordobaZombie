/*
 * FEElement.cpp
 *
 *  Created on: Oct 28, 2013
 *      Author: agustin
 */
#include "FEElement.h"

#include <debug/DebugUtil.h>

#include "FEManager.h"

namespace ui {

void
FEElement::setDimensions(const core::AABB& size)
{
    if (mManager && mManager->contains(this)) {
        mManager->updateDimensions(this, size);
    } else {
        // we simply set the dims
        mAABB = size;
    }
}

void
FEElement::setActive(bool active)
{
    if (active == mIsActive) {
        return;
    }
    if (mManager) {
        mManager->setActive(this, active);
    } else {
        // we simply set the flag
        mIsActive = active;
    }
}

}
