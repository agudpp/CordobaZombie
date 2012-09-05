/*
 * triangleTest.cpp
 *
 *  Created on: 20/01/2012
 *      Author: agustin
 *
 */



#include <set>
#include <deque>
#include <iostream>
#include "Triangle.h"
#include "DebugUtil.h"
#include "PolyStructsContainer.h"
#include "TriangleNavMesh.h"


#define INVRT_COORD_X(x)	(800-x)
#define INVRT_COORD_Y(y)	(600-y)

typedef PolyStructsContainer<Vertex *>	VertexContainer;


static std::vector<Triangle> 		navMeshTriangles;

static void createTriangle(float x1, float y1,
		float x2, float y2, float x3, float y3)
{
	Vertex *v1 = new Vertex(x1,y1);
	Vertex *v2 = new Vertex(x2, y2);
	Vertex *v3 = new Vertex(x3,y3);
	VertexContainer::getInstance().addObj(v1);
	VertexContainer::getInstance().addObj(v2);
	VertexContainer::getInstance().addObj(v3);

	navMeshTriangles.push_back(Triangle(v1,v2,v3));
}

static void createSimpletriangles(void)
{
	// see the "testingmapsimple.png"
	Vertex *v1 = new Vertex(0,INVRT_COORD_Y(0));
	Vertex *v2 = new Vertex(0,INVRT_COORD_Y(600));
	Vertex *v3 = new Vertex(280,INVRT_COORD_Y(433));
	Vertex *v4 = new Vertex(280,INVRT_COORD_Y(249));
	Vertex *v5 = new Vertex(800,INVRT_COORD_Y(0));
	Vertex *v6 = new Vertex(509,INVRT_COORD_Y(256));
	Vertex *v7 = new Vertex(800,INVRT_COORD_Y(600));
	Vertex *v8 = new Vertex(514,INVRT_COORD_Y(435));

	VertexContainer::getInstance().addObj(v1);
	VertexContainer::getInstance().addObj(v2);
	VertexContainer::getInstance().addObj(v3);
	VertexContainer::getInstance().addObj(v4);
	VertexContainer::getInstance().addObj(v5);
	VertexContainer::getInstance().addObj(v6);
	VertexContainer::getInstance().addObj(v7);
	VertexContainer::getInstance().addObj(v8);

	navMeshTriangles.push_back(Triangle(v1,v2,v3));
	navMeshTriangles.push_back(Triangle(v1,v4,v3));
	navMeshTriangles.push_back(Triangle(v1,v4,v5));
	navMeshTriangles.push_back(Triangle(v4,v6,v5));
	navMeshTriangles.push_back(Triangle(v7,v6,v5));
	navMeshTriangles.push_back(Triangle(v7,v6,v8));
	navMeshTriangles.push_back(Triangle(v7,v2,v8));
	navMeshTriangles.push_back(Triangle(v2,v3,v8));
//
//	createTriangle(1,INVRT_COORD_Y(0),
//			280,INVRT_COORD_Y(432),
//			1, INVRT_COORD_Y(596));
//
//	createTriangle(1,INVRT_COORD_Y(0),
//			280,INVRT_COORD_Y(251),
//			278, INVRT_COORD_Y(430));
//
//	createTriangle(1,INVRT_COORD_Y(0),
//			280,INVRT_COORD_Y(250),
//			800, INVRT_COORD_Y(0));
//
//	createTriangle(280,INVRT_COORD_Y(250),
//			800,INVRT_COORD_Y(0),
//			510, INVRT_COORD_Y(260));
//
//	createTriangle(800,INVRT_COORD_Y(0),
//			800,INVRT_COORD_Y(600),
//			510, INVRT_COORD_Y(260));
//
//	createTriangle(800,INVRT_COORD_Y(600),
//			515,INVRT_COORD_Y(434),
//			510, INVRT_COORD_Y(260));
//
//	createTriangle(800,INVRT_COORD_Y(600),
//			515,INVRT_COORD_Y(434),
//			0, INVRT_COORD_Y(600));
//
//	createTriangle(0,INVRT_COORD_Y(600),
//			515,INVRT_COORD_Y(434),
//			280, INVRT_COORD_Y(432));

}

