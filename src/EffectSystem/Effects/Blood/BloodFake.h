/*
 * ExplosionFake.h
 *
 *  Created on: Apr 3, 2012
 *      Author: Emi
 */

#ifndef BLOODFAKE_H_
#define BLOODFAKE_H_

#include <EffectSystem/Effect.h>
#include <string>
#include <cstdlib>

class BloodFake: public Effect {
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
