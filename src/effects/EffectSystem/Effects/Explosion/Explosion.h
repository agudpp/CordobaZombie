/*
 * Explosion.h
 *
 *  Created on: Apr 3, 2012
 *      Author: Emi
 */

#ifndef EXPLOSION_H_
#define EXPLOSION_H_

#include <EffectSystem/Effect.h>
#include <string>
#include <cstdlib>

class Explosion: public Effect {
private:
	static int numberOfInstance;

	std::string name;

	//Particle system that handles the effect
	Ogre::ParticleSystem *effectPS;
public:
	Explosion();
	virtual ~Explosion();


	void start();
	void end();
	bool hasEnded();
	void update();

};

#endif /* EXPLOSION_H_ */
