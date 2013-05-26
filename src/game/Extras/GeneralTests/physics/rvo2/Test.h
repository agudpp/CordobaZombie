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

#include <vector>
#include <RVO.h>
#include <RVOSimulator.h>

#include "AppTester.h"
#include "DebugUtil.h"
#include "PrimitiveDrawer.h"


class GameUnitT {
public:
	GameUnitT(float radius);
	~GameUnitT();

	void updatePosition(const Ogre::Vector3 &p)
	{
		node->setPosition(p);
	}
	void updatePosition(const RVO::Vector2 &p)
	{
		node->setPosition(p.x(), 10, p.y());
	}

	size_t agentNo;
	Ogre::Entity *ent;
	Ogre::SceneNode *node;
};


class Test : public AppTester
{
	const static Ogre::Real	RANDOM_POSITION		= 200.0f;
public:
	Test();

	/* Load additional info */
	void loadAditionalData(void);

	/* function called every frame. Use GlobalObjects::lastTimeFrame */
	void update();


	~Test();

private:
	void drawObstacle(std::vector<RVO::Vector2> &obstacle1);
	void setupScenario(RVO::RVOSimulator* sim);
	void setPreferredVelocities(RVO::RVOSimulator* sim);
	bool reachedGoal(RVO::RVOSimulator* sim);


private:
	Ogre::SceneNode *node;
	Ogre::Entity *ent;
	std::vector<GameUnitT>	mUnits;
	std::vector<RVO::Vector2> goals;
	RVO::RVOSimulator* sim;


};

#endif /* TEST_H_ */
