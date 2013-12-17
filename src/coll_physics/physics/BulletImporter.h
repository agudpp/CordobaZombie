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
#include "BulletShape.h"

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
    // @param bb                The bounding box
    // @param mass              The mass of the bb
    // @param createPrimitive   Flat to create a primitive (box) or not.
    // @return BulletObject | 0 on error
    // This method will set the rigidBody->userDefinedPointer(this).
    //
    static BulletObject*
    createBox(const Ogre::AxisAlignedBox& bb,
              float mass,
              bool createPrimitive = true);
    static bool
    createBox(BulletObject& bo,
              const Ogre::AxisAlignedBox& bb,
              float mass,
              bool createPrimitive = true);

    // @brief Create a bullet shape of the type box from a given Ogre BB.
    // @param bb        The Ogre bounding box.
    // @return the new allocated shape | 0 on error.
    //
    static BulletShape*
    createBoxShape(const Ogre::AxisAlignedBox& bb);

    // @brief Create a plane shape from a vector (direction) and a float distance
    // @param direction     The direction of the plane
    // @param distance      The distance where the plane should be set
    // @return the new allocated shape or 0 on error.
    //
    static BulletShape*
    createPlaneShape(const Ogre::Vector3& direction, float distance);

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
