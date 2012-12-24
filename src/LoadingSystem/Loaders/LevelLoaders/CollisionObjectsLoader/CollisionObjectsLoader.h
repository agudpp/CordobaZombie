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

class TiXmlElement;
class CollisionManager;
class CollisionObject;

class CollisionObjectsLoader : public Loader {
public:
    CollisionObjectsLoader();
    virtual ~CollisionObjectsLoader();

    // Functoin used to load something
    virtual int load(TiXmlElement*, LoaderData *data);

    // Unload the information?
    virtual int unload();

private:
    typedef std::vector<CollisionObject *> ObjectsVec;

    ObjectsVec mCollisionObjects;
    CollisionManager *mCollisionMngr;
};

#endif /* COLLISIONOBJECTSLOADER_H_ */
