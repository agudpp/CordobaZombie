/*
 * OpenALHandler.cpp
 *
 *  Created on: Dec 12, 2013
 *      Author: agustin
 */

#include "OpenALHandler.h"

#include <debug/DebugUtil.h>

namespace mm {

////////////////////////////////////////////////////////////////////////////////
OpenALHandler::OpenALHandler() :
    mContext(0)
,   mDevice(0)
{
}
////////////////////////////////////////////////////////////////////////////////
OpenALHandler::~OpenALHandler()
{
    // destroy the context and the device?
    debugWARNING("Here we need to check if we want to close the device or not, "
        "because, probably we are using a \"shared\" device and we have not to"
        " close it.\n");
    destroyCurrentContext();
    closeDevice();
}

////////////////////////////////////////////////////////////////////////////////
void
OpenALHandler::getDevices(std::vector<std::string>& devsVec)
{
    // Using Soundsystem carlo's code.
    int pos = 0,
        MAX_POS = 1<<10;
    const ALCchar* device  = NULL;
    const ALCchar* devices = NULL;

    devsVec.clear();

    if (!alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT")) {
        debugWARNING("%s","Can't retrieve sound devices info. "
                "Missing extension \"ALC_ENUMERATION_EXT\".\n");
        return;
    }

    devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

    // check if we could get some device
    if (devices == 0) {
        debugERROR("We couldn't retrieve devices, this is rare, problems in "
            "openal?\n");
        return;
    }

    // "devices" contains the device names, separated by NULL
    // and terminated by two consecutive NULLs.
    pos = 0;
    do {
        device = &(devices[pos]);
        if (*device != '\0') {
            devsVec.push_back(device);
            pos += strlen(device)+1;
        }
    } while (*device != '\0' && pos < MAX_POS);

    if (pos >= MAX_POS) {
        debugERROR("Error retrieving sound devices info. List was too long.\n");
        devsVec.clear();
    }

    // TODO: check, here devices should be freed?
}

////////////////////////////////////////////////////////////////////////////////
bool
OpenALHandler::openDevice(const std::string& name)
{
    // check if we already have some device
    if (mDevice) {
        debugERROR("We already have a device set!\n");
        return false;
    }

    // Copied from SoundSystem code.
    //
    mDevice = name.empty() ? alcOpenDevice(0) : alcOpenDevice(name.c_str());
    if (mDevice == 0) {
        debugERROR("Couldn't use \"%s\" sound device.\n",
                        (name.empty() ? "default" : name.c_str()));
        return false;
    }

    // we could open the device
    debugGREEN("Device %s opened correctly\n", name.c_str());
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
OpenALHandler::useDevice(ALCdevice* device)
{
    if (mDevice) {
        debugERROR("We already have a device set\n");
        ASSERT(false && "TODO: Remove the current device and use the new one. "
            "Probably this will have no sense and we will never use this");
        return;
    }
    if (device == 0) {
        debugERROR("Invalid device param\n");
        return;
    }

    mDevice = device;
}

////////////////////////////////////////////////////////////////////////////////
bool
OpenALHandler::createContext(void)
{
    if (mDevice == 0) {
        debugERROR("No device was opened / set before! We cannot create a context "
            "without device\n");
        return false;
    }
    if (mContext != 0) {
        debugWARNING("We already have a context, we will not create anything. Destroy"
            " the current one before\n");
        return false;
    }

    mContext = alcCreateContext(mDevice, NULL);
    if(!mContext) {
        debugERROR("Error creating the context\n");
        return false;
    }

    // everything fine
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
OpenALHandler::makeContextCurrent(void)
{
    if (mContext == 0) {
        debugERROR("No context created\n");
        return false;
    }

    return alcMakeContextCurrent(mContext);
}

////////////////////////////////////////////////////////////////////////////////
void
OpenALHandler::suspendCurrentContext(void)
{
    if (mContext == 0) {
        debugERROR("No context created\n");
        return;
    }
    alcSuspendContext(mContext);
}

////////////////////////////////////////////////////////////////////////////////
void
OpenALHandler::enableCurrentContextet(void)
{
    if (mContext == 0) {
        debugERROR("No context created\n");
        return;
    }
    alcProcessContext(mContext);
}

////////////////////////////////////////////////////////////////////////////////
bool
OpenALHandler::closeDevice(void)
{
    if (mDevice == 0) {
        // nothing to do
        return true;
    }

    const bool ret = alcCloseDevice(mDevice);
    if (ret) {
        mDevice = 0;
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
bool
OpenALHandler::destroyCurrentContext(void)
{
    if (mContext == 0) {
        // nothing to do.
        return true;
    }
    alcDestroyContext(mContext);
    mContext = 0;
    return true;
}

}
