/*
 * CollObjBuilder.cpp
 *
 *  Created on: 23/04/2012
 *      Author: agustin
 */

#include "PrimitiveDrawer.h"

#include "CommonMath.h"
#include "Triangle.h"
#include "CollObjBuilder.h"
#include "Box2D.h"

struct TmpEdge {
	const sm::Vertex *v1;
	const sm::Vertex *v2;
};



void CollObjBuilder::getBoundingBox(std::vector<sm::Vertex *> &v, sm::AABB &bb)
{
	ASSERT(v.size() >= 2);

	float minx, miny, maxx, maxy;
	minx = maxx = v[0]->x;
	miny = maxy = v[0]->y;

	for(int i = v.size()-1; i >= 0; --i){
		if(minx > v[i]->x){minx = v[i]->x;}
		if(maxx < v[i]->x){maxx = v[i]->x;}
		if(miny > v[i]->y){miny = v[i]->y;}
		if(maxy < v[i]->y){maxy = v[i]->y;}
	}
	bb.tl.x = minx;
	bb.tl.y = maxy;
	bb.br.x = maxx;
	bb.br.y = miny;
}

void CollObjBuilder::getBoundingBox(std::vector<const sm::Vertex *> &v, sm::AABB &bb)
{
	ASSERT(v.size() >= 2);

	float minx, miny, maxx, maxy;
	minx = maxx = v[0]->x;
	miny = maxy = v[0]->y;

	for(int i = v.size()-1; i >= 0; --i){
		if(minx > v[i]->x){minx = v[i]->x;}
		if(maxx < v[i]->x){maxx = v[i]->x;}
		if(miny > v[i]->y){miny = v[i]->y;}
		if(maxy < v[i]->y){maxy = v[i]->y;}
	}
	bb.tl.x = minx;
	bb.tl.y = maxy;
	bb.br.x = maxx;
	bb.br.y = miny;
}


 /* Create a collision object from an entity.
 * @param 	entity
 * @return	CollisionObject
 */
CollisionObject *CollObjBuilder::createFromMesh(Ogre::Entity *ent,
		const Ogre::String &userDef)
{
	ASSERT(ent);

	PolyStructsContainer<sm::Vertex *> cont;
	PolyStructsContainer<Triangle *> triangles;

	if(!Common::Util::getTrianglesFromMesh(cont, triangles, ent->getMesh())){
		debug("Error getting triangles from mesh\n");
		return 0;
	}

	CollisionObject *result = 0;

	// construct the specified type
	if(userDef == "poly"){
		result = createPolyShape(cont, triangles);
	} else if(userDef == "circle"){
		result = createCircleShape(cont, triangles);
	} else if(userDef == "edge"){
		result = createEdgeShape(cont, triangles);
	} else if(userDef == "box"){
		result = createBoxShape(cont, triangles);
	} else if(userDef == "aabb"){
		result = createAABBShape(cont, triangles);
	} else {
		debug("unknown type %s\n", userDef.c_str());
		ASSERT(false);
	}

	cont.removeAndFreeAll();
	triangles.removeAndFreeAll();

	return result;
}


/**
 * Create Polygon shape (counterclockwise vertex)
 */
CollisionObject *CollObjBuilder::createPolyShape(PolyStructsContainer<sm::Vertex *> &cont,
		PolyStructsContainer<Triangle *> &triangles)
{
	// get the center vertex
	std::vector<Triangle *> &tri = triangles.getObjs();
	std::vector<sm::Vertex *> &vert = cont.getObjs();
	sm::Vertex *v = 0;
	bool found = false;

	debug("Creating poly with %zd triangles and %zd vertex\n",
			triangles.getSize(), vert.size());

	for(int j = vert.size()-1; j >= 0 && !found; --j){
		int middleCount = triangles.getSize();
		v = vert[j];
		for(int i = tri.size() - 1; i >= 0; --i){
			if(tri[i]->v1 == v || tri[i]->v2 == v || tri[i]->v3 == v){
				middleCount--;
				if(middleCount <= 0){
					found = true;
					break;
				}
			}
		}
	}

	if(!found){
		debug("The mesh is not right, we have a triangle that not share "
				"the middle vertex\n");
		ASSERT(false);
		return 0;
	}

	debug("\n");

	// else we have the middle vertex that is v, we have now to remove all the
	// edges that are connected with this edge
	std::vector<TmpEdge> edges;
	TmpEdge edge;
	for(int i = tri.size()-1; i >= 0; --i){
		if(v == tri[i]->v1){edge.v1 = tri[i]->v3; edge.v2 = tri[i]->v2;}
		else if(v == tri[i]->v2){edge.v1 = tri[i]->v1; edge.v2 = tri[i]->v3;}
		else if(v == tri[i]->v3){edge.v1 = tri[i]->v1; edge.v2 = tri[i]->v2;}
		else {ASSERT(false);}

		// save the edge
		edges.push_back(edge);
	}
	debug("edges calculated: %zd\n", edges.size());

	// save the vector of vertices without repetition
	// FUCKING UGLY SLOW!
	std::vector<const sm::Vertex *> vSet;
	vSet.push_back(edges[edges.size()-1].v1);
	for(int i = edges.size()-1; i >= 0; --i){
		for(int j = 0; j < edges.size(); ++j){
			if(edges[i].v1 == vSet.back()) {
				vSet.push_back(edges[i].v2);
				break;
			} else if(edges[i].v2 == vSet.back()){
				vSet.push_back(edges[i].v1);
				break;
			} else {
				continue;
			}
		}
	}

	debug("Real number of vertex %zd\n", vSet.size());

	//We build the b2PolyShape
	int arraySize = vSet.size();
	b2PolygonShape* polyShape = new b2PolygonShape();
	b2Vec2 vertex[arraySize];

	//We pass the information of the vSet to the Box2D shape vertex
	for(int i= 0; i< arraySize; i++){
		vertex[i].x = vSet[i]->x;
		vertex[i].y = vSet[i]->y;
	}
	polyShape->Set(vertex, arraySize);
	//ASSERT(false);

	// Now create the collision object
	CollisionObject *result = new CollisionObject;
	
	//We set the real shape
	result->b2dData = new CollisionObject::Box2DData;
	result->b2dData->realShape = polyShape;

	// get the bounding box
	getBoundingBox(vSet, result->bb);

	return result;
}

