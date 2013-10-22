/*
 * BulletImporter.h
 *
 *  Created on: Sep 25, 2013
 *      Author: agustin
 */

#ifndef BULLETIMPORTER_H_
#define BULLETIMPORTER_H_

#include <OgreAxisAlignedBox.h>
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreMesh.h>

#include <bullet/btBulletCollisionCommon.h>

#include "BulletObject.h"

namespace physics {

// @brief This class will be used to create the btRigidBodies from the .mesh
//        Ogre files.
//

class BulletImporter
{
public:
    BulletImporter();
    ~BulletImporter();

    // @brief Helper method to construct a simple box.
    // @param bb        The bounding box
    // @param mass      The mass of the bb
    // @return BulletObject | 0 on error
    // This method will set the rigidBody->userDefinedPointer(this).
    //
    static BulletObject*
    createBox(const Ogre::AxisAlignedBox& bb, float mass);

    // @brief This method will create a shape from a given mesh.
    // @param mesh      The Ogre mesh that we want to use to create the shape
    // @returns btCollisionShape on success | 0 on error
    //
    static btCollisionShape*
    createShapeFromMesh(const Ogre::Mesh* mesh);

    // @brief This method will construct a BulletObject from an entity structure.
    //        We will need also the SceneNode associated to the entity to get
    //        the transformation position and rotation.
    //        This method is for debug only, for now.
    // @param node      The scene node of the entity
    // @param entity    The entity we want to create the bullet object
    // @param mass      The mass we want to use for the object (probably 0).
    // @return the allocated BulletObject | 0 on error.
    //
    static BulletObject*
    createFromEntity(const Ogre::SceneNode* node,
                     const Ogre::Entity* entity,
                     float mass = 0.f);

};

} /* namespace physics */
#endif /* BULLETIMPORTER_H_ */
