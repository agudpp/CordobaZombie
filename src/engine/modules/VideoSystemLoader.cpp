/*
 * VideoSystemLoader.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#include "VideoSystemLoader.h"

#include <debug/DebugUtil.h>

namespace engine {

VideoSystemLoader::VideoSystemLoader(mm::VideoPlayer*& videoPlayer) :
    IModuleLoader("VideoSystemLoader")
,   mVideoPlayer(videoPlayer)
{

}

VideoSystemLoader::~VideoSystemLoader()
{
}

////////////////////////////////////////////////////////////////////////////
// Inherited methods
//

////////////////////////////////////////////////////////////////////////////
bool
VideoSystemLoader::load(const EngineConfiguration& config)
{
    if (mVideoPlayer != 0) {
        debugWARNING("Video player instance already created\n");
        return true;
    }

    // build the instance
    debugERROR("TODO: here we need to build the instance and configure it using "
        "whatever we need from the config (we need to add these information in"
        " the config file)\n");

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
VideoSystemLoader::unload(void)
{
    debugERROR("TODO: destroy the instance and uninitialize whatever we need\n");

    // after destroying reset the pointer.
    mVideoPlayer = 0;

    return true;
}

} /* namespace engine */
