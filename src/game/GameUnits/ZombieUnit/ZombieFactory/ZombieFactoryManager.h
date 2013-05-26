/*
 * ZombieFactoryManager.h
 *
 *  Created on: 20/06/2012
 *      Author: agustin
 */

#ifndef ZOMBIEFACTORYMANAGER_H_
#define ZOMBIEFACTORYMANAGER_H_

#include <map>

#include "ZombieFactory.h"


class ZombieFactoryManager
{
public:
	ZombieFactoryManager();
	~ZombieFactoryManager();

	/**
	 * Add a new factory
	 * @note	The memory allocated is now a problem of this class, so you must
	 * 			allocate the factories using new. This class will delete it later
	 */
	inline void addNewFactory(ZombieFactory *zf);

	/**
	 * Removes and frees the memories
	 */
	inline void removeFactory(ZombieFactory *zf);
	inline void removeFactory(const Ogre::String &name);

	/**
	 * Get a zombie factory
	 */
	inline ZombieFactory *getFactory(const Ogre::String &name);

	/**
	 * Free and remove all the memory of the factories
	 */
	void freeAndRemoveAll(void);

private:
	typedef std::map<Ogre::String, ZombieFactory *>		FactoryCont;


	FactoryCont			mFactories;
};




/**
 * Add a new factory
 * @note	The memory allocated is now a problem of this class, so you must
 * 			allocate the factories using new. This class will delete it later
 */
inline void ZombieFactoryManager::addNewFactory(ZombieFactory *zf)
{
	ASSERT(zf);
	ASSERT(!getFactory(zf->getName()));
	mFactories[zf->getName()] = zf;
}

/**
 * Removes and frees the memories
 */
inline void ZombieFactoryManager::removeFactory(ZombieFactory *zf)
{
	ASSERT(zf);
	ASSERT(getFactory(zf->getName()) == zf);
	FactoryCont::iterator it = mFactories.find(zf->getName());
	delete it->second;
	mFactories.erase(it);
}
inline void ZombieFactoryManager::removeFactory(const Ogre::String &name)
{
	removeFactory(getFactory(name));
}

/**
 * Get a zombie factory
 */
inline ZombieFactory *ZombieFactoryManager::getFactory(const Ogre::String &name)
{
	FactoryCont::iterator it = mFactories.find(name);
	if(it == mFactories.end()) return 0;
	return it->second;
}

#endif /* ZOMBIEFACTORYMANAGER_H_ */
