/*
 * PrimitiveDrawer.cpp
 *
 *  Created on: 07/02/2012
 *      Author: agustin
 */


#ifdef DEBUG

#include <OgreMaterialManager.h>
#include <OgreMaterial.h>

#include "GlobalObjects.h"

#include "PrimitiveDrawer.h"



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

void DrawablePath::drawPath(const std::vector<core::Point> &path, int color)
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



PrimitiveDrawer::PrimitiveDrawer()
{
	static bool materialsCreated = false;

	if(!materialsCreated){
		// red
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("redMat", "General");
		mat->setReceiveShadows(false);
		mat->getTechnique(0)->setLightingEnabled(true);
		mat->getTechnique(0)->getPass(0)->setDiffuse(1,0,0,0);
		mat->getTechnique(0)->getPass(0)->setAmbient(1,0,0);
		mat->getTechnique(0)->getPass(0)->setSelfIllumination(1,0,0);

		// green
		Ogre::MaterialPtr green = Ogre::MaterialManager::getSingleton().create("greenMat", "General");
		green->setReceiveShadows(false);
		green->getTechnique(0)->setLightingEnabled(true);
		green->getTechnique(0)->getPass(0)->setDiffuse(0,1,0,0);
		green->getTechnique(0)->getPass(0)->setAmbient(0,1,0);
		green->getTechnique(0)->getPass(0)->setSelfIllumination(0,1,0);

		// blue
		Ogre::MaterialPtr blue = Ogre::MaterialManager::getSingleton().create("blueMat", "General");
		blue->setReceiveShadows(false);
		blue->getTechnique(0)->setLightingEnabled(true);
		blue->getTechnique(0)->getPass(0)->setDiffuse(1,0,1,0);
		blue->getTechnique(0)->getPass(0)->setAmbient(1,0,1);
		blue->getTechnique(0)->getPass(0)->setSelfIllumination(1,0,1);

		materialsCreated = true;
	}
}


// Draw a line
PrimitiveDrawer::Line *PrimitiveDrawer::createNewLine(const Ogre::Vector3 &p1,
		const Ogre::Vector3 &p2, int color)
{
	Line *l = new Line;
	l->manual = GLOBAL_SCN_MNGR->createManualObject();

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
	l->manual->begin(colorstr, Ogre::RenderOperation::OT_LINE_STRIP);

	Ogre::Real height = 10.0f;
	l->manual->position(p1);  // start position
	l->manual->position(p2);  // start position

	l->manual->end();
	l->node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	l->node->attachObject(l->manual);
	mLines.push_back(l);

	return l;
}

void PrimitiveDrawer::destroyLine(Line *l)
{
	ASSERT(l);
	ASSERT(l->manual);
	ASSERT(l->node);

	GLOBAL_SCN_MNGR->destroyManualObject(l->manual);
	GLOBAL_SCN_MNGR->getRootSceneNode()->removeAndDestroyChild(l->node->getName());
	delete l;
}

void PrimitiveDrawer::destroyAllLines(void)
{
	for(int i = mLines.size() -1; i >= 0; --i){
		destroyLine(mLines[i]);
	}
	mLines.clear();
}

// Draw VolumePoint
PrimitiveDrawer::VPoint *PrimitiveDrawer::createNewVPoint(const Ogre::Vector3 &p1, int color)
{
	VPoint *r = new VPoint;
	r->node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	r->ent = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_CUBE);


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

	r->ent->setMaterialName(colorstr);
	r->node->attachObject(r->ent);
	r->node->setPosition(p1);
	r->node->setScale(0.12,0.12,0.12);

	mVPoints.push_back(r);
	return r;
}

void PrimitiveDrawer::destroyVPoint(VPoint *l)
{
	ASSERT(l);
	ASSERT(l->ent);
	ASSERT(l->node);
	GLOBAL_SCN_MNGR->destroyEntity(l->ent);
	GLOBAL_SCN_MNGR->getRootSceneNode()->removeAndDestroyChild(l->node->getName());
	delete l;
}

void PrimitiveDrawer::destroyAllVPoints(void)
{
	for(int i = mVPoints.size() - 1; i >= 0; --i)destroyVPoint(mVPoints[i]);
	mVPoints.clear();
}


PrimitiveDrawer::Box *PrimitiveDrawer::createBox(const core::AABB &box)
{
	Box *r = new Box;
	r->node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
//	r->ent = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_CUBE);
//
//	// scale the box
//	Ogre::Real heightSize = r->ent->getBoundingBox().getCorner(
//			Ogre::AxisAlignedBox::FAR_LEFT_BOTTOM).length();
//	Ogre::Real widthSize = r->ent->getBoundingBox().getCorner(
//			Ogre::AxisAlignedBox::NEAR_RIGHT_BOTTOM).length();
//
//	heightSize = box.getHeight() / heightSize;
//	widthSize = box.getWidth( ) / widthSize;
//
//	r->node->scale(heightSize, 1, widthSize);
//	debug("ScalaX:%f, scalaY:%f, x:%f, y:%f\n",
//			box.getWidth()*r->ent->getBoundingBox().getHalfSize().length()*2.0,
//			box.getHeight()*r->ent->getBoundingBox().getHalfSize().length()*2.0,
//			box.pos.x, box.pos.y);

	r->ent = GLOBAL_SCN_MNGR->createManualObject();

	Ogre::String colorstr;
	colorstr = "greenMat";

	r->ent->begin(colorstr, Ogre::RenderOperation::OT_LINE_STRIP);

	r->ent->position(Ogre::Vector3(box.tl.x, 0, box.tl.y));  // start position
	r->ent->position(Ogre::Vector3(box.br.x, 0, box.tl.y));  // start position
	r->ent->position(Ogre::Vector3(box.br.x, 0, box.br.y));  // start position
	r->ent->position(Ogre::Vector3(box.tl.x, 0, box.br.y));  // start position
	r->ent->position(Ogre::Vector3(box.tl.x, 0, box.tl.y));  // start position
	r->ent->end();


	r->node->attachObject(r->ent);
	r->node->setPosition(0, 8, 0);

	mBoxes.push_back(r);

	return r;

}
void PrimitiveDrawer::destroyBox(Box *l)
{
	ASSERT(l);
	ASSERT(l->ent);
	ASSERT(l->node);
	GLOBAL_SCN_MNGR->destroyManualObject(l->ent);
	GLOBAL_SCN_MNGR->getRootSceneNode()->removeAndDestroyChild(l->node->getName());
	delete l;

}
void PrimitiveDrawer::destroyAllBoxes(void)
{
	for(int i = mBoxes.size() - 1; i >= 0; --i)destroyBox(mBoxes[i]);

	mBoxes.clear();
}


#endif
