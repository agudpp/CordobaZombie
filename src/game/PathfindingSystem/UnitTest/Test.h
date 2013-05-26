/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef TEST_H_
#define TEST_H_

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>

#include "AppTester.h"
#include "DebugUtil.h"

#include "TriNavMeshBuilder.h"
#include "TriangleNavMesh.h"
#include "TriangleAStar.h"
#include "TriangleFunnel.h"
#include "PathfindingManager.h"
#include "CameraController.h"
#include "MouseCursor.h"


class DrawablePath {
	enum {
		COLOR_GREEN,
		COLOR_BLUE,
		COLOR_RED
	};
public:
	DrawablePath()
	{
	}
	~DrawablePath();

	void drawPath(const std::vector<math::Vertex> &path, int color = COLOR_BLUE);

	void clearPath(void);

private:
	Ogre::SceneNode *getFreshSceneNode(int i);
	Ogre::ManualObject* createLine(const Ogre::Vector3 &p1,
			const Ogre::Vector3 &p2, int color);

private:
	std::vector<Ogre::SceneNode *>	mPoints;
	std::vector<Ogre::SceneNode *> mLines;
};


class Test : public AppTester
{
public:
	Test();

	/* Load additional info */
	void loadAditionalData(void);

	/* function called every frame. Use GlobalObjects::lastTimeFrame */
	void update();


	~Test();

private:
	// draw a box in the middle of a triangle for a PolyStructoscontainer
	void drawTrianglesBox(const PolyStructsContainer<Triangle *> &cont);

	void drawTriangle(const Triangle *t);

	// Perform ray
	bool performMouseRay(Ogre::Vector3 &v);

	// handle input
	void handleInput(void);

private:
	PolyStructsContainer<math::Vertex *> mVvertexs;
	PolyStructsContainer<Triangle *> mTriangles;
	bool 		mKeyPressed;
	int			mPlacesCount;
	CameraController		mCamController;
	MouseCursor				mMouseCursor;

};

#endif /* TEST_H_ */
