/*
 * ZombieFactoryManager.cpp
 *
 *  Created on: 20/06/2012
 *      Author: agustin
 */

#include "DebugUtil.h"

#include "ZombieFactoryManager.h"

ZombieFactoryManager::ZombieFactoryManager()
{

}

ZombieFactoryManager::~ZombieFactoryManager()
{
	freeAndRemoveAll();
}


/**
 * Free and remove all the memory of the factories
 */
void ZombieFactoryManager::freeAndRemoveAll(void)
{
	FactoryCont::iterator it = mFactories.begin(), endIt = mFactories.end();
	for(; it != endIt; ++it){
		delete it->second;
	}
	mFactories.clear();
}
