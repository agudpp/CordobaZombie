/*
 * TriangleNavMesh.cpp
 *
 *  Created on: 20/01/2012
 *      Author: agustin
 *
 */

// Define some usefull MACROS
#define MATRIX_GET_COLUMN(x)	int(x * mXFactorPosition)
#define MATRIX_GET_ROW(y)		int(y * mYFactorPosition)

#define M_MAX(x,y)				((x)>(y))?(x):(y)
#define M_MIN(x,y)				((x)<(y))?(x):(y)


#include <algorithm>
#include <set>
#include <map>
#include <list>

#include "TriangleNavMesh.h"



////////////////////////////////////////////////////////////////////////////////
void
TriangleNavMesh::createMatrix(int rows, int columns)
{
	// check if the matrix is already created
	destroyMatrix();

	// set the new fields and construct the new matrix
	mRowCount = rows;
	mColumnCount = columns;

	mMatrix = new NodeVec*[mRowCount];

	for(int i = 0; i < mRowCount; i++) {
		mMatrix[i] = new NodeVec[mColumnCount];
	}
}

////////////////////////////////////////////////////////////////////////////////
void
TriangleNavMesh::destroyMatrix(void)
{
	// remove all the nodes and edges
	mGraph.removeAndDestroyEdeges();
	mGraph.removeAndDestroyNodes();

	if(mMatrix){
		int i = mRowCount - 1;
		// frees all the elements of the matrix
		for(; i >= 0; i--) {
			// frees the i row
			delete[] mMatrix[i];
		}
		delete[] mMatrix;
	}

	mMatrix = 0;
	debug("Matrix Deleted\n");
}

////////////////////////////////////////////////////////////////////////////////
void
TriangleNavMesh::setFactorsPosition(float levelSizeX, float levelSizeY)
{
	mXFactorPosition = 1.0f / ((levelSizeX) / static_cast<float>(mColumnCount));
	mYFactorPosition = 1.0f / ((levelSizeY) / static_cast<float>(mRowCount));

}

////////////////////////////////////////////////////////////////////////////////
void
TriangleNavMesh::getTrianglesBoundingBox(const TriangleVec &triangles,
		core::Point &tl, core::Point &br)
{
	ASSERT(!triangles.empty());

	tl.x = triangles[0]->v1->x;
	br.x = triangles[0]->v1->x;
	tl.y = triangles[0]->v1->y;
	br.y = triangles[0]->v1->y;

	const core::Vertex *v1,*v2,*v3;
	for(int i = triangles.size() - 1; i >= 0; --i){
		v1 = triangles[i]->v1;
		v2 = triangles[i]->v2;
		v3 = triangles[i]->v3;

		tl.x = M_MIN(M_MIN(tl.x, v1->x), M_MIN(v2->x, v3->x));
		tl.y = M_MAX(M_MAX(tl.y, v1->y), M_MAX(v2->y, v3->y));

		br.x = M_MAX(M_MAX(br.x, v1->x), M_MAX(v2->x, v3->x));
		br.y = M_MIN(M_MIN(br.y, v1->y), M_MIN(v2->y, v3->y));
	}
}

////////////////////////////////////////////////////////////////////////////////
void
TriangleNavMesh::getCellsIntersection(const Triangle &region,
			std::vector<NodeVec*> &cells)
{
	ASSERT(region.v1);
	ASSERT(region.v2);
	ASSERT(region.v3);

	// TODO: tenemos que devolver la lista de celdas que son afectadas
	// por un triangulo

	cells.clear();

	// TODO: mejorar esto, estamos haciendo muchos chequeos de mas
	unsigned int tlx = 0, tly = 0, brx = 0, bry = 0, i = 0, j = 0;

	tlx = MATRIX_GET_COLUMN((M_MIN(region.v1->x, M_MIN(region.v2->x, region.v3->x))));
	bry = MATRIX_GET_ROW((M_MIN(region.v1->y, M_MIN(region.v2->y, region.v3->y))));
	tly = MATRIX_GET_ROW((M_MAX(region.v1->y, M_MAX(region.v2->y, region.v3->y))));
	brx = MATRIX_GET_COLUMN((M_MAX(region.v1->x, M_MAX(region.v2->x, region.v3->x))));

	tly = M_MIN(tly, mRowCount-1);
	brx = M_MIN(brx, mColumnCount-1);

	// now check all the cells if there are inside or not
	core::AlignedBox box;
	for(i = bry; i <= tly; i++) {
		for(j = tlx; j <= brx; j++) {
			getCellBoundingBox(box, i, j);
//			debug("Boundingbox of %d,%d: tl:(%f,%f), br:(%f,%f)\n",
//								i, j, box.tl.x, box.tl.y, box.br.x, box.br.y);
			if(region.intersectAlignedBox(box)){
				cells.push_back(&(mMatrix[i][j]));
			}
		}
	}

//	debug("Returning cells size: %d\tRegion:(%f,%f), (%f,%f), (%f,%f)\t"
//			"tlx:%d,tly:%d,brx:%d,bry:%d, MATRIX(bry):%d\n",
//			cells.size(),
//			region.v1->x, region.v1->y, region.v2->x, region.v2->y,
//			region.v3->x, region.v3->y,
//			tlx, tly, brx, bry, MATRIX_GET_ROW(bry));

}

