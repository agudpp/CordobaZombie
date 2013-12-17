/*
 * SoundSystemLoader.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#include "SoundSystemLoader.h"

#include <debug/DebugUtil.h>
#include <os_utils/OSHelper.h>

namespace engine {

SoundSystemLoader::SoundSystemLoader(mm::SoundHandler*& soundHandler,
                                     rrh::ResourceHandler*& rh,
                                     mm::OpenALHandler*& openalHandler) :
    IModuleLoader("SoundSystemLoader")
,   mResourceHandler(rh)
,   mSoundHandler(soundHandler)
,   mOpenalHandler(openalHandler)
{
}

SoundSystemLoader::~SoundSystemLoader()
{
}

////////////////////////////////////////////////////////////////////////////////
// Inherited methods
//

////////////////////////////////////////////////////////////////////////////////
bool
SoundSystemLoader::load(const EngineConfiguration& config)
{
    // Get full path to the resources (sound files)
    std::string soundsRscPath;
    if (!config.getValue("SoundSystemLoader", "rootRscPath", soundsRscPath)) {
        debugERROR("We couldn't find the path to the sounds folder.\n");
        return false;
    }

    // FIXME: HARDCODED VALUES, SHOULD BE READ FROM XML ///////////////////////
    debugERROR("HARDCODED values here, MUST be read from XML.\n");
    // Check MantisBT issue #296 (http://goo.gl/awV5FF)
    soundsRscPath = mResourceHandler->getResourceRootPath();
    ///////////////////////////////////////////////////////////////////////////

    // Get the SoundHandler singleton instance
    if (mSoundHandler) {
        debugWARNING("The sound system had already been built.\n");
    } else {
        mSoundHandler = &mm::SoundHandler::getInstance();
    }

    debugERROR("TODO: must load all sound files into the SoundSystem.\n" "Find them in the path constructed in \"soundsRscPath\".\n");
    ASSERT(false);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
SoundSystemLoader::unload(void)
{
    debugERROR("Check if we have to do something here... since it is a singleton " "we cannot free its memory\n");

    mSoundHandler = 0;
    return true;
}

} /* namespace engine */
