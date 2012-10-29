/*
 * LoaderBuilder.cpp
 *
 *  Created on: 28/10/2012
 *      Author: agustin
 *
 */

#include "LoaderBuilder.h"

#include "LevelGeometryLoader.h"
#include "LevelPathfindLoader.h"
#include "ZombieLoader.h"

/**
 * Build all the loaders
 */
static void
LoaderBuilder::getLoaders(std::vector<Loader *> &result)
{
    result.clear();

    result.push_back(new LevelGeometryLoader);
    result.push_back(new LevelPathfindLoader);
    result.push_back(new ZombieLoader);
    // TODO: add the other ones
}