////////////////////////////////////////////////////////////////////////////////
void
TriangleNavMesh::fillMatrix(const NodeVec &nodes)
{
	std::vector<NodeVec*> cells;
	for(int i = nodes.size() - 1; i >= 0; --i){
		// get the cells intersection of the node
		getCellsIntersection(*(nodes[i]->getTriangle()), cells);
		if(cells.size() < 1){
//			debug("Node %d (%d) has not cell associated: (%f,%f), (%f,%f), (%f,%f)\n",
//					i, cells.size(),
//				nodes[i]->getTriangle()->v1->x,nodes[i]->getTriangle()->v1->y,
//				nodes[i]->getTriangle()->v2->x,nodes[i]->getTriangle()->v2->y,
//				nodes[i]->getTriangle()->v3->x,nodes[i]->getTriangle()->v3->y);
		}
		// for every cell we put the node
		for(int j = cells.size()-1; j >= 0; --j){
			// ASSERT that the triangle doesnt exists already
			ASSERT(std::find(cells[j]->begin(), cells[j]->end(),
					nodes[i]) == cells[j]->end());
			cells[j]->push_back(nodes[i]);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
GEdge *
TriangleNavMesh::linkNodes(GNode *n1, GNode *n2)
{
	ASSERT(n1);
	ASSERT(n2);
	ASSERT(n1->getTriangle());
	ASSERT(n2->getTriangle());

	if(n1 == n2){
		// trying to link the same node, return;
		return 0;
	}
	if(n1 != n2){
		ASSERT(n1->getTriangle()!= n2->getTriangle());
	}

	// now check if we really can link it
	int numCommon = n1->getNumCommonVertexs(*n2);
	ASSERT(numCommon <= 3);
	if(numCommon < 2){
		ASSERT(numCommon == 1);
		// we cannot create link
		return 0;
	}

	// check if we already have that neighbor
	if(n1->hasNeighbor(n2)){
		return 0;
	}

	// we can create the new Edge
	GEdge *e = new GEdge(n1,n2);

	// calculate the distance squared distance
//	e->setWeight(n1->getSquaredDistance(*n2));

	// set the edge to the nodes
	n1->setNewEdge(e);
	n2->setNewEdge(e);

	return e;
}

////////////////////////////////////////////////////////////////////////////////
int
TriangleNavMesh::buildGraph(const TriangleVec &triangles)
{
	typedef std::map<const core::Vertex*, std::list<GNode *> > HashVertex;
	HashVertex hash;
	HashVertex::iterator it;
	const core::Vertex *v1,*v2,*v3;

	// check if the graph is not empty we remove everything
	mGraph.removeAndDestroyEdeges();
	mGraph.removeAndDestroyNodes();

	NodeVec nodes;
	EdgeVec edges;

	// for each triangle create the associated nodes and edges
	for(int i = triangles.size() - 1; i >= 0; --i){
		v1 = triangles[i]->v1;
		v2 = triangles[i]->v2;
		v3 = triangles[i]->v3;

		// create a new node
		GNode *n = new GNode(triangles[i]);
		nodes.push_back(n);

		// verify the v1
		it = hash.find(v1);
		if(it != hash.end()){
			// we have to analyze the triangles associated to this triangle
			// over the actual vertex
			std::list<GNode *>::iterator nIt;
			for(nIt = it->second.begin(); nIt != it->second.end(); ++nIt){
				GEdge *e = linkNodes(n, *nIt);
				if(e){
					// we have linked a node
					edges.push_back(e);
				}
			}
		}

		// verify the v2
		it = hash.find(v2);
		if(it != hash.end()){
			// we have to analyze the triangles associated to this triangle
			// over the actual vertex
			std::list<GNode *>::iterator nIt;
			for(nIt = it->second.begin(); nIt != it->second.end(); ++nIt){
				GEdge *e = linkNodes(n, *nIt);
				if(e){
					// we have linked a node
					edges.push_back(e);
				}
			}
		}

		// verify the v3
		it = hash.find(v3);
		if(it != hash.end()){
			// we have to analyze the triangles associated to this triangle
			// over the actual vertex
			std::list<GNode *>::iterator nIt;
			for(nIt = it->second.begin(); nIt != it->second.end(); ++nIt){
				GEdge *e = linkNodes(n, *nIt);
				if(e){
					// we have linked a node
					edges.push_back(e);
				}
			}
		}

		// add to the map the actual node
		hash[v1].push_back(n);
		hash[v2].push_back(n);
		hash[v3].push_back(n);
	}

	// set the edges and nodes
	mGraph.setNodesAndEdges(nodes, edges);


	// once we have been finish we have in edges all the edges that we will gona
	// use and in nodes all the nodes
	fillMatrix(nodes);

	debug("Number of nodes created: %zd\tNumber of edges created: %zd\n",
			nodes.size(), edges.size());

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
TriangleNavMesh::TriangleNavMesh(int numColumns, int numRows) :
mMatrix(0),
mRowCount(numRows),
mColumnCount(numColumns)
{

}

////////////////////////////////////////////////////////////////////////////////
TriangleNavMesh::~TriangleNavMesh()
{
	destroyMatrix();

}

////////////////////////////////////////////////////////////////////////////////
void
TriangleNavMesh::setNumColumnsAndRows(int numColumns, int numRows)
{
	// check if is a matrix already created, if it is, we cant change the values
	if(mMatrix){
		return;
	}
	mRowCount = numRows;
	mColumnCount = numColumns;
}



////////////////////////////////////////////////////////////////////////////////
const GNode *
TriangleNavMesh::getNodeFromPoint(const core::Point &p) const
{
	ASSERT(mMatrix);
	int r = MATRIX_GET_ROW(p.y);
	int c = MATRIX_GET_COLUMN(p.x);

	if(c < 0 || c >= mColumnCount || r < 0 || r >= mRowCount) {
	    return 0;
	}

	// check if there are some Node checking the triangles
	const NodeVec &vec = mMatrix[r][c];

	for(size_t i = 0, size = vec.size(); i < size; ++i){
		// check if the point is inside the triangle
		ASSERT(vec[i]->getTriangle());
		if(vec[i]->getTriangle()->containsPoint(p)){
			return vec[i];
		}
	}

	// there are no node intersecting the point
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
void
TriangleNavMesh::addTriangle(const Triangle *t)
{
	ASSERT(t);
	mTriangles.push_back(t);
}

void
TriangleNavMesh::addTriangles(const std::vector<Triangle *> &triangles)
{
#ifdef DEBUG
    for(size_t i = 0, size = triangles.size(); i < size; ++i){
        ASSERT(triangles[i]);
    }
#endif
    mTriangles.insert(mTriangles.end(), triangles.begin(), triangles.end());
}
////////////////////////////////////////////////////////////////////////////////
int
TriangleNavMesh::build(void)
{
	// get the size of the level
	core::Point tl, br;
	getTrianglesBoundingBox(mTriangles, tl, br);
	ASSERT(tl.x < br.x);
	ASSERT(tl.y > br.y);
	ASSERT(tl.x >= 0);
	ASSERT(br.y >= 0);

	// we force to start in the 0 position
	tl.x = 0;
	br.y = 0;

	// create the matrix with a size of 50*50 (with 24 bytes of vectors) =
	// 24 * 50 * 50 ~ 60kb (more performance)
	debugOPTIMIZATION("TODO: cambiar esto si tenemos que los niveles no son tan "
			"cuadrados\n");
	createMatrix(mRowCount,mColumnCount);

	// set the factor values
	setFactorsPosition(br.x - tl.x, tl.y - br.y);
	debug("Level Bounding Box: Xsize: %f\t YSize: %f\n", br.x - tl.x, tl.y - br.y);

	// create all the nodes and edges
	int result = buildGraph(mTriangles);
	// clear all the memory
	mTriangles.clear();

	return result;
}

////////////////////////////////////////////////////////////////////////////////
void
TriangleNavMesh::destroy(void)
{
    mTriangles.clear();
    destroyMatrix();
}

////////////////////////////////////////////////////////////////////////////////
int
TriangleNavMesh::buildFromGraph(const Graph &g)
{
	// get all the triangles
	const std::vector<GNode *> &nodes = g.getNodes();
	mTriangles.clear();
	mTriangles.reserve(nodes.size());
	for(int i = nodes.size()-1; i >= 0; --i){
		ASSERT(nodes[i]->getTriangle());
		mTriangles.push_back(nodes[i]->getTriangle());
	}

	// get the size of the level
	core::Point tl, br;
	getTrianglesBoundingBox(mTriangles, tl, br);
	ASSERT(tl.x < br.x);
	ASSERT(tl.y > br.y);
	ASSERT(tl.x >= 0);
	ASSERT(br.y >= 0);

	// we force to start in the 0 position
	tl.x = 0;
	br.y = 0;

	// create the matrix with a size of 50*50 (with 24 bytes of vectors) =
	// 24 * 50 * 50 ~ 60kb (more performance)
	debugOPTIMIZATION("TODO: cambiar esto si tenemos que los niveles no son tan "
				"cuadrados\n");
	createMatrix(mRowCount,mColumnCount);

	// set the factor values
	setFactorsPosition(br.x - tl.x, tl.y - br.y);
	debug("Level Bounding Box: Xsize: %f\t YSize: %f\n", br.x - tl.x, tl.y - br.y);

	// set the new graph
	if(!mGraph.getEdges().empty()){
		debug("Removeing the edges of the old graph\n");
		mGraph.removeAndDestroyEdeges();
	}
	if(!mGraph.getNodes().empty()){
		debug("Removeing the nodes of the old graph\n");
		mGraph.removeAndDestroyNodes();
	}
	mGraph.setNodesAndEdges(g.getNodes(),g.getEdges());

	// fill the matrix
	fillMatrix(g.getNodes());

	// TODO: chequear que realmente sea valido el grafo.

	return 0;

}

