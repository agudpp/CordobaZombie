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

#include "GlobalObjects.h"
#include "Test.h"



// test box intersection
void Test::testBoxIntersections(void)
{
	AlignedBox box;
	Vertex v1, v2, v3;
	Triangle t(&v1,&v2,&v3);
	Point p;

	box.tl.x = 100; box.tl.y = 500;
	box.br.x = 300; box.br.y = 200;

	// box inside the triangle
	v1.x = -500; v1.y = 600;
	v2.x = 1500; v2.y = 600;
	v3.x = 150; v3.y = -500;
	ASSERT(t.intersectAlignedBox(box));

	// triangle inside the box
	v1.x = 105;	v1.y = 205;
	v2.x = 105; v2.y = 400;
	v3.x = 125; v3.y = 250;
	ASSERT(t.intersectAlignedBox(box));

	// triangle intersectiong one single vertex
	v1.x = 0; v1.y = 0;
	v2.x = 150; v2.y = 250;
	v3.x = 0; v3.y = 50;
	ASSERT(t.intersectAlignedBox(box));
	v3.y = 250;
	ASSERT(t.intersectAlignedBox(box));
	v3.y = 550;
	ASSERT(t.intersectAlignedBox(box));
	v3.x = 200;
	ASSERT(t.intersectAlignedBox(box));

	//triangle outside of the box
	v1.x = 0; v1.y = 0;
	v2.x = 5; v2.y = 5;
	v3.x = 10; v3.y = 0;
	ASSERT(!t.intersectAlignedBox(box));
	v2.x = 150;
	ASSERT(!t.intersectAlignedBox(box));
	v3.x = 350;
	ASSERT(!t.intersectAlignedBox(box));

	// intersects by lines
	v1.x = 150; v1.y = 190;
	v2.x = 50; v2.y = 300;
	v3.x = 350; v3.y = 300;
	ASSERT(t.intersectAlignedBox(box));
	v3.y = 550;
	ASSERT(t.intersectAlignedBox(box));
	v3.x = 150;
	ASSERT(t.intersectAlignedBox(box));
	v3.x = 60;
	ASSERT(t.intersectAlignedBox(box));

	//332.956818,1611.814209), br:(665.913635,1289.451416)
	//(545.716187,1552.759033), (545.716187,1611.814209), (1015.177734,1611.814209)
	box.tl.x = 332.956818; box.tl.y = 1611.814209;
	box.br.x = 665.913635; box.br.y = 1289.451416;
	v1.x = 545.716187; v1.y = 1552.759033;
	v2.x = 545.716187; v2.y = 1611.814209;
	v3.x = 1015.177734; v3.y = 1611.814209;
	ASSERT(t.intersectAlignedBox(box));

	//(665.913635,1611.814209), br:(998.870483,1289.451416)
	box.tl.x = 665.913635; box.tl.y = 1611.814209;
	box.br.x = 998.870483; box.br.y = 1289.451416;
	ASSERT(t.intersectAlignedBox(box));

	//(998.870483,1611.814209), br:(1331.827271,1289.451416)
	box.tl.x = 998.870483; box.tl.y = 1611.814209;
	box.br.x = 1331.827271; box.br.y = 1289.451416;
	ASSERT(t.intersectAlignedBox(box));

}



void Test::drawTrianglesBox(const PolyStructsContainer<Triangle *> &cont)
{
	const std::vector<Triangle *> &objs = cont.getObjs();
	for(int i = objs.size()-1; i >= 0; --i){
		Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_CUBE);
		Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

		node->attachObject(ent);
		Point p;
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

// draw path
void Test::drawPath(TriangleAStar::Path p)
{
	static std::vector<Ogre::SceneNode *> nodes;
	int count = 0;
	Point point;

	for(int i = nodes.size()-1; i >= 0; --i)nodes[i]->setVisible(false);
	while(p[count]){
		if(nodes.size() <= count){
			// create a new one
			Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_SPHERE);
			Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
			node->attachObject(ent);
			p[count]->getTriangle()->getCenterPoint(point);
			node->setScale(0.2f,0.2f,0.2f);
			node->setPosition(point.x, 0 , point.y);
			nodes.push_back(node);
		} else {
			p[count]->getTriangle()->getCenterPoint(point);
			nodes[count]->setPosition(point.x, 0 , point.y);
			nodes[count]->setVisible(true);
		}
		++count;
	}
}


// draw funnel
void Test::drawFunnel(const std::vector<Vertex> &points)
{
	debug("Drawing Funnel: %d\n", points.size());
	for(int i = points.size()-1; i>= 0; --i){
		Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_CUBE);
		Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
		node->attachObject(ent);
		node->setScale(0.2f,0.2f,0.2f);
		node->setPosition(points[i].x, 0 , points[i].y);
		debug("Point[%d](%f,%f)\n", i, points[i].x, points[i].y);
	}
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
	testBoxIntersections();

	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity("pathfinder.mesh");
	TriNavMeshBuilder::getTrianglesFromMesh(mVvertexs, mTriangles, ent->getMesh());

	debug("Number of vertexs: %d, Triangles size: %d\n", mVvertexs.getSize(),
			mTriangles.getSize());

	std::vector<Vertex *> &vv = mVvertexs.getObjs();
	for(int i = vv.size()-1; i >= 0; --i){
		debug("Vertex[%d]: (%f, %f)\n", i, vv[i]->x, vv[i]->y);
	}

	std::vector<Triangle *> &tv = mTriangles.getObjs();
	for(int i = tv.size() - 1; i >= 0; --i)
	{
		mTnm.addTriangle(tv[i]);
		drawTriangle(tv[i]);
	}
	ASSERT(mTnm.build() == 0);
	debug("finish\n");

//	drawTrianglesBox(mTriangles);

	// load the mesh pathfinding
	Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	node->attachObject(ent);
//	Ogre::Entity *mesh = GLOBAL_SCN_MNGR->createEntity("mesh.mesh");
//	node->attachObject(mesh);


//	Point p1(1635, 566);
//	Point p2(515,450);
//	size_t pathSize = 0;
//	const TriangleAStar::Path p = mTAStar.getshortestPath(p1, p2, pathSize);
//	debug("FIND FIRST PATH SIZE: %d\n", pathSize);

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
		static Point p1, p2;
		if(!mKeyPressed){
			mKeyPressed = true;
			if(mPlacesCount < 1){
				++mPlacesCount;
				p1.x = mBodySceneNode->getPosition().x;
				p1.y = mBodySceneNode->getPosition().z;
			} else {
				mPlacesCount = 0;
				p2.x = mBodySceneNode->getPosition().x;
				p2.y = mBodySceneNode->getPosition().z;

				size_t pathSize = 0;
				const TriangleAStar::Path p = mTAStar.getshortestPath(p1, p2, pathSize);
				debug("Shortest path of size: %d\n", pathSize);
				drawPath(p);

				// funnel
				std::vector<Vertex> v;
				mTriFunnel.getShortestPath(p, v,
						pathSize, p1, p2, 0.0f);
				drawFunnel(v);
			}
		}
	} else {
		mKeyPressed = false;
	}

}
