/*
 * PrimitiveDrawer.h
 *
 *  Created on: 07/02/2012
 *      Author: agustin
 */

#ifndef PRIMITIVEDRAWER_H_
#define PRIMITIVEDRAWER_H_

#ifdef DEBUG

#include <OgreSceneNode.h>
#include <OgreManualObject.h>
#include <OgreEntity.h>


#include <vector>
#include "CommonMath.h"

#define DRAWER		PrimitiveDrawer::getInstance()


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

	void drawPath(const std::vector<sm::Point> &path, int color = COLOR_BLUE);

	void clearPath(void);

private:
	Ogre::SceneNode *getFreshSceneNode(int i);
	Ogre::ManualObject* createLine(const Ogre::Vector3 &p1,
			const Ogre::Vector3 &p2, int color);

private:
	std::vector<Ogre::SceneNode *>	mPoints;
	std::vector<Ogre::SceneNode *> mLines;
};

class PrimitiveDrawer
{
public:
	enum {
		COLOR_GREEN,
		COLOR_BLUE,
		COLOR_RED
	};

	struct Line {
		Line():node(0),manual(0){};
		Ogre::SceneNode *node;
		Ogre::ManualObject *manual;
	};

	struct VPoint {
		Ogre::Entity *ent;
		Ogre::SceneNode *node;
	};

	struct Box {
		Ogre::ManualObject *ent;
		Ogre::SceneNode *node;
	};

public:


	static PrimitiveDrawer &getInstance(void)
	{
		static PrimitiveDrawer instance;
		return instance;
	}

	// Draw a line
	Line *createNewLine(const Ogre::Vector3 &p1, const Ogre::Vector3 &p2,
			int color = COLOR_RED);
	void destroyLine(Line *l);
	void destroyAllLines(void);

	// Draw VolumePoint
	VPoint *createNewVPoint(const Ogre::Vector3 &p1, int color = COLOR_RED);
	void destroyVPoint(VPoint *l);
	void destroyAllVPoints(void);

	// Draw a box
	Box *createBox(const sm::AABB &box);
	void destroyBox(Box *b);
	void destroyAllBoxes(void);



private:
	PrimitiveDrawer();
	~PrimitiveDrawer(){};

private:
	std::vector<PrimitiveDrawer::Line *>		mLines;
	std::vector<PrimitiveDrawer::VPoint *>		mVPoints;
	std::vector<PrimitiveDrawer::Box *>			mBoxes;

};

#endif
#endif /* PRIMITIVEDRAWER_H_ */
