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

    // get the resource path
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
