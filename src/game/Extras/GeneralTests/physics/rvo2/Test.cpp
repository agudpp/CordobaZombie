/*
 * Test.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreMath.h>
#include "GlobalObjects.h"
#include "Test.h"

#include <sys/time.h>


GameUnitT::GameUnitT(float radius)
{
	ent = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_SPHERE);
	node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	node->attachObject(ent);
	Ogre::Real factor = radius /ent->getBoundingRadius();
	node->setScale(factor, factor, factor);
}

GameUnitT::~GameUnitT()
{

}



static double gettimestamp(void)
{
	struct timeval now;

	if(gettimeofday(&now, 0) < 0)
		return 0.0;

	double nowDouble = static_cast<double>(now.tv_sec + (
						static_cast<double>(now.tv_usec)/1000000.0));

	return nowDouble;
}





void Test::drawObstacle(std::vector<RVO::Vector2> &o)
{
	for(int i = 0; i < o.size()-1; ++i){
		DRAWER.createNewLine(Ogre::Vector3(o[i].x(), 10, o[i].y()),
				Ogre::Vector3(o[i+1].x(), 10, o[i+1].y()));
	}
}

void Test::setupScenario(RVO::RVOSimulator* sim)
{
	/* Specify the default parameters for agents that are subsequently added. */
	float radius = 2.0f;
	sim->setAgentDefaults(15.0f, 10, 5.0f, 5.0f, radius, 150.0f);

	/*
	 * Add agents, specifying their start position, and store their goals on the
	 * opposite side of the environment.
	 */
	for (int i = 0; i < 100; ++i) {
		for (int j = 0; j < 15; ++j) {
			GameUnitT unit(radius);
			unit.agentNo = sim->addAgent(RVO::Vector2(55.0f + i * 10.0f, 55.0f + j * 10.0f));
			goals.push_back(RVO::Vector2(-1075.0f, -1075.0f));
			unit.updatePosition(RVO::Vector2(-1075.0f, -1075.0f));
			mUnits.push_back(unit);

			GameUnitT unit2(radius);
			unit2.agentNo = sim->addAgent(RVO::Vector2(-55.0f - i * 10.0f, 55.0f + j * 10.0f));
			goals.push_back(RVO::Vector2(1075.0f, -1075.0f));
			unit2.updatePosition(RVO::Vector2(1075.0f, -1075.0f));
			mUnits.push_back(unit2);

			GameUnitT unit3(radius);
			unit3.agentNo = sim->addAgent(RVO::Vector2(55.0f + i * 10.0f, -55.0f - j * 10.0f));
			goals.push_back(RVO::Vector2(-1075.0f, 1075.0f));
			unit3.updatePosition(RVO::Vector2(-1075.0f, 1075.0f));
			mUnits.push_back(unit3);

			GameUnitT unit4(radius);
			unit4.agentNo = sim->addAgent(RVO::Vector2(-55.0f - i * 10.0f, -55.0f - j * 10.0f));
			goals.push_back(RVO::Vector2(1075.0f, 1075.0f));
			unit4.updatePosition(RVO::Vector2(1075.0f, 1075.0f));
			mUnits.push_back(unit4);
		}
	}

	 /*
	   * Add (polygonal) obstacles, specifying their vertices in counterclockwise
	   * order.
	   */
	  std::vector<RVO::Vector2> obstacle1, obstacle2, obstacle3, obstacle4;

	  obstacle1.push_back(RVO::Vector2(-1010.0f, 1040.0f));
	  obstacle1.push_back(RVO::Vector2(-1040.0f, 1040.0f));
	  obstacle1.push_back(RVO::Vector2(-1040.0f, 1010.0f));
	  obstacle1.push_back(RVO::Vector2(-1010.0f, 1010.0f));

	  obstacle2.push_back(RVO::Vector2(1010.0f, 1040.0f));
	  obstacle2.push_back(RVO::Vector2(1010.0f, 1010.0f));
	  obstacle2.push_back(RVO::Vector2(1040.0f, 1010.0f));
	  obstacle2.push_back(RVO::Vector2(1040.0f, 1040.0f));

	  obstacle3.push_back(RVO::Vector2(1010.0f, -1040.0f));
	  obstacle3.push_back(RVO::Vector2(1040.0f, -1040.0f));
	  obstacle3.push_back(RVO::Vector2(1040.0f, -1010.0f));
	  obstacle3.push_back(RVO::Vector2(1010.0f, -1010.0f));

	  obstacle4.push_back(RVO::Vector2(-1010.0f, -1040.0f));
	  obstacle4.push_back(RVO::Vector2(-1010.0f, -1010.0f));
	  obstacle4.push_back(RVO::Vector2(-1040.0f, -1010.0f));
	  obstacle4.push_back(RVO::Vector2(-1040.0f, -1040.0f));

	  sim->addObstacle(obstacle1);
	  sim->addObstacle(obstacle2);
	  sim->addObstacle(obstacle3);
	  sim->addObstacle(obstacle4);

	  drawObstacle(obstacle1);
	  drawObstacle(obstacle2);
	  drawObstacle(obstacle3);
	  drawObstacle(obstacle4);

	  /* Process the obstacles so that they are accounted for in the simulation. */
	  sim->processObstacles();
}

