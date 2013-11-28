/*
 * Engine.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#include "Engine.h"

#include <debug/DebugUtil.h>
#include <modules/InputSystemLoader.h>
#include <modules/OgreSystemLoader.h>
#include <modules/ResourcesSystemLoader.h>
#include <modules/SoundSystemLoader.h>
#include <modules/VideoSystemLoader.h>

#include "EngineConfiguration.h"

namespace engine {


////////////////////////////////////////////////////////////////////////////////
Engine::Engine()
{
    // create the loaders

    // ogreSystemLoader
    OgreSystemLoader* ogreLoader = new OgreSystemLoader(mOgreData.renderWindow,
                                                        mOgreData.sceneMngr,
                                                        mOgreData.camera,
                                                        mOgreData.root);
    mLoaders.push_back(ogreLoader);
    mLoader.addModuleLoader(ogreLoader);

    // Input system
    InputSystemLoader* inputLoader = new InputSystemLoader(mOgreData.renderWindow,
                                                           mInputData.mouse,
                                                           mInputData.keyboard,
                                                           mInputData.manager);
    mLoaders.push_back(inputLoader);
    mLoader.addModuleLoader(inputLoader);

    // ResourceSystem
    ResourcesSystemLoader* rscLoader =
        new ResourcesSystemLoader(mResourcesData.handler);

    mLoaders.push_back(rscLoader);
    mLoader.addModuleLoader(rscLoader);

    // SoundSystem
    SoundSystemLoader* soundLoader = new SoundSystemLoader(mSoundData.handler);

    mLoaders.push_back(soundLoader);
    mLoader.addModuleLoader(soundLoader);

    // VideoSystem
    VideoSystemLoader* videoLoader = new VideoSystemLoader(mVideoData.player);

    mLoaders.push_back(videoLoader);
    mLoader.addModuleLoader(videoLoader);
}

////////////////////////////////////////////////////////////////////////////////
Engine::~Engine()
{
    unloadAll();

    // destroy all the loaders
    for (IModuleLoader* l : mLoaders) delete l;
    mLoaders.clear();
}

////////////////////////////////////////////////////////////////////////////////
bool
Engine::loadAll(const std::string& configPath)
{
    //check that we have loaders
    if (mLoaders.empty()) {
        debugERROR("No loaders set!\n");
        return false;
    }

    // load the configuration
    EngineConfiguration config;
    if (!config.load(configPath)) {
        debugERROR("Error loading the config file %s\n", configPath.c_str());
        return false;
    }

    if (!mLoader.load(config)) {
        debugERROR("Error when loading something\n");
        return false;
    }

    // everything perfect
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
Engine::unloadAll(void)
{
    return mLoader.unload();
}

} /* namespace engine */
