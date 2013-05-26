/*
 * Test.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreSceneNode.h>
#include <iostream>
#include <assert.h>
#include <OgreSceneQuery.h>
#include <OgreRay.h>
#include <OgreManualObject.h>

#include "TriNavMeshBuilder.h"
#include "GlobalObjects.h"
#include "Test.h"
#include "Util.h"

void Test::drawTrianglesBox(const PolyStructsContainer<Triangle *> &cont)
{
	const std::vector<Triangle *> &objs = cont.getObjs();
	for(int i = objs.size()-1; i >= 0; --i){
		Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_CUBE);
		Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

		node->attachObject(ent);
		math::Point p;
		objs[i]->getCenterPoint(p);
		node->setPosition(Ogre::Vector3(p.x,0,p.y));
		node->setScale(0.2, 0.2, 0.2);
	}


}

void Test::drawTriangle(const Triangle *t)
{
	Ogre::ManualObject* manual = GLOBAL_SCN_MNGR->createManualObject();
	manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);

	Ogre::Real height = 10.0f;
	manual->position(t->v1->x, height, t->v1->y);  // start position
	manual->position(t->v2->x, height, t->v2->y);  // start position
	manual->position(t->v3->x, height, t->v3->y);  // start position
	manual->position(t->v1->x, height, t->v1->y);  // start position

	manual->end();
	GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode()->attachObject(manual);
}



Test::Test() :
		mTnm(5,5),
		mTAStar(&mTnm),
		mPlacesCount(0)
{


}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}


/* Load additional info */
void Test::loadAditionalData(void)
{

	std::vector<math::Vertex *> vertexs;
	std::vector<Triangle *> triangles;

	ASSERT(TriNavMeshBuilder::importGraph(vertexs, triangles, "testGraph.txt"));
	for(int i = triangles.size() - 1; i >= 0; --i)
	{
//		mTnm.addTriangle(tv[i]);
		drawTriangle(triangles[i]);
	}
	return ;


//	debugBLUE("Estamos importando un grafo de un archivo, comentar esto y "
//			"utilizar la segunda parte para crear un .txt del grafo.\n"
//			"TODO: Hacer mas generica esta app que soporte por parametros el .mesh"
//			" y el nombre de archivo a exportar. (en otra vida probablemente :p)\n ");
//	// load from file...
//	std::vector<GNode *> nodes;
//	std::vector<GEdge *> edges;
//	ASSERT(TriNavMeshBuilder::importGraph(nodes,mVvertexs, mTriangles, edges, "testGraph.txt"));
//
//	std::vector<Triangle *> &tv2 = mTriangles.getObjs();
//	for(int i = tv2.size() - 1; i >= 0; --i)
//	{
////		mTnm.addTriangle(tv[i]);
//		drawTriangle(tv2[i]);
//	}
//	Graph graph;
//	graph.setNodesAndEdges(nodes,edges);
//	ASSERT(mTnm.buildFromGraph(graph) == 0);
//
//	return;

//
//	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity("pathfinding.mesh");
//	Common::Util::getTrianglesFromMesh(mVvertexs, mTriangles, ent->getMesh());
//
//
//	debug("Number of vertexs: %d, Triangles size: %d\n", mVvertexs.getSize(),
//			mTriangles.getSize());
//
//	std::vector<math::Vertex *> &vv = mVvertexs.getObjs();
//	for(int i = vv.size()-1; i >= 0; --i){
//		debug("Vertex[%d]: (%f, %f)\n", i, vv[i]->x, vv[i]->y);
//	}
//
//	std::vector<Triangle *> &tv = mTriangles.getObjs();
//	for(int i = tv.size() - 1; i >= 0; --i)
//	{
//		mTnm.addTriangle(tv[i]);
//		drawTriangle(tv[i]);
//	}
//	ASSERT(mTnm.build() == 0);
//	debug("finish\n");
//
//	const Graph &g = mTnm.getGraph();
//	TriNavMeshBuilder::exportGraph(g.getNodes(),g.getEdges(), "testGraph.txt");
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{

}
