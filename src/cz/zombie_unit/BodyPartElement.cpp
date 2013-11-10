/*
 * BodyPartElement.cpp
 *
 *  Created on: Nov 9, 2013
 *      Author: agustin
 */
#include "BodyPartElement.h"

namespace cz {

bool
BodyPartElement::checkImpact(HitInfo& hitInfo) const
{
    debugYELLOW("CALLING BODY PART!!\n");
    // if bullet detect that we hit against this then its true :)
    return true;
}

void
BodyPartElement::processImpactInfo(const HitInfo& hitInfo)
{
    debugERROR("TODO: we can show a blood effect, apply some force to the "
        "bullet object, etc\n");
    debugYELLOW("TODO!\n");
}

}

