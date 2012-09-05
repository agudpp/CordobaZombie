/*
 * ZombieLoader.cpp
 *
 *  Created on: 15/05/2012
 *      Author: agustin
 */

#include <OgreStringConverter.h>

#include "DebugUtil.h"
#include "ZombieLoader.h"
#include "ZombieFactoryManager.h"


bool ZombieLoader::parseFactories(TiXmlElement *elem, ZombieFactoryManager &zfm)
{
	ASSERT(elem);
	ASSERT(Ogre::String(elem->Value()) == "ZombieFactories");

	// for every factory..
	TiXmlElement *pElement = elem->FirstChildElement("ZombieFactory");
	while(pElement){
		ZombieFactory *zombiefact = 0;

		Ogre::String name;
		sm::AABB aabb;
		sm::Vector2 dir;
		int N;

		name = pElement->Attribute("name");
		parseAABB(pElement->FirstChildElement("aabb"), aabb);
		parseVector(pElement->FirstChildElement("direction"), dir);
		TiXmlElement *number = pElement->FirstChildElement();
		ASSERT(number);
		N = Ogre::StringConverter::parseInt(number->Attribute("N"));

		// configure the factory
		zombiefact = new ZombieFactory;
		zombiefact->configure(aabb, Ogre::Vector3(dir.x, 0, dir.y), N, &mQueue);
		zfm.addNewFactory(zombiefact);

		debugGREEN("Factory parsed and created.\n");

		pElement = pElement->NextSiblingElement("ZombieFactory");
	}



	return true;
}

bool ZombieLoader::parseZombies(TiXmlElement *elem, ZombieUnitVec &zombies)
{
	ASSERT(elem);

	if(!zombies.empty()){
		debugWARNING("We already have zombies? %lu\n", zombies.size());
	}

	// get the zombieBuilderFile
	Ogre::String zombieBuilderFile = elem->Attribute("zombieBuilderFile");
	ASSERT(!zombieBuilderFile.empty());
	mBuilder.setFilename(zombieBuilderFile);

	// the builder must already be configured... now start to parse and build
	// all the zombies
	TiXmlElement *pElem = elem->FirstChildElement("Zombie");
	while(pElem){
		// parse the count
		int count = 1;
		if(pElem->Attribute("count")){
			count = Ogre::StringConverter::parseInt(pElem->Attribute("count"));
		}
		Ogre::String nameId = pElem->Attribute("nameID");
		for(int i = 0; i < count; ++i){
			ZombieUnit *zu = mBuilder.createZombie(nameId);
			// put it in the queue and the vector
			if(zu){
				mQueue.addNewObject(zu);
				zombies.push_back(zu);
				debugGREEN("New Zombie [%s] created.\n", nameId.c_str());
			} else {
				debugERROR("Error creando zombie con nameID: %s\n",
					nameId.c_str());
			ASSERT(false);
				return false;
			}
		}

		pElem = pElem->NextSiblingElement("Zombie");
	}

	return true;
}



ZombieLoader::ZombieLoader() :
Loader("ZombieLoader"),
mZombieFactoryMngr(0),
mZombies(0)
{

}

ZombieLoader::~ZombieLoader()
{

}

// Functoin used to load something
int ZombieLoader::load(TiXmlElement *elem, LoaderData *data)
{
	ASSERT(elem);
	ASSERT(data);

	// clear all the data
	unload();

	// get the values that we will use
	ASSERT(data->zombieFactoryManager);
	ASSERT(data->zombieUnits);

	mZombieFactoryMngr = data->zombieFactoryManager;
	mZombies = data->zombieUnits;

	// configure the builder
	ASSERT(data->collisionManager);
	ASSERT(data->bloodBatery);
	mBuilder.configure(data->collisionManager,data->bloodBatery, &mQueue);


	if(Ogre::String(elem->Value()) != "Zombies"){
		debugERROR("Wrong file: %s\n", elem->Value());
		return -1;
	}

	debugGREEN("Start to load zombies and Zombie factories\n");

	// first get the zombies
	TiXmlElement *auxElem = elem->FirstChildElement("Zombies");
	ASSERT(auxElem);
	if(!parseZombies(auxElem, *mZombies)){
		debugERROR("Error parsing the Zombies\n");
		return -3;
	}

	// get the first element to parse the factories
	auxElem = elem->FirstChildElement("ZombiesFactories");
	if(auxElem){
		if(!parseFactories(auxElem, *mZombieFactoryMngr)){
			debugERROR("Error parsing the factories\n");
			return -2;
		}
	}

	return 0;
}

// Unload the information?
int ZombieLoader::unload()
{
	ASSERT(mZombieFactoryMngr);
	ASSERT(mZombies);

	debugGREEN("Unloading zombies and factories\n");

	ASSERT(mZombies->size() == mQueue.getQueue().size());
	mZombieFactoryMngr->freeAndRemoveAll();
	mQueue.freeAll();
	mZombies->clear();

	ASSERT(false);
}


