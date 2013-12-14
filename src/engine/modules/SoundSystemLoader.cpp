/*
 * SoundSystemLoader.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#include "SoundSystemLoader.h"

#include <debug/DebugUtil.h>

namespace engine {

SoundSystemLoader::SoundSystemLoader(mm::SoundHandler*& soundHandler,
                                     mm::OpenALHandler*& openalHandler) :
    IModuleLoader("SoundSystemLoader")
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
    if (mSoundHandler != 0) {
        debugWARNING("The sound system was already built\n");
    }

    // get the singleton instance for now
    mSoundHandler = &mm::SoundHandler::getInstance();
    debugERROR("TODO: we need to configure the sound system here... reading the"
        " values in the config structure...\n "
        "We need to set the openalHandler also!\n");


    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
SoundSystemLoader::unload(void)
{
    debugERROR("Check if we have to do something here... since it is a singleton "
        "we cannot free its memory\n");

    mSoundHandler = 0;
    return true;
}

} /* namespace engine */