static TriangleNavMesh *createSimpleNavMesh(void)
{
	TriangleNavMesh *result = new TriangleNavMesh(5,5);
	navMeshTriangles.clear();
	createSimpletriangles();

	for(int i = 0; i < navMeshTriangles.size(); ++i){
		result->addTriangle(&navMeshTriangles[i]);
	}

	int err = result->build();
	debug("Creating the navMesh: %d\n", err);

	return result;
}

static void checkNode(const TriangleNavMesh::Node *node1, int pos)
{
	debug("Checking node %d\n", pos);
	ASSERT(node1);
	ASSERT(node1->triangle);
	ASSERT(node1->triangle == &(navMeshTriangles[pos]));
}

static int getNodePos(const TriangleNavMesh::Node *node1)
{
	for(int i = navMeshTriangles.size()-1; i >= 0; --i){
		if(&navMeshTriangles[i] == node1->triangle){
			return i;
		}
	}
	ASSERT(false);
	return -1;
}

static void printGraph(const TriangleNavMesh::Node *node)
{
	ASSERT(node);
	std::deque<const TriangleNavMesh::Node *> q;
	const TriangleNavMesh::Node *neighbor;
	std::set<const TriangleNavMesh::Node *> analyzed;
	q.push_back(node);

	while(!q.empty()){
		node = q.front();
		q.pop_front();
		analyzed.insert(node);

		std::cout << "Node[" << getNodePos(node) << "] neighbors: \n";
		if(node->e1){
			neighbor = node->e1->getNeighbor(node);
			std::cout << "Has e1 Neighbor " << getNodePos(neighbor) << " with distance: " <<
					node->e1->distance << std::endl;
			if(analyzed.find(neighbor) == analyzed.end()){
				q.push_back(neighbor);
				analyzed.insert(neighbor);
			}
		}

		if(node->e2){
			neighbor = node->e2->getNeighbor(node);
			std::cout << "Has e2 Neighbor " << getNodePos(neighbor) <<  " with distance: " <<
					node->e2->distance << std::endl;
			if(analyzed.find(neighbor) == analyzed.end()){
				q.push_back(neighbor);
				analyzed.insert(neighbor);
			}
		}

		if(node->e3){
			neighbor = node->e3->getNeighbor(node);
			std::cout << "Has e3 Neighbor " << getNodePos(neighbor) << " with distance: " <<
					node->e3->distance << std::endl;
			if(analyzed.find(neighbor) == analyzed.end()){
				q.push_back(neighbor);
				analyzed.insert(neighbor);
			}
		}


	}

	std::cout << "\n\n";
}

int main(void)
{
	TriangleNavMesh *result = createSimpleNavMesh();

	// we will test that we are getting the corrects nodes
	Point p;

	// triangle 1
	p.x = 86; p.y = INVRT_COORD_Y(400);
	const TriangleNavMesh::Node *node1 = result->getNodeFromPoint(p);
	p.x = 99; p.y = INVRT_COORD_Y(120);
	const TriangleNavMesh::Node *node2 = result->getNodeFromPoint(p);
	p.x = 299; p.y = INVRT_COORD_Y(35);
	const TriangleNavMesh::Node *node3 = result->getNodeFromPoint(p);
//
//	checkNode(node1,0);
//	checkNode(node2,1);
//	checkNode(node3,2);
//
//
	std::cout << "\n\nPrinting Graph: \n";
	printGraph(node1);
	std::cout << "\nEnd printing graph\n\n";


	delete result;
	VertexContainer::getInstance().removeAndFreeAll();
	return 0;
}
