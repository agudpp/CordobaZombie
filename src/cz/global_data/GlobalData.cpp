/*
 * GlobalData.cpp
 *
 *  Created on: Sep 10, 2013
 *      Author: agustin
 */

#include "GlobalData.h"

#include <debug/DebugUtil.h>
#include <os_utils/OSHelper.h>
#include <ResourceHandler.h>

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
    if (!rscHandler) {
        debugERROR("No ResourceHandler set!\n");
        return false;
    }
    path = rscHandler->getResourceRootPath();
    return true;
}

} /* namespace cz */
