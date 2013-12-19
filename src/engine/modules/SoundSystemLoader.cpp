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

    // Check MantisBT issue #296 (http://goo.gl/awV5FF)
    std::string fullPath;
    core::OSHelper::addEndPathVar(soundsRscPath);
    soundsRscPath.append("resources.cfg");
    mResourceHandler->getResourceFullPath(soundsRscPath, fullPath);
    rrh::ResourceGroup rg;
    rg.setOgreResourceFile(fullPath);
    if (!mResourceHandler->loadResourceGroup(rg)) {
        debugERROR("We are not being able to load the folder with the sounds %s\n",
                   fullPath.c_str());
        // this is not critical so we will not kill ourself here.
    }
    ///////////////////////////////////////////////////////////////////////////

    // Get the SoundHandler singleton instance
    if (mSoundHandler) {
        debugWARNING("The sound system had already been built.\n");
    } else {
        mSoundHandler = &mm::SoundHandler::getInstance();
    }

    // set the openal handler
    ASSERT(mSoundHandler->soundManager());
    mSoundHandler->soundManager()->setOpenALHandler(mOpenalHandler);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
SoundSystemLoader::unload(void)
{

    mSoundHandler = 0;
    return true;
}

} /* namespace engine */
