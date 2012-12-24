/*
 * CollisionObjectsLoader.cpp
 *
 *  Created on: 02/12/2012
 *      Author: agustin
 */

#include "CollisionObjectsLoader.h"

#include "CollisionManager.h"

CollisionObjectsLoader::CollisionObjectsLoader() :
    mCollisionMngr(0)
{

}

CollisionObjectsLoader::~CollisionObjectsLoader()
{
    unload();
}


// Functoin used to load something
int
CollisionObjectsLoader::load(TiXmlElement *elem, LoaderData *data)
{
    ASSERT(elem);
    ASSERT(data);

    // first unload everything
    unload();

    mCollisionMngr = data->collisionManager;
    ASSERT(mCollisionMngr);
    mCollisionObjects.clear();

    debugERROR("santy aca deberias meter tu codigo de cargado de "
            "collisionObjects\n");
    return 0;
}

// Unload the information?
int
CollisionObjectsLoader::unload()
{
    ASSERT(mCollisionMngr);
    // iterate over all the collision objects and remove it from the collision
    // manager and free it
    for(size_t i = 0, size = mCollisionObjects.size(); i < size; ++i){
        CollisionObject *obj = mCollisionObjects[i];
        mCollisionMngr->removeStaticObject(obj);
        delete obj;
    }
    mCollisionObjects.clear();

    return 0;
}
