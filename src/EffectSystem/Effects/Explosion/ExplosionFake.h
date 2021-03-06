/*
 * ExplosionFake.h
 *
 *  Created on: Apr 3, 2012
 *      Author: Emi
 */

#ifndef EXPLOSIONFAKE_H_
#define EXPLOSIONFAKE_H_

#include "Effects.h"
#include <string>
#include <cstdlib>

#include <EffectQueues.h>

class ExplosionFake: public Effects {
private:
	static int numberOfInstance;

	std::string name;

	static std::vector<Explosion *> *availableEffects;

	EffectQueues * ef;



public:
	ExplosionFake();
	virtual ~ExplosionFake();


	void start();
	void end();
	bool hasEnded();
	void update();

};

#endif /* EXPLOSIONFAKE_H_ */
