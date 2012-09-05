/*
 * Explosion.cpp
 *
 *  Created on: Apr 3, 2012
 *      Author: Emi
 */

#include "Explosion.h"

int Explosion::numberOfInstance = 0;

Explosion::Explosion():Effects(0.05) {

	numberOfInstance++;

	type = effectsEnum::Explosion;

	std::stringstream sstm;
	sstm << "explosion" << numberOfInstance;
	name = sstm.str();

	effectPS = GLOBAL_SCN_MNGR->createParticleSystem(name, "explosion");

	effectPS->setKeepParticlesInLocalSpace(true);
	effectPS->setVisible(false);

	//Attach it to a node.
	effectNode = GLOBAL_SCN_MNGR->createSceneNode(name + Ogre::String("ExplosionNode"));
	effectNode->attachObject(effectPS);


}

Explosion::~Explosion() {

}



void Explosion::start()
{
	std::cout << "startEffect" << std::endl;
	//Clear all existing particles
	effectPS->clear();

	//Starts emitting
	effectPS->setEmitting(true);
	effectPS->setVisible(true);

	started = true;
	ended = false;


	//system("PAUSE");
}


void Explosion::end()
{
	ended = true;
	started = false;
	effectPS->setEmitting(false);
	//effectPS->setVisible(false);
	mTimer = 0.0;
	std::cout << "endEffect" << std::endl;
}


bool Explosion::hasEnded(){
	return ended;
}


void Explosion::update()
{
	if(!ended){

		//We update the timer
		mTimer += Common::GlobalObjects::lastTimeFrame;


		if (mTimer >= mEffectTime)
		{
			end();
		}
	}
}
