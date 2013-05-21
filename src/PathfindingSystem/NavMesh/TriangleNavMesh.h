/*
 * TriangleNavMesh.h
 *
 *  Created on: 20/01/2012
 *      Author: agustin
 *
 */

#ifndef TRIANGLENAVMESH_H_
#define TRIANGLENAVMESH_H_


#include <vector>

#include "Graph.h"
#include "GNode.h"
#include "GEdge.h"
#include "Triangle.h"
#include "PolygonStructs.h"
#include "DebugUtil.h"

class TriangleNavMesh {
public:
	// We can set the number of rows and columns of the matrix
	TriangleNavMesh(int numColumns = 50, int numRows = 50);
	~TriangleNavMesh();

	/* Set the number of columns and rows of the matrix */
	void setNumColumnsAndRows(int numColumns, int numRows);

	/* This function returns a Node from a 2D point in the world.
	 * In case that the point is outside of the world or there are no Node
	 * in that point 0 is returned
	 */
	const GNode *getNodeFromPoint(const sm::Point &p) const;

	// Adds a new triangle to be used when build the graph
	void addTriangle(const Triangle *t);
	void addTriangles(const std::vector<Triangle *> &triangles);

	/* Builds the level with all the information we have. In case of error,
	 * a negative number is returned.
	 * This is a slow function that will build the Graph
	 */
	int build(void);

	/**
	 * Destroy the actual graph
	 */
	void destroy(void);

	/* Builds the level from a list of Edges and a list of nodes */
	int buildFromGraph(const Graph &g);

	/* Returns the actual Graph */
	const Graph &getGraph(void) const {return mGraph;}

	/* TODO: deberiamos poder crear el navMeshMatrix de antemano cargandolo
	 * desde un archivo asi no hay que recalcular todo el tiempo el grafo.
	 */

	/* TODO: deberiamos poder soportar "agregar" nuevos triangulos para
	 * poder modificar el grafo dinamicamente (en caso que el nivel "estatico"
	 * pueda cambiar rotundamente).
	 */


private:

	typedef std::vector<const Triangle *>	TriangleVec;
	// the NodeMatrix
	typedef std::vector<GNode *>				NodeVec;
	typedef std::vector<GEdge *>				EdgeVec;


	// Auxiliary function to create the matrix (is destroyed if already created)
	void createMatrix(int rows, int columns);

	// destroy the matrix
	void destroyMatrix(void);

	// Set the {X,Y}FactorPosition used to get the matrix position
	// It use the mRowCount and mColumnCount
	void setFactorsPosition(float levelSizeX, float levelSizeY);

	// Gets the bounding box that contains all the triangles
	void getTrianglesBoundingBox(const TriangleVec &triangles,
			sm::Point &tl, sm::Point &br);

	// Get the bounding box of a cell
	inline void getCellBoundingBox(sm::AlignedBox &box, unsigned int r, unsigned int c) const
	{
		ASSERT(r < mRowCount);
		ASSERT(c < mColumnCount);
		float xFactor = 1.0f/mXFactorPosition;
		float yFactor = 1.0f/mYFactorPosition;

		box.tl.x = c * xFactor;
		box.tl.y = (r+1) * yFactor;
		box.br.x = (c+1) * xFactor;
		box.br.y = r * yFactor;

	}

	// Returns all the cells that intersect a triangle
	void getCellsIntersection(const Triangle &triangle,
				std::vector<NodeVec*> &cells);

	// construction functions

	// Link two nodes creating the Edge associated if is possible, 0 otherwise
	GEdge *linkNodes(GNode *n1, GNode *n2);

	// Fills the matrix with the nodes
	void fillMatrix(const NodeVec &nodes);

	int buildGraph(const TriangleVec &triangles);

private:
	// the triangles that will use to build the level
	TriangleVec		mTriangles;
	// the matrix
	NodeVec			**mMatrix;
	int				mRowCount;
	int				mColumnCount;
	// variables used to get the real row and column of the matrix given a point
	float			mXFactorPosition;
	float			mYFactorPosition;

	// TODO: este vector no haria falta, es solo por comodidad, ya que tenemos
	// todos los nodes en la matrix al igual que las edges, se pueden
	// obtener todas desde la matrix
	Graph			mGraph;

};

#endif /* TRIANGLENAVMESH_H_ */
