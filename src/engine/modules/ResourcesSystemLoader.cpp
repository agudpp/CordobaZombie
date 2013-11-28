/*
 * ResourcesSystemLoader.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#include "ResourcesSystemLoader.h"

#include <debug/DebugUtil.h>
#include <os_utils/OSHelper.h>

namespace engine {

ResourcesSystemLoader::ResourcesSystemLoader(rrh::ResourceHandler*& rh) :
    IModuleLoader("ResourcesSystemLoader")
,   mResourceHandler(rh)
{
}

ResourcesSystemLoader::~ResourcesSystemLoader()
{
}

////////////////////////////////////////////////////////////////////////////
// Inherited methods
//

////////////////////////////////////////////////////////////////////////////////
bool
ResourcesSystemLoader::load(const EngineConfiguration& config)
{
    if (mResourceHandler != 0) {
        debugWARNING("The resourceSystem was already loaded!\n");
        return true;
    }

    // create the instance of the resource handler
    mResourceHandler = new rrh::ResourceHandler;

    // get he full path to the root resources elements
    std::string rootRscPath;
    if (!config.getValue("ResourcesSystemLoader", "rootRscPath", rootRscPath)) {
        debugERROR("We couldn't find the path to the resource folder\n");
        return false;
    }
    debugERROR("We will use the one that is in the dropbox folder, this should be"
        " removed in the release and we should use the just readed path\n");
    char* envVar = 0;
    // for now we will use the one used by CZ = CZ01_RC_PATH
    if (!core::OSHelper::getEnvVar("CZ01_RC_PATH", envVar) ||
        envVar == 0) {
        debugERROR("Error getting the CZ01 resource path CZ01_RC_PATH\n");
        return false;
    }
    // get the resource path
    rootRscPath = envVar;
    core::OSHelper::addEndPathVar(rootRscPath);


    // set it to the resource handler
    mResourceHandler->setResourceRootPath(rootRscPath);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
ResourcesSystemLoader::unload(void)
{
    if (mResourceHandler == 0) {
        debugWARNING("We never build this?\n");
        return true;
    }

    // we should unload everything?
    // mResourceHandler->unloadAllResourceGroup();????
    //
    delete mResourceHandler;
    return true;
}

} /* namespace engine */
