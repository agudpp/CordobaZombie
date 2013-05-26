/*
 * TriNavMeshBuilder.cpp
 *
 *  Created on: 24/01/2012
 *      Author: agustin
 */

#include <OgreSubMesh.h>

#include <map>
#include <algorithm>
#include <set>
#include <fstream>

#include "TriNavMeshBuilder.h"

#include "DebugUtil.h"




/* Export a graph (triangle navmesh graph) to a file
 * Requires:
 * 	@nodes		The list of all the nodes
 * 	@fname		the filename to export the graph
 */
bool
TriNavMeshBuilder::exportGraph(const std::vector<GNode *> &nodes,
		const std::vector<GEdge *> &edges,
		const Ogre::String &fname)
{
	std::ofstream out;

	out.open(fname.c_str());
	if(!out.is_open()){
		debugERROR("Error trying to open the file %s\n", fname.c_str());
		return false;
	}

	// get all the vertexs
	std::map<const sm::Vertex *, int> vertexs;
	int vertexCount = 0;
	std::vector<const sm::Vertex *> vertexList;
	for(int i = nodes.size()-1; i >= 0; --i){
		ASSERT(nodes[i]->getTriangle());
		const sm::Vertex *v1 = nodes[i]->getTriangle()->v1;
		const sm::Vertex *v2 = nodes[i]->getTriangle()->v2;
		const sm::Vertex *v3 = nodes[i]->getTriangle()->v3;
		if(vertexs.find(v1) == vertexs.end())
		{vertexs[v1] = vertexCount++; vertexList.push_back(v1);}

		if(vertexs.find(v2) == vertexs.end())
		{vertexs[v2] = vertexCount++; vertexList.push_back(v2);}

		if(vertexs.find(v3) == vertexs.end())
		{vertexs[v3] = vertexCount++; vertexList.push_back(v3);}
	}

	ASSERT(vertexCount == vertexList.size());

	// save the number of vertexs and save the vertexs
	out << vertexCount << "\n";
	for(int i = 0; i < vertexCount; ++i){
		out << vertexList[i]->x << "\t" << vertexList[i]->y << "\n";
	}

	// save the number of nodes and the nodes and the nodeMap
	std::map<const GNode *, int> nodesMap;
	int nodesCount = 0;
	out << nodes.size() << "\n";
	for(int i = 0; i < nodes.size(); ++i){
		if(nodesMap.find(nodes[i]) == nodesMap.end()){nodesMap[nodes[i]] = nodesCount++;}
		const Triangle *t = nodes[i]->getTriangle();
		ASSERT(t);
		// we will save the vertexs index of the triangle of the node
		out << vertexs[t->v1] << "\t" << vertexs[t->v2] << "\t" <<
				vertexs[t->v3] << "\n";
	}

	// save the number of edges and the edges
	out << edges.size() << "\n";
	for(int i = 0; i < edges.size(); ++i){
		const GNode *n1 = edges[i]->getNode1();
		const GNode *n2 = edges[i]->getNode2();
		ASSERT(n1 && n2);

		// we will save the nodes index and the distance
		out << nodesMap[n1] << "\t" << nodesMap[n2] << "\t" <<
				edges[i]->getWeight() << "\n";
	}

	out.close();

	return true;
}

bool
TriNavMeshBuilder::exportGraph(const Graph &graph, const Ogre::String &fname)
{
    return exportGraph(graph.getNodes(), graph.getEdges(), fname);
}


/* Import a graph from a file.
 * Requires:
 * 	@nodes		The list to be filled with the nodes
 * 	@container	The container to put the vertexs
 * 	@edges		The list of edges to be filled
 * 	@fname		The filename
 * Returns:
 * 	true		on success
 * 	false		otherwise
 */
