/*
 * ZombieBuilder.h
 *
 *  Created on: 11/06/2012
 *      Author: agustin
 */

#ifndef ZOMBIEBUILDER_H_
#define ZOMBIEBUILDER_H_

#include <OgreString.h>

#include "XMLHelper.h"
#include "ZombieUnit.h"
#include "tinyxml.h"
#include "BillboardBatery.h"
#include "CollisionManager.h"
#include "ZombieFactory.h"

class TiXmlElement;

class ZombieBuilder
{
public:
    ZombieBuilder();
    ~ZombieBuilder();

    /**
     * Set the filename to be used
     */
    void
    setFilename(const Ogre::String &fname);

    /**
     * Configure the Builder
     * @param	cm		The collision manager to use
     * @param	bbb		The billboardBateryManager for the blood
     * @param	q		The queue of zombies
     *
     * @note This function will create the transition table associated to the
     * normal zombies and all the other zombies.
     */
    void
    configure(c_p::CollisionManager *cm, effects::BillboardBatery *bbb,
        ZombieQueue *q);

    /**
     * Create a new zombie from a NAME
     */
    ZombieUnit *
    createZombie(const Ogre::String &id);

private:
    ZombieUnit *
    buildZombie(const TiXmlElement *elem);
    ZombieUnit *
    buildNormalZombie(const TiXmlElement *elem);

private:
    core::XMLHelper mHelper;
};

#endif /* ZOMBIEBUILDER_H_ */
