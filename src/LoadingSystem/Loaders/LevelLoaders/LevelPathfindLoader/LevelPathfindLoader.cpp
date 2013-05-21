/*
 * LevelPathfindLoader.cpp
 *
 *  Created on: 09/05/2012
 *      Author: agustin
 *
 */
#include "DebugUtil.h"
#include "TriNavMeshBuilder.h"

#include "LevelPathfindLoader.h"

LevelPathfindLoader::LevelPathfindLoader() :
Loader("LevelPathfindLoader"),
mPathfindingManager(0)
{
    setWeight(20);
}

LevelPathfindLoader::~LevelPathfindLoader()
{
}


// Functoin used to load something
int
LevelPathfindLoader::load(TiXmlElement*elem, LoaderData *data)
{
	ASSERT(data);
	ASSERT(elem);
	ASSERT(Ogre::String(elem->Value()) == "LevelPathfindingLoader");

	mPathfindingManager = data->pathfindingManager;
	ASSERT(mPathfindingManager);

	// load the pathfinding.
	const TiXmlElement *e = elem->FirstChildElement("file");
	ASSERT(e);
	const char *fname = e->Attribute("name");
	ASSERT(fname);

	// unload all
	unload();

	// load all
	std::vector<sm::Vertex *> vertexs;
	std::vector<Triangle *> triangles;
	if(!TriNavMeshBuilder::importGraph(vertexs,triangles,fname)) {
		debugERROR("Error loading the file %s\n", fname);
		ASSERT(false);
		return -1;
	}

	if(mPathfindingManager->loadLevel(triangles, vertexs) < 0){
		debugERROR("Error creating the level of %s\n", fname);
		ASSERT(false);
		return -2;
	}

	debugGREEN("Pathfinding Level %s loaded OK\n", fname);
	return 0;
}

// Unload the information?
int LevelPathfindLoader::unload()
{
	ASSERT(mPathfindingManager);
	mPathfindingManager->unloadActualLevel();
	debugGREEN("Pathfinding unloaded ok\n");
	return 0;
}
