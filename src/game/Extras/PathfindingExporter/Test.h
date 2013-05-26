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


	// handle input
	void handleInput(void);

private:
	PolyStructsContainer<core::Vertex *> mVvertexs;
	PolyStructsContainer<Triangle *> mTriangles;
	TriangleNavMesh mTnm;
	TriangleAStar mTAStar;
	bool 		mKeyPressed;
	int			mPlacesCount;
	TriangleFunnel mTriFunnel;

};

#endif /* TEST_H_ */
