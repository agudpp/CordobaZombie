/*
 * CollisionObjectsLoader.h
 *
 *  Created on: 02/12/2012
 *      Author: agustin
 */

#ifndef COLLISIONOBJECTSLOADER_H_
#define COLLISIONOBJECTSLOADER_H_

#include <vector>

#include "Loader.h"
#include "LoaderData.h"
#include "tinyxml.h"

// Forward
//
class TiXmlElement;
namespace c_p {
class CollisionManager;
class CollisionObject;
}

class CollisionObjectsLoader : public Loader
{
public:
    CollisionObjectsLoader();
    virtual
    ~CollisionObjectsLoader();

    // Functoin used to load something
    virtual int
    load(TiXmlElement*, LoaderData *data,
        LoaderManager::LoaderCallback *callback);

    // Unload the information?
    virtual int
    unload();

private:
    typedef std::vector<c_p::CollisionObject *> ObjectsVec;

    ObjectsVec mCollisionObjects;
    c_p::CollisionManager *mCollisionMngr;
};

#endif /* COLLISIONOBJECTSLOADER_H_ */
