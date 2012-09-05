/*
 * ExplosionFake.cpp
 *
 *  Created on: Apr 3, 2012
 *      Author: Emi
 */

#include "ExplosionFake.h"

int ExplosionFake::numberOfInstance = 0;

std::vector<Explosion *> *ExplosionFake::availableEffects = 0;

ExplosionFake::ExplosionFake():Effects(0.05) {

	numberOfInstance++;

	type = effectsEnum::ExplosionFake;

	std::stringstream sstm;
	sstm << "explosionFake" << numberOfInstance;
	name = sstm.str();

	availableEffects = EffectQueues::getInstance()->getExplosionQueue();



}

ExplosionFake::~ExplosionFake() {

}



void ExplosionFake::start()
{
	std::cout << "startEffect" << std::endl;

	started = true;
	ended = false;

}


void ExplosionFake::end()
{
	ended = true;
	started = false;
	mTimer = 0.0;
}


bool ExplosionFake::hasEnded(){
	return ended;
}


void ExplosionFake::update()
{

}
