/*
 * BulletImporter.cpp
 *
 *  Created on: Sep 25, 2013
 *      Author: agustin
 */

#include "BulletImporter.h"

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>

#include <debug/PrimitiveDrawer.h>
#include <ogre_utils/OgreUtil.h>
#include <types/basics.h>

#include "BulletUtils.h"



namespace physics {

BulletImporter::BulletImporter()
{
    // TODO Auto-generated constructor stub

}

BulletImporter::~BulletImporter()
{
    // TODO Auto-generated destructor stub
}

////////////////////////////////////////////////////////////////////////////////
BulletObject*
BulletImporter::createBox(const Ogre::AxisAlignedBox& bb, float mass)
{
    BulletObject* result = new BulletObject;

    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
    core::Primitive* prim = pd.createBox(bb.getCenter(), bb.getSize(), pd.getFreshColour());
    result->motionState.setNode(prim->node);

    btVector3 localInertia(0,0,0);
    btTransform startTransform;
    startTransform.setIdentity();

    const Ogre::Vector3 halfsize = bb.getHalfSize();
    const Ogre::Vector3 center = bb.getCenter();

    btCollisionShape* shape = new btBoxShape(btVector3(halfsize.x, halfsize.y, halfsize.z));
    if (mass != 0.f) {
        shape->calculateLocalInertia(mass,localInertia);
    }
    startTransform.setOrigin(btVector3(center.x,center.y,center.z));
    result->motionState.setWorldTransform(startTransform);
    btRigidBody::btRigidBodyConstructionInfo handrbInfo(mass,
                                                        &(result->motionState),
                                                        shape,
                                                        localInertia);
    btRigidBody* handbody = new btRigidBody(handrbInfo);
    result->rigidBody = handbody;
    result->shape = shape;

    handbody->setUserPointer(result);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
btCollisionShape*
BulletImporter::createShapeFromMesh(const Ogre::Mesh* mesh)
{
    ASSERT(mesh);

    // get the mesh information
    Ogre::Vector3 vertices[10000];
    core::size_t vCount = 10000;
    unsigned long indices[10000];
    core::size_t iCount = 10000;

    if (!core::OgreUtil::getMeshInformation(mesh,
                                            vCount,
                                            vertices,
                                            iCount,
                                            indices)) {
        debugERROR("Error getting mesh information\n");
        return 0;
    }

    // now remove the duplicated vertices
    core::OgreUtil::removeDuplicated(vertices, vCount, indices, iCount);

    // now we will proceed to construct the btConvexHullShape
    btConvexHullShape originalShape((btScalar*) vertices, vCount, sizeof(Ogre::Vector3));
    btShapeHull hull(&originalShape);
    float margin = originalShape.getMargin();
    hull.buildHull(margin);
    btConvexHullShape* simplifiedConvexShape =
        new btConvexHullShape((btScalar*)hull.getVertexPointer(), hull.numVertices());

    return simplifiedConvexShape;
}

////////////////////////////////////////////////////////////////////////////////
BulletObject*
BulletImporter::createFromEntity(const Ogre::SceneNode* node,
                                 const Ogre::Entity* entity,
                                 float mass)
{
    ASSERT(node);
    ASSERT(entity);

    // get the mesh and extract the vertices information
    if (entity->getNumSubEntities() > 1) {
        debugERROR("WE ARE NOT SUPPORTING THIS RIGHT NOW! ent:%s with %d\n",
            entity->getName().c_str(), entity->getNumSubEntities());
        return 0;
    }

    btCollisionShape* simplifiedConvexShape = createShapeFromMesh(entity->getMesh().get());

    // scale the rigid body as the scene node
    simplifiedConvexShape->setLocalScaling(BulletUtils::ogreToBullet(node->getScale()));


    // construct the object and potitionate where we need.
    BulletObject* result = new BulletObject;
    btVector3 localInertia(0,0,0);
    btTransform startTransform;
    startTransform.setIdentity();
    if (mass != 0.f) {
        simplifiedConvexShape->calculateLocalInertia(mass,localInertia);
    }
    startTransform.setOrigin(BulletUtils::ogreToBullet(node->getPosition()));
    startTransform.setRotation(BulletUtils::ogreToBullet(node->getOrientation()));

    result->motionState.setWorldTransform(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,
                                                    &(result->motionState),
                                                    simplifiedConvexShape,
                                                    localInertia);
    btRigidBody* rigidBody = new btRigidBody(rbInfo);
    result->rigidBody = rigidBody;
    result->shape = simplifiedConvexShape;
    rigidBody->setUserPointer(result);

    return result;
}

} /* namespace physics */
