/*
 * BulletImporter.cpp
 *
 *  Created on: Sep 25, 2013
 *      Author: agustin
 */

#include "BulletImporter.h"

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <debug/PrimitiveDrawer.h>

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

} /* namespace physics */