void Test::setPreferredVelocities(RVO::RVOSimulator* sim)
{
	/*
	 * Set the preferred velocity to be a vector of unit magnitude (speed) in the
	 * direction of the goal.
	 */
#pragma omp parallel for
	for (int i = 0; i < static_cast<int> (sim->getNumAgents()); ++i) {
		RVO::Vector2 goalVector = goals[i] - sim->getAgentPosition(i);

		if (RVO::absSq(goalVector) > 1.0f) {
			goalVector = RVO::normalize(goalVector);
		}

		sim->setAgentPrefVelocity(i, goalVector);

//		/*
//		 * Perturb a little to avoid deadlocks due to perfect symmetry.
//		 */
//		float angle = rand() * 2.0f * M_PI / RAND_MAX;
//		float dist = rand() * 0.0001f / RAND_MAX;
//
//		sim->setAgentPrefVelocity(
//				i,
//				sim->getAgentPrefVelocity(i) + dist * RVO::Vector2(
//						std::cos(angle), std::sin(angle)));
	}
}

bool Test::reachedGoal(RVO::RVOSimulator* sim)
{

  /* Check if all agents have reached their goals. */
  for (size_t i = 0; i < sim->getNumAgents(); ++i) {
    if (RVO::absSq(sim->getAgentPosition(i) - goals[i]) > 20.0f * 20.0f) {
      return false;
    }
  }

  return true;
}

Test::Test()
{

}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}



/* Load additional info */
void Test::loadAditionalData(void)
{
	sim = new RVO::RVOSimulator();
	/* Set up the scenario. */
	setupScenario(sim);
	setPreferredVelocities(sim);
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	static double t1,t2,t3,t4;
	static bool keyPres = false;
	static bool runSim = false;

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
		if(!keyPres){
			keyPres = true;
			runSim = runSim ? false : true;
			std::cout << "Now runSim is: " << runSim << std::endl;
		}
	} else {
		keyPres = false;
	}

	t1 = gettimestamp();
	t3 = t2 - t1;
	if (runSim) {
		/* Specify the global time step of the simulation. */
		sim->setTimeStep(GLOBAL_TIME_FRAME * 10.0f);
		sim->doStep();

		/* Output the current position of all the agents. */
		for (size_t i = 0; i < sim->getNumAgents(); ++i) {
			mUnits[i].updatePosition(sim->getAgentPosition(i));
		}
//		setPreferredVelocities(sim);
	} else {
		for(int i = mUnits.size()-1; i >=0; --i){
			mUnits[i].updatePosition(mUnits[i].node->getPosition() +
					Ogre::Vector3(1.0,0,0));
		}
	}
  t2 = gettimestamp();


  if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_P)){
  		if(!keyPres){
  			keyPres = true;
  			std::cout << "Printing Times:\n" <<
  					"LastFrameTime: " << GLOBAL_TIME_FRAME << "\n" <<
  					"Time of sim: " << t2 - t1 << "\n" <<
  					"Time of rendering and more: " << t3 << std::endl;
  		}
  	} else {
  		keyPres = false;
  	}



}
