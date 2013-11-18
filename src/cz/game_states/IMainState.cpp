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
const rrh::ResourceHandler *IMainState::sRcHandler = 0;


IMainState::IMainState()
{
    // we will ensure that we have the ogre information already set
    //
    ASSERT(sOgreInfo.camera);
    ASSERT(sOgreInfo.renderWindow);
    ASSERT(sOgreInfo.root);
    ASSERT(sOgreInfo.sceneMngr);
    ASSERT(sRcHandler);
}

////////////////////////////////////////////////////////////////////////////////
void
IMainState::setOgreData(const OgreCommon& ogreData)
{
    sOgreInfo = ogreData;
}

void
IMainState::setRcHandler(const rrh::ResourceHandler* rch)
{
	sRcHandler = rch;
}



} /* namespace cz */
