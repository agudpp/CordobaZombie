/*
 * SoundSystemLoader.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#include "SoundSystemLoader.h"

#include <debug/DebugUtil.h>

namespace engine {

SoundSystemLoader::SoundSystemLoader(mm::SoundManager*& soundManager) :
    IModuleLoader("SoundSystemLoader")
,   mSoundManager(soundManager)
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
    if (mSoundManager != 0) {
        debugWARNING("The sound system was already built\n");
    }

    // get the singleton instance for now
    mSoundManager = &mm::SoundManager::getInstance();
    debugERROR("TODO: we need to configure the sound system here... reading the"
        " values in the config structure...\n");

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
SoundSystemLoader::unload(void)
{
    debugERROR("Check if we have to do something here... since it is a singleton "
        "we cannot free its memory\n");

    mSoundManager = 0;
    return true;
}

} /* namespace engine */
