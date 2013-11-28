/*
 * EngineLoader.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#include "EngineLoader.h"

#include <debug/DebugUtil.h>

namespace engine {



////////////////////////////////////////////////////////////////////////////////
EngineLoader::EngineLoader()
{
}

////////////////////////////////////////////////////////////////////////////////
EngineLoader::~EngineLoader()
{
}


////////////////////////////////////////////////////////////////////////////////
void
EngineLoader::addModuleLoader(IModuleLoader* loader)
{
    ASSERT(loader);
    mLoaders.push_back(loader);
}

////////////////////////////////////////////////////////////////////////////////
void
EngineLoader::clearLoaders(void)
{
    mLoaders.clear();
}

////////////////////////////////////////////////////////////////////////////////
bool
EngineLoader::load(const EngineConfiguration& config, bool verbose)
{
    if (mLoaders.empty()) {
        debugERROR("We have no loaders to load anything\n");
        return false;
    }

    if (verbose) {
        debugGREEN("Starting loading system, ready to use %d loaders\n",
                   mLoaders.size());
    }
    for (IModuleLoader* l : mLoaders) {
        if (verbose) {
            debugGREEN("Loading %s loader\n", l->name().c_str());
        }
        if (!l->load(config)) {
            debugERROR("Error loading loader %s\n", l->name().c_str());
            return false;
        }
    }

    if (verbose) {
        debugGREEN("All loaders executed correctly\n");
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
EngineLoader::unload(void)
{
    if (mLoaders.empty()) {
        debugERROR("We have no loaders to unload anything\n");
        return false;
    }

    for (IModuleLoader* l : mLoaders) {
        if (!l->unload()) {
            debugERROR("Error unloading loader %s\n", l->name().c_str());
            return false;
        }
    }

    return true;
}

} /* namespace engine */
