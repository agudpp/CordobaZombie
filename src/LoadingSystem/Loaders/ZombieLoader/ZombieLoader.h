/*
 * ZombieLoader.h
 *
 *  Created on: 15/05/2012
 *      Author: agustin
 */

#ifndef ZOMBIELOADER_H_
#define ZOMBIELOADER_H_

#include <string>
#include <list>
#include <vector>

#include "Loader.h"
#include "LoaderData.h"
#include "ZombieFactory.h"
#include "ZombieBuilder.h"


namespace Ogre {
class Entity;
}

class ZombieFactoryManager;
class TiXmlElement;

class ZombieLoader : public Loader
{
public:
	ZombieLoader();
	virtual ~ZombieLoader();

	// Functoin used to load something
	virtual int load(TiXmlElement*, LoaderData *);

	// Unload the information?
	virtual int unload();

private:
	bool parseFactories(TiXmlElement *elem, ZombieFactoryManager &zfm);
	bool parseZombies(TiXmlElement *elem, ZombieUnitVec &zombies);

private:
	ZombieBuilder			mBuilder;
	ZombieFactoryManager 	*mZombieFactoryMngr;
	ZombieUnitVec			*mZombies;
	ZombieQueue				mQueue;
};

#endif /* ZOMBIELOADER_H_ */
