/*
 * IMainState.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#include "IMainState.h"

#include <debug/DebugUtil.h>

namespace cz {

OgreCommon IMainState::sOgreInfo;


IMainState::IMainState()
{
    // we will ensure that we have the ogre information already set
    //
    ASSERT(sOgreInfo.camera);
    ASSERT(sOgreInfo.renderWindow);
    ASSERT(sOgreInfo.root);
    ASSERT(sOgreInfo.sceneMngr);
}

////////////////////////////////////////////////////////////////////////////////
void
IMainState::setOgreData(const OgreCommon& ogreData)
{
    sOgreInfo = ogreData;
}

} /* namespace cz */
