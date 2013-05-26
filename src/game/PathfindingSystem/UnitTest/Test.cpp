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
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreString.h>

#include "GlobalObjects.h"
#include "Test.h"


Ogre::SceneNode *DrawablePath::getFreshSceneNode(int i)
{
	if(mPoints.size() <= i){
		// create a new one
		Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
		mPoints.push_back(node);
		return node;
	} else {
		Ogre::SceneNode *node = mPoints[i];
		node->detachAllObjects();
		node->setInitialState();
		return node;
	}
}


Ogre::ManualObject* DrawablePath::createLine(const Ogre::Vector3 &p1,
		const Ogre::Vector3 &p2, int color)
{
	Ogre::ManualObject* manual = GLOBAL_SCN_MNGR->createManualObject();

	Ogre::String colorstr;
	switch(color){
	case COLOR_BLUE:
		colorstr = "blueMat";
		break;
	case COLOR_RED:
		colorstr = "redMat";
		break;
	case COLOR_GREEN:
		colorstr = "greenMat";
		break;
	default:
		ASSERT(false);
	}
	manual->begin(colorstr, Ogre::RenderOperation::OT_LINE_STRIP);

	Ogre::Real height = 10.0f;
	manual->position(p1);  // start position
	manual->position(p2);  // start position

	manual->end();
	Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	node->attachObject(manual);
	mLines.push_back(node);

	return manual;
}

DrawablePath::~DrawablePath()
{
	clearPath();
}

void DrawablePath::drawPath(const std::vector<math::Vertex> &path, int color)
{
	Ogre::String colorstr;
	switch(color){
	case COLOR_BLUE:
		colorstr = "blueMat";
		break;
	case COLOR_RED:
		colorstr = "redMat";
		break;
	case COLOR_GREEN:
		colorstr = "greenMat";
		break;
	default:
		ASSERT(false);
	}
	for(int i = 0; i < path.size(); ++i){
		Ogre::SceneNode *node = getFreshSceneNode(i);
		// Create an entity small entity
		Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_SPHERE);
		ent->setMaterialName(colorstr);
		node->attachObject(ent);
		node->setScale(0.12f,0.12f,0.12f);
		node->setPosition(path[i].x, 0, path[i].y);
		if(i > 0){
			// create a line
			Ogre::ManualObject *line = createLine(mPoints[i-1]->getPosition(),
					mPoints[i]->getPosition(), color);
//			node->attachObject(line);
		}
	}

}

void DrawablePath::clearPath(void)
{
	for(int i = mPoints.size() - 1; i>= 0; --i){
		GLOBAL_SCN_MNGR->destroyEntity(static_cast<Ogre::Entity *>(
				mPoints[i]->getAttachedObject(0)));
		mPoints[i]->detachAllObjects();
		GLOBAL_SCN_MNGR->getRootSceneNode()->removeAndDestroyChild(mPoints[i]->getName());
	}
	mPoints.clear();
	for(int i = mLines.size() - 1; i>= 0; --i){
		GLOBAL_SCN_MNGR->destroyManualObject(
				static_cast<Ogre::ManualObject *>(mLines[i]->getAttachedObject(0)));
	}
	mLines.clear();
}


void Test::drawTriangle(const Triangle *t)
{
	Ogre::ManualObject* manual = GLOBAL_SCN_MNGR->createManualObject();
	manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);

	Ogre::Real height = 5.0f;
	manual->position(t->v1->x, height, t->v1->y);  // start position
	manual->position(t->v2->x, height, t->v2->y);  // start position
	manual->position(t->v3->x, height, t->v3->y);  // start position
	manual->position(t->v1->x, height, t->v1->y);  // start position

	manual->end();
	GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode()->attachObject(manual);
}

bool Test::performMouseRay(Ogre::Vector3 &v)
{
	// set up the ray
	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mMouseCursor.getXRelativePos(),
			mMouseCursor.getYRelativePos());

	// check if the ray intersects our plane
	// intersects() will return whether it intersects or not (the bool value) and
	// what distance (the Real value) along the ray the intersection is
	std::pair<bool, Ogre::Real> result = mouseRay.intersects(mGlobalPlane);

	if (result.first)
	{
		// if the ray intersect the plane, we have a distance value
		// telling us how far from the ray origin the intersection occurred.
		// the last thing we need is the point of the intersection.
		// Ray provides us getPoint() function which returns a point
		// along the ray, supplying it with a distance value.

		// get the point where the intersection is
		v = mouseRay.getPoint(result.second);
		return true;
	}

	return false;
}


