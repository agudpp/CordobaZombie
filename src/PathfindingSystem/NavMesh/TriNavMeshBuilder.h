/*
 * TriNavMeshBuilder.h
 *
 *  Created on: 24/01/2012
 *      Author: agustin
 */

#ifndef TRINAVMESHBUILDER_H_
#define TRINAVMESHBUILDER_H_

#include <OgreMesh.h>
#include <OgreString.h>

#include <vector>

#include "TriangleNavMesh.h"
#include "Graph.h"
#include "PolyStructsContainer.h"
#include "Triangle.h"


class TriNavMeshBuilder
{
public:


	/* Export a graph (triangle navmesh graph) to a file
	 * Requires:
	 * 	@nodes		The list of all the nodes
	 * 	@edges		The list of edges
	 * 	@fname		the filename to export the graph
	 */
	static bool exportGraph(const std::vector<GNode *> &nodes,
			const std::vector<GEdge *> &edges,
			const Ogre::String &fname);

	/* Import a graph from a file.
	 * Requires:
	 * 	@nodes		The list to be filled with the nodes
	 * 	@vertexs	The container to put the vertexs
	 * 	@triangles	The container to put the triangles
	 * 	@edges		The list of edges to be filled
	 * 	@fname		The filename
	 * Returns:
	 * 	true		on success
	 * 	false		otherwise
	 */
	static bool importGraph(std::vector<GNode *> &nodes,
			PolyStructsContainer<sm::Vertex *> &vertexs,
			PolyStructsContainer<Triangle *> &triangles,
			std::vector<GEdge *> &edges,
			const Ogre::String &fname);

	/**
	 * Import the graph from a file, we only will read the vertexs and triangles
	 * @param	vertexs		The vertexs to read (and allocate)
	 * @param	triangles	The triangles to read (and allocate)
	 * @return	True		on success
	 * 			false		otherwise
	 */
	static bool importGraph(std::vector<sm::Vertex *> &vertexs,
			std::vector<Triangle *> &triangles,
			const Ogre::String &fname);



private:
	TriNavMeshBuilder();
	~TriNavMeshBuilder();
};

#endif /* TRINAVMESHBUILDER_H_ */
