/*
 * OpenALLoader.cpp
 *
 *  Created on: Dec 14, 2013
 *      Author: agustin
 */

#include "OpenALLoader.h"

#include <string>

#include <debug/DebugUtil.h>


namespace engine {


////////////////////////////////////////////////////////////////////////////////
OpenALLoader::OpenALLoader(mm::OpenALHandler*& openalHandler) :
    IModuleLoader("OpenALLoader")
,   mOpenalHandler(openalHandler)
{

}
////////////////////////////////////////////////////////////////////////////////
OpenALLoader::~OpenALLoader()
{
}

////////////////////////////////////////////////////////////////////////////////
bool
OpenALLoader::load(const EngineConfiguration& config)
{
    if (mOpenalHandler) {
        debugERROR("Openal handler already created?\n");
        return false;
    }

    // create the openal handler
    mOpenalHandler = new mm::OpenALHandler;

    std::string device;
    if (!config.getValue("OpenALLoader", "device", device)) {
        debugERROR("We couldn't find the device attribute? This doesn't mean that"
            " we want the default behavior!\n");
        return false;
    }

    // now load the openal context
    if (!mOpenalHandler->openDevice(device)) {
        debugWARNING("Error opening the device %s\n", device.c_str());
        // try to open the default device
        if (device.empty() || !mOpenalHandler->openDevice()) {
            debugERROR("Error opening the default device... something is wrong?\n");
            return false;
        } else {
            debugWARNING("Using the default device instead of %s\n", device.c_str());
        }
    }

    // create the context now
    if (!mOpenalHandler->createContext()) {
        debugERROR("Error creating the context... we cannot do anything else..\n");
        return false;
    }

    // make the current context current :p
    if (!mOpenalHandler->makeContextCurrent()) {
        debugERROR("Error setting the context as current\n");
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
OpenALLoader::unload(void)
{
    if (mOpenalHandler == 0) {
        return true; // nothing to do
    }

    // unload the context and the device
    bool ret = mOpenalHandler->destroyCurrentContext();
    ret = mOpenalHandler->closeDevice() && ret;

    delete mOpenalHandler;
    mOpenalHandler = 0;

    return ret;
}

} /* namespace engine */