Test::Test()
{
	setUseDefaultInput(false);
	mMouseCursor.setImage("cursor.png");
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight());

	// init the material colors
	DrawablePath p;


	math::Point p1(10,100);
	math::Point p2(100, 10);
	if(p1.crossProduct(p2) < 0.0f){
		debug("P1 to p2 clockwise %f\n", p1.crossProduct(p2));
	} else {
		debug("P1 to p2 counterclockwise %f\n", p1.crossProduct(p2));
	}
	if(p2.crossProduct(p1) < 0.0f){
		debug("P2 to p1 clockwise %f\n", p2.crossProduct(p1));
	} else {
		debug("P2 to p1 counterclockwise %f\n", p2.crossProduct(p1));
	}
	if(p1.crossProduct(p1) < 0.0f){
		debug("P1 to p1 clockwise %f\n", p1.crossProduct(p1));
	} else {
		debug("P1 to p1 counterclockwise %f\n", p1.crossProduct(p1));
	}
	math::Point p3(-10, -100);
	if(p1.crossProduct(p3) < 0.0f){
		debug("P1 to p3 clockwise %f\n", p1.crossProduct(p3));
	} else {
		debug("P1 to p3 counterclockwise %f\n", p1.crossProduct(p3));
	}
	if(p2.crossProduct(p3) < 0.0f){
		debug("P2 to p3 clockwise %f\n", p2.crossProduct(p3));
	} else {
		debug("P2 to p3 counterclockwise %f\n", p2.crossProduct(p3));
	}
	if(p3.crossProduct(p1) < 0.0f){
		debug("P3 to p1 clockwise %f\n", p3.crossProduct(p1));
	} else {
		debug("P3 to p1 counterclockwise %f\n", p3.crossProduct(p1));
	}
	if(p3.crossProduct(p2) < 0.0f){
		debug("P3 to p2 clockwise %f\n", p3.crossProduct(p2));
	} else {
		debug("P3 to p2 counterclockwise %f\n", p3.crossProduct(p2));
	}

	p1.x = 10; p1.y = -10;
	p2.x = 10; p2.y = -9;
	if(p1.crossProduct(p2) < 0.0f){
		debug("P1 to p2 clockwise %f\n", p1.crossProduct(p2));
	} else {
		debug("P1 to p2 counterclockwise %f\n", p1.crossProduct(p2));
	}
	p2.x = -10;
	if(p1.crossProduct(p2) < 0.0f){
		debug("P1 to p2 clockwise %f\n", p1.crossProduct(p2));
	} else {
		debug("P1 to p2 counterclockwise %f\n", p1.crossProduct(p2));
	}

}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}

// handle input
void Test::handleInput(void)
{
	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
		// we have to exit
		mStopRunning = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// CAMERA
//	float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
	Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;
	Ogre::Real zoom = 0.0f;

	// HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
	// ARROWS TO MOVE THE CAMERA
	// NOTE: we are getting the cameraPosition and modifying the position
	// without using translation, this is because we want to move always
	// in the same axis whatever be the direction of the camera.


	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	if(mKeyboard->isKeyDown(OIS::KC_LEFT) || mKeyboard->isKeyDown(OIS::KC_A) ||
			lMouseState.X.abs <= 0)
	{
		mTranslationVec.x -= 1.0f;
	}
	if(mKeyboard->isKeyDown(OIS::KC_RIGHT) || mKeyboard->isKeyDown(OIS::KC_D) ||
			lMouseState.X.abs >= lMouseState.width)
	{
		mTranslationVec.x += 1.0f;
	}
	if(mKeyboard->isKeyDown(OIS::KC_Q))
	{
		zoom += 1;
	}
	if(mKeyboard->isKeyDown(OIS::KC_E))
	{
		zoom -= 1;
	}
	if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) ||
			lMouseState.Y.abs <= 0)
	{
		mTranslationVec.z -= 1.0f;
	}
	if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) ||
			lMouseState.Y.abs >= lMouseState.height)
	{
		mTranslationVec.z += 1.0f;
	}

	if(mTranslationVec != Ogre::Vector3::ZERO)
	{
		mCamController.moveCamera(mTranslationVec);
	}
	if(zoom != 0.0f){
		mCamController.zoomCamera(zoom);
	}

	if(mKeyboard->isKeyDown(OIS::KC_R))
	{
		mCamController.rotateCameraY(Ogre::Radian(-1));
	}
	if(mKeyboard->isKeyDown(OIS::KC_T))
	{
		mCamController.rotateCameraY(Ogre::Radian(1));
	}

	if(mKeyboard->isKeyDown(OIS::KC_Z))
	{
		mCamController.rotateCameraX(Ogre::Radian(-1));
	}
	if(mKeyboard->isKeyDown(OIS::KC_X))
	{
		mCamController.rotateCameraX(Ogre::Radian(1));
	}


	static bool mousePressed = false;
	// Mouse input check
	if(GLOBAL_MOUSE->getMouseState().buttonDown(OIS::MB_Left)){
		if (!mousePressed)
		{
			mousePressed = true;

			Ogre::Vector3 v;
			if(!performMouseRay(v)){
				return;
			}

			static math::Point p1, p2;

			if (mPlacesCount < 1)
			{
				++mPlacesCount;
				p1.x = v.x;
				p1.y = v.z;
			}
			else
			{
				mPlacesCount = 0;
				p2.x = v.x;
				p2.y = v.z;

				std::vector<math::Vertex> v;
				PATHFINDER_INSTANCE.getPath(p1, p2, v, 20.0f);
				debug("ShortestPath of size: %d\n", v.size());
				static DrawablePath dwp;
				dwp.clearPath();
				dwp.drawPath(v);

			}
		}
	} else {
		mousePressed = false;
	}
}


