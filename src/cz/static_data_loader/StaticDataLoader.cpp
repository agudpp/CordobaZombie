/*
 * StaticDataLoader.cpp
 *
 *  Created on: Sep 10, 2013
 *      Author: agustin
 */

#include "StaticDataLoader.h"

#include <ia/path_handler/PathHandler.h>
#include <world_object/WorldObject.h>
#include <global_data/GlobalData.h>

namespace cz {

StaticDataLoader::StaticDataLoader()
{
    cz::PathHandler::setPathfinder(&pathfinder);
    cz::WorldObject::setCollisionHandler(&collHandler);

    cz::GlobalData::collHandler = &collHandler;
}

StaticDataLoader::~StaticDataLoader()
{

}

} /* namespace cz */