bool
TriNavMeshBuilder::importGraph(std::vector<GNode *> &nodes,
		PolyStructsContainer<sm::Vertex *> &container,
		PolyStructsContainer<Triangle *> &triangles,
		std::vector<GEdge *> &edges,
		const Ogre::String &fname)
{
	std::ifstream in;
	in.open(fname.c_str());

	if(!in.is_open()){
		debugERROR("Error while opening the file %s\n", fname.c_str());
		return false;
	}

	if(!container.isEmpty()){
		debugWARNING("Warning, Vertex container is not empty\n");
		ASSERT(false);
	}
	if(!triangles.isEmpty()){
		debugWARNING("Warning, triangles is not empty\n");
		ASSERT(false);
	}

	// first read the vertexs list
	std::vector<sm::Vertex *> vertexs;
	int aux;
	in >> aux;
	ASSERT(aux > 0);
	vertexs.reserve(aux);

	for(int i = 0; i < aux; ++i){
		sm::Vertex *v = new sm::Vertex;
		in >> v->x;
		in >> v->y;
		vertexs.push_back(v);
	}

	ASSERT(in.good());

	// now read the number of nodes
	nodes.clear();
	in >> aux;
	nodes.reserve(aux);
	std::vector<Triangle *> triangVec;
	int i1, i2, i3;
	for(int i = 0; i < aux; ++i){
		in >> i1;
		in >> i2;
		in >> i3;
		ASSERT(i1 < vertexs.size() && i2 < vertexs.size() && i3 < vertexs.size());
		Triangle *t = new Triangle(vertexs[i1], vertexs[i2], vertexs[i3]);
		triangVec.push_back(t);

		GNode *node = new GNode(t);
		nodes.push_back(node);
	}
	ASSERT(in.good());

	// now get all the edges
	edges.clear();
	in >> aux;
	ASSERT(aux > 0);
	edges.reserve(aux);
	bool ok = false;
	float weight;
	for(int i = 0; i < aux; ++i){
		in >> i1;
		in >> i2;
		in >> weight;
		if(i1 >= nodes.size() || i2 >= nodes.size()){
			debugRED("i1: %d, size: %zd, i2: %d, size: %zd\n", i1, nodes.size(),
					i2, nodes.size());
			ASSERT(i1 < nodes.size() && i2 < nodes.size());
		}
		GNode *n1 = nodes[i1], *n2 = nodes[i2];

		GEdge *e = new GEdge(n1,n2);
		ok = n1->setNewEdge(e);
#ifdef DEBUG
		if (!ok){
		    debugERROR("i1: %d, i2: %d, i: %d\n", i1, i2, i);
		    ASSERT(ok);
		}
#endif

		ok = n2->setNewEdge(e);
#ifdef DEBUG
        if (!ok){
            debugERROR("i1: %d, i2: %d, i: %d\n", i1, i2, i);
            ASSERT(ok);
        }
#endif
		edges.push_back(e);
		e->setWeight(weight);

	}


	// Save the triangles and the vertexs
	for(int i = vertexs.size()-1; i >= 0; --i){
		container.addObj(vertexs[i]);
	}
	vertexs.clear();

	for(int i = triangVec.size()-1; i >= 0; --i){
		triangles.addObj(triangVec[i]);
	}
	triangVec.clear();


	return true;
}



/**
 * Import the graph from a file, we only will read the vertexs and triangles
 * @param	vertexs		The vertexs to read (and allocate)
 * @param	triangles	The triangles to read (and allocate)
 * @return	True		on success
 * 			false		otherwise
 */
bool
TriNavMeshBuilder::importGraph(std::vector<sm::Vertex *> &vertexs,
		std::vector<Triangle *> &triangVec,
		const Ogre::String &fname)
{
	std::ifstream in;
	in.open(fname.c_str());

	if(!in.is_open()){
		debugERROR("Error while opening the file %s\n", fname.c_str());
		return false;
	}

	if(!vertexs.empty()){
		debugWARNING("Warning, Vertex container is not empty\n");
		ASSERT(false);
	}
	if(!triangVec.empty()){
		debugWARNING("Warning, triangles is not empty\n");
		ASSERT(false);
	}

	// first read the vertexs list
	int aux;
	in >> aux;
	ASSERT(aux > 0);
	vertexs.reserve(aux);

	for(int i = 0; i < aux; ++i){
		sm::Vertex *v = new sm::Vertex;
		in >> v->x;
		in >> v->y;
		vertexs.push_back(v);
	}

	if(!in.good()){
		debugERROR("Error reading the file\n");
		ASSERT(in.good());
		return false;
	}

	// now read the number of nodes
	in >> aux;
	int i1, i2, i3;
	for(int i = 0; i < aux; ++i){
		in >> i1;
		in >> i2;
		in >> i3;
		ASSERT(i1 < vertexs.size() && i2 < vertexs.size() && i3 < vertexs.size());
		Triangle *t = new Triangle(vertexs[i1], vertexs[i2], vertexs[i3]);
		triangVec.push_back(t);
	}

	if(!in.good()){
		debugERROR("Error reading the file\n");
		ASSERT(in.good());
		return false;
	}

	return true;
}


