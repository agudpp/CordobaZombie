/*
 * Blood.h
 *
 *  Created on: Apr 1, 2012
 *      Author: Emi
 */

#ifndef BLOOD_H_
#define BLOOD_H_

#include "Effects.h"
#include <string>
#include <cstdlib>

class Blood: public Effects {
private:
	static int numberOfInstance;

	//Particle system that handles the effect
	Ogre::ParticleSystem *effectPS;

	std::string name;
public:
	Blood();
	virtual ~Blood();

	void start();
	void end();
	bool hasEnded();
	void update();
};

#endif /* BLOOD_H_ */
