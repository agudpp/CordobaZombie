/*
 * ExplosionFake.h
 *
 *  Created on: Apr 3, 2012
 *      Author: Emi
 */

#ifndef BLOODFAKE_H_
#define BLOODFAKE_H_

#include "Effects.h"
#include <string>
#include <cstdlib>

#include <EffectQueues.h>

class BloodFake: public Effects {
private:
	static int numberOfInstance;

	std::string name;

	static std::vector<Blood *> *availableEffects;



public:
	BloodFake();
	virtual ~BloodFake();


	void start();
	void end();
	bool hasEnded();
	void update();

};

#endif /* BLOODFAKE_H_ */