/**
 * Create circle shape
 */
CollisionObject *CollObjBuilder::createCircleShape(PolyStructsContainer<sm::Vertex *> &cont,
		PolyStructsContainer<Triangle *> &triangles)
{
	std::vector<Triangle *> &tri = triangles.getObjs();
	std::vector<sm::Vertex *> &vert = cont.getObjs();

	ASSERT(vert.size() == 2);

	// get the radius
	float radius = vert[0]->squaredDistance(*vert[1]);

	// create the object
	CollisionObject *result = new CollisionObject;

	//We build the b2CircleShape with the radius information
	b2CircleShape* circleShape = new b2CircleShape();
	circleShape->m_radius = radius;

	//We set the b2CircleShape to the collisionObject
	result->b2dData = new CollisionObject::Box2DData;
	result->b2dData->realShape = circleShape;
	//ASSERT(false);

	result->bb.setSize(radius *2, radius *2);

	return result;
}

/**
 * Edge shape
 */
CollisionObject *CollObjBuilder::createEdgeShape(PolyStructsContainer<sm::Vertex *> &cont,
		PolyStructsContainer<Triangle *> &triangles)
{
	std::vector<Triangle *> &tri = triangles.getObjs();
	std::vector<sm::Vertex *> &vert = cont.getObjs();

	ASSERT(vert.size() == 2);

	//vert[0] and vert[1] have the edges...
	CollisionObject *result = new CollisionObject;

	//We build the b2EdgeShape
	b2EdgeShape* edgeShape = new b2EdgeShape();
	b2Vec2 vertice1, vertice2;

	//Set the two vertices of the edge
	vertice1.x = vert[0]->x;
	vertice1.y = vert[0]->y;
	vertice2.x = vert[1]->x;
	vertice2.x = vert[1]->x;

	edgeShape->Set(vertice1, vertice2);

	//We set the edgeShape to the CollisionObject
	result->b2dData = new CollisionObject::Box2DData;
	result->b2dData->realShape = edgeShape;

	getBoundingBox(vert, result->bb);

	return result;
}

/**
 * Box shape
 */
CollisionObject *CollObjBuilder::createBoxShape(PolyStructsContainer<sm::Vertex *> &cont,
		PolyStructsContainer<Triangle *> &triangles)
{
	std::vector<Triangle *> &tri = triangles.getObjs();
	std::vector<sm::Vertex *> &vert = cont.getObjs();

	ASSERT(vert.size() == 4);

	std::vector<const sm::Vertex *> shared;
	std::vector<const sm::Vertex *> notShared;

	//get the unshared
	const sm::Vertex *v = 0;
	Triangle *t = 0;
	for(int j = 0; j < 2; j++){
		t = tri[j];
		for(int i = 0; i < 3; ++i){
			if(i == 0) v = t->v1;
			else if(i == 0) v = t->v2;
			else if(i == 0) v = t->v3;
			else ASSERT(false);
			if(v != t->v1 && v != t->v2 && v != t->v3){
				notShared.push_back(v);
			} else {
				if(std::find(shared.begin(), shared.end(), v) == shared.end()){
					shared.push_back(v);
				}
			}
		}
	}

	ASSERT(notShared.size() == 2);
	ASSERT(shared.size() == 2);

	// create the vector
	std::vector<const sm::Vertex *> vertexs;
	vertexs.push_back(shared[0]);
	vertexs.push_back(notShared[0]);
	vertexs.push_back(shared[1]);
	vertexs.push_back(notShared[1]);

	CollisionObject *result = new CollisionObject;

	b2PolygonShape* boxShape = new b2PolygonShape();
	b2Vec2 vertex[4];

	//We pass the information of the vertexs to the Box2D shape vertex
	for(int i= 0; i< 4; i++){
		vertex[i].x = vertexs[i]->x;
		vertex[i].y = vertexs[i]->y;
	}
	boxShape->Set(vertex, 4);
	//ASSERT(false);

	//We set the boxShape to the CollisionObject
	result->b2dData = new CollisionObject::Box2DData;
	result->b2dData->realShape = boxShape;
	
	getBoundingBox(vertexs,result->bb);

	return result;
}

/**
 * AABB shape
 */
CollisionObject *CollObjBuilder::createAABBShape(PolyStructsContainer<sm::Vertex *> &cont,
		PolyStructsContainer<Triangle *> &triangles)
{
	std::vector<Triangle *> &tri = triangles.getObjs();
	std::vector<sm::Vertex *> &vert = cont.getObjs();

	ASSERT(vert.size() == 4);

	// we don't need the box2d object
	CollisionObject *result = new CollisionObject;

	for(int i = 0; i < vert.size(); i++){
		DRAWER.createNewVPoint(Ogre::Vector3(vert[i]->x, 0,vert[i]->y));
	}

	getBoundingBox(vert, result->bb);

	return result;
}



