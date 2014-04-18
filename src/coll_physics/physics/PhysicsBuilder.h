/*
 * PhysicsBuilder.h
 *
 *  Created on: Jan 8, 2014
 *      Author: agustin
 */

#ifndef PHYSICSBUILDER_H_
#define PHYSICSBUILDER_H_

#include <string>

#include <OgreAxisAlignedBox.h>
#include <OgreMesh.h>

#include "Shape.h"
#include "CollisionObject.h"
#include "RigidBody.h"
#include "MotionState.h"



namespace physics {

// @brief This module will be the one used to build and construct all the
//        physics elements. Do not try to build it by yourself.
//

class PhysicsBuilder
{
public:

    ////////////////////////////////////////////////////////////////////////////
    //                          Shape construction
    ////////////////////////////////////////////////////////////////////////////

    // @brief Helper method to construct a simple box.
    // @param shape             The shape to be constructed.
    // @param bb                The bounding box to be built.
    // @return true on success | false otherwise
    // @note We will allocate memory for the shape here. The caller is the
    //       responsible for freeing later the memory.
    //
    static bool
    createBoxShape(Shape& shape, const Ogre::AxisAlignedBox& bb);

    // @brief Create a plane shape from a vector (direction) and a float distance
    // @param shape         The shape to be constructed.
    // @param direction     The direction of the plane
    // @param distance      The distance where the plane should be set
    // @return true on success | false otherwise
    // @note We will allocate memory for the shape here. The caller is the
    //       responsible for freeing later the memory.
    //
    static bool
    createPlaneShape(Shape& shape, const Ogre::Vector3& direction, float distance);

    // @brief This method will create a shape from a given mesh. This is not the
    //        optimum way to create a collision shape since it will be an
    //        approximation and not necessarily the best one.
    // @param shape     The shape to be built.
    // @param mesh      The Ogre mesh that we want to use to create the shape
    // @returns true on success | false otherwise
    // @note We will allocate memory for the shape here. The caller is the
    //       responsible for freeing later the memory.
    //
    static bool
    createShapeFromMesh(Shape& shape, const Ogre::Mesh* mesh);

    // @brief Destroy a shape (deallocating its bullet memory).
    //        Note that we will just free its memory, we will not check if the
    //        shape is being used, or anything.
    // @param shape     The shape to destroy
    //
    static void
    destroyShape(Shape& shape);


    ////////////////////////////////////////////////////////////////////////////
    //                     CollisionObject construction
    ////////////////////////////////////////////////////////////////////////////

    // @brief Create a collision object from a given already created shape.
    // @param co        The collision object to be constructed.
    // @param shape     The shape to be used for the collision object. This
    //                  shape will be set to the collision object.
    // @return true on success | false otherwise
    // @note We will allocate memory for collision object here, so the caller
    //       is the responsible for freeing the memory later.
    //
    static bool
    createCollObjectFromShape(CollisionObject& co, Shape& shape);

    // @brief Destroy a collision object and its associated shape, without checking
    //        if the shape nor the collision object are being shared or used
    //        anywhere else.
    // @param co        The collision object to destroy
    //
    static void
    destroyCollObjectAndShape(CollisionObject& co);

    ////////////////////////////////////////////////////////////////////////////
    //                     RigidBody construction
    ////////////////////////////////////////////////////////////////////////////

    // @brief Create a rigid body from a given shape and a mass to be used.
    // @param rb            The rigid body to be constructed.
    // @param shape         The shape to be used. This shape will be set to the
    //                      rigid body.
    // @param mass          The mass of the rigid body.
    // @param motionState   The motion state to be used. Note that this instance
    //                      shouldn't be free until the rigid body is freed.
    // @return true on success | false otherwise
    // @note We will allocate memory for the rigid body, so the caller is the one
    //       who should free the memory later.
    //
    static bool
    createRigidBodyFromShape(RigidBody& rb,
                             Shape& shape,
                             float mass,
                             MotionState* motionState);


    ////////////////////////////////////////////////////////////////////////////
    //                     Import / Export methods
    ////////////////////////////////////////////////////////////////////////////

    // @brief Export an ogre mesh into a .3DColl object (or any other extension)
    //        to be able to be imported later. This will create a 3D bullet shape
    //        representation.
    //        NOTE that this is only useful for bullet objects that have no mass nor
    //        anything else. If we want to save all that information we will need
    //        something else.
    // @param mesh      The mesh to be exported
    // @param outFile   The output filename (.3DColl for example).
    // @return true on success | false otherwise
    //
    static bool
    exportMesh(const Ogre::Mesh* mesh, const std::string& outFile);

    // @brief Create a bullet shape representation from a file (.3DColl).
    // @param shape     The shape to be built
    // @param file      The file name where the info is.
    // @return true on success | false otherwise
    // @note that the caller of this method is the owner of the memory now.
    //
    static bool
    buildShapeFromFile(Shape& shape, const std::string& file);

};

} /* namespace physics */
#endif /* PHYSICSBUILDER_H_ */
