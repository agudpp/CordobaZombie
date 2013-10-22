/*
 * BulletLoader.cpp
 *
 *  Created on: Oct 22, 2013
 *      Author: agustin
 */

#include "BulletLoader.h"

#include <OgreMesh.h>

#include <unordered_map>

#include <debug/DebugUtil.h>

#include "BulletUtils.h"
#include "BulletImporter.h"

namespace physics {


////////////////////////////////////////////////////////////////////////////////
BulletLoader::BulletLoader()
{
}
////////////////////////////////////////////////////////////////////////////////
BulletLoader::~BulletLoader()
{
    freeMemory();
}

////////////////////////////////////////////////////////////////////////////////
bool
BulletLoader::construct(const InfoVec& ogreElements, BulletObjectVec& bulletElements)
{
    bulletElements.clear();
    if (ogreElements.empty()) {
        debugWARNING("You are trying to construct bullet objects from an empty"
            " list of OgreElements!\n");
        return false;
    }

    // Here we will need to check for all the entities (mesh) that were already
    // loaded (its shapes) and reuse that for the new BulletObjects.
    // We will use a stl hash here, probably it will be a good idea to implement
    // a new one (StackHash).
    //
    debugOPTIMIZATION("Implement a hash here! (StackHash?).\n");

    debugERROR("Note that here we will be loading all the bullet objects (shapes)"
        " from a Ogre::Mesh, and that is slow and not necessary, we need to get the "
        "bullet representation file (i.e: <meshName>.bullet). For now we are "
        "just loading from the entity directly, this could be changed then in the"
        " BulletImporter instead or here.\n");

    mBulletObjects.reserveMem(ogreElements.size());

    std::unordered_map<Ogre::Mesh*, btCollisionShape*> shapeHash;

    for (const Info& i : ogreElements) {
        // 1) Create a new bulletObject.
        // 2) Check if the shape already exists and use it or create a new one
        // 3) Configure the rigid body to be in the same position / orientation
        //    than the sceneNode
        // 4) Hold the bullet data into the containers
        //

        // (1)
        BulletObject* bo = new BulletObject;
        bulletElements.push_back(bo);

        // (2)
        btCollisionShape* shape = 0;
        Ogre::Mesh* mesh = i.object->getMesh().get();
        ASSERT(mesh);

        auto it = shapeHash.find(mesh);
        const bool newShape = it == shapeHash.end();
        if (newShape) {
            // create a new one
            shape = BulletImporter::createShapeFromMesh(mesh);

            // add it to the hash
            shapeHash[mesh] = shape;
        } else {
            // use the already allocated one
            shape = it->second;
        }

        ASSERT(shape);

        // (3)
        // create the rigid body
        //
        bo->entity = i.object;
        bo->motionState.setNode(i.node);
        bo->shape = shape;

        // scale the rigid body as the scene node
        debugERROR("Here we are using probably in a wrong way the scaling, check"
            " issue #165\n");
        shape->setLocalScaling(BulletUtils::ogreToBullet(i.node->getScale()));

        // construct the object and put it where it should be
        btVector3 localInertia(0,0,0);
        btTransform startTransform;
        startTransform.setIdentity();
        if (i.mass != 0.f) {
            shape->calculateLocalInertia(i.mass,localInertia);
        }
        startTransform.setOrigin(BulletUtils::ogreToBullet(i.node->getPosition()));
        startTransform.setRotation(BulletUtils::ogreToBullet(i.node->getOrientation()));
        bo->motionState.setWorldTransform(startTransform);

        btRigidBody::btRigidBodyConstructionInfo rbInfo(i.mass,
                                                        &(bo->motionState),
                                                        shape,
                                                        localInertia);
        btRigidBody* rigidBody = new btRigidBody(rbInfo);
        bo->rigidBody = rigidBody;
        rigidBody->setUserPointer(bo);

        // (4)
        mBulletObjects.add(bo);
        if (newShape) {
            mShapes.add(shape);
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
BulletLoader::freeMemory(void)
{
    mShapes.freeAll();
    mBulletObjects.freeAll();
}

} /* namespace physics */
