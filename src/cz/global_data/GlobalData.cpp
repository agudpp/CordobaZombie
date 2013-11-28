/*
 * GlobalData.cpp
 *
 *  Created on: Sep 10, 2013
 *      Author: agustin
 */

#include "GlobalData.h"

#include <debug/DebugUtil.h>
#include <os_utils/OSHelper.h>

namespace cz {

Ogre::SceneManager* GlobalData::sceneMngr = 0;
Ogre::Camera* GlobalData::camera = 0;

rrh::ResourceHandler* GlobalData::rscHandler = 0;
coll::CollisionHandler* GlobalData::collHandler = 0;
float GlobalData::lastTimeFrame = 0.f;


////////////////////////////////////////////////////////////////////////////
bool
GlobalData::getRootResourcesPath(std::string& path)
{
    // TODO: here we should return the resourceHandler->getRootPath();
    char* envVar = 0;
    // for now we will use the one used by CZ = CZ01_RC_PATH
    if (!core::OSHelper::getEnvVar("CZ01_RC_PATH", envVar) ||
        envVar == 0) {
        debugERROR("Error getting the CZ01 resource path CZ01_RC_PATH\n");
        return false;
    }
    // get the resource path
    path = envVar;
    core::OSHelper::addEndPathVar(path);
    return true;
}

} /* namespace cz */
