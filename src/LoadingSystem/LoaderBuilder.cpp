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
void
LoaderBuilder::getLoaders(std::vector<Loader *> &result)
{
    result.clear();

    result.push_back(new LevelGeometryLoader);
//	TODO: enable the following two loaders
//    result.push_back(new LevelPathfindLoader);
//    result.push_back(new ZombieLoader);
    // TODO: add the other ones
}