/* Load additional info */
void Test::loadAditionalData(void)
{
	//	GLOBAL_CAMERA->getParentSceneNode()->detachAllObjects();
	GLOBAL_CAMERA->detachFromParent();
	mCamController.setCamera(GLOBAL_CAMERA);

	Ogre::AxisAlignedBox box;
	box.setMaximum(2500, 1800, 2500);
	box.setMinimum(-2500,0,-2500);


	mCamController.setCameraMoveZone(box);
	mCamController.setMaxRotateX(Ogre::Radian(0), Ogre::Radian((-Ogre::Math::PI-0.1f)/2.0f));
	mCamController.setCamPos(Ogre::Vector3(500,500,500));
	mCamController.setRotationX(Ogre::Radian(-Ogre::Math::PI/2.0f));
	mCamController.setCameraVelocity(5.0f);



	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity("pathfinder.mesh");
	TriNavMeshBuilder::getTrianglesFromMesh(mVvertexs, mTriangles, ent->getMesh());

	debug("Number of vertexs: %d, Triangles size: %d\n", mVvertexs.getSize(),
			mTriangles.getSize());

	int err = PATHFINDER_INSTANCE.loadLevel(mTriangles.getObjs(), mVvertexs.getObjs());
	ASSERT(err == 0);
	std::vector<math::Vertex *> &vv = mVvertexs.getObjs();
//	for(int i = vv.size()-1; i >= 0; --i){
//		debug("Vertex[%d]: (%f, %f)\n", i, vv[i]->x, vv[i]->y);
//	}

	// draw the triangles
	std::vector<Triangle *> &vt = mTriangles.getObjs();
	for(int i = vt.size()-1; i >= 0; --i){
		drawTriangle(vt[i]);
	}
	mTriangles.clearAll();
	mVvertexs.clearAll();

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	handleInput();

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
		static math::Point p1, p2;
		if(!mKeyPressed){
			mKeyPressed = true;
			if(mPlacesCount < 1){
				++mPlacesCount;
				p1.x = GLOBAL_CAMERA->getDerivedPosition().x;
				p1.y = GLOBAL_CAMERA->getDerivedPosition().z;
			} else {
				mPlacesCount = 0;
				p2.x = GLOBAL_CAMERA->getDerivedPosition().x;
				p2.y = GLOBAL_CAMERA->getDerivedPosition().z;

				std::vector<math::Vertex> v;
				PATHFINDER_INSTANCE.getPath(p1, p2, v, 30.0f);
				debug("ShortestPath of size: %d\n", v.size());
				static DrawablePath dwp;
				dwp.clearPath();
				dwp.drawPath(v);

//				size_t pathSize = 0;
//				const TriangleAStar::Path p = mTAStar.getshortestPath(p1, p2, pathSize);
//				debug("Shortest path of size: %d\n", pathSize);
//				drawPath(p);
//
//				// funnel
//				std::vector<Vertex> v;
//				mTriFunnel.getShortestPath(p, v,
//						pathSize, p1, p2, 0.0f);
//				drawFunnel(v);
			}
		}
	} else {
		mKeyPressed = false;
	}

}
