/*
 * Blood.cpp
 *
 *  Created on: Apr 1, 2012
 *      Author: Emi
 */

#include "Blood.h"

namespace effects {

int Blood::numberOfInstance = 0;

Blood::Blood():Effects(0.2) {

	numberOfInstance++;

	type = effectsEnum::Blood;

	std::stringstream sstm;
	sstm << "sangre" << numberOfInstance;
	name = sstm.str();

	effectPS = GLOBAL_SCN_MNGR->createParticleSystem(name, "explosion"); //TODO cambiar esto por blood

	effectPS->setKeepParticlesInLocalSpace(true);
	effectPS->setVisible(false);

	//Attach it to a node.
	effectNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode(name + Ogre::String("BloodNode"));
	effectNode->attachObject(effectPS);
	effectNode->setPosition(300,850,0);

}

Blood::~Blood() {

}


void Blood::start()
{
	//Clear all existing particles
	effectPS->clear();

	//Starts emitting
	effectPS->setEmitting(true);
	effectPS->setVisible(true);

	started = true;
	ended = false;


}


void Blood::end()
{
	ended = true;
	started = false;
	effectPS->setEmitting(false);
	effectPS->setVisible(true);
}

bool Blood::hasEnded(){
	return ended;
}


void Blood::update()
{
	if(!ended){

		std::cout<<"Hago una llamada al manager\n";
		//effectMgr->imprimir("prueba de referencia circular");


		//We update the timer
		mTimer += Common::GlobalObjects::lastTimeFrame;


		if (mTimer >= mEffectTime)
		{

			end();
		}
	}
}


}
