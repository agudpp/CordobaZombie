/*
 * PhysicsBuilder.cpp
 *
 *  Created on: Jan 8, 2014
 *      Author: agustin
 */

#include "PhysicsBuilder.h"


#include <string.h>
#include <fstream>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>
#include <bullet/BulletCollision/CollisionShapes/btStaticPlaneShape.h>

#include <debug/DebugUtil.h>
#include <ogre_utils/OgreUtil.h>
#include <types/basics.h>

#include "BulletUtils.h"




// Some internal defines
#define BE_HEADER       "BEXPORTER0.1"
#define BE_MAX_VERTS    10000


namespace physics {

////////////////////////////////////////////////////////////////////////////
//                          Shape construction
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
bool
PhysicsBuilder::createBoxShape(Shape& shape, const Ogre::AxisAlignedBox& bb)
{
    // check if we have another shape already set here
#ifdef DEBUG
    if (shape.bulletShape() != 0) {
        debugWARNING("We already have a shape set. Be careful with a memory leak\n");
    }
#endif
    const Ogre::Vector3 halfsize = bb.getHalfSize();
    shape.setShape(new btBoxShape(btVector3(halfsize.x, halfsize.y, halfsize.z)));

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
PhysicsBuilder::createPlaneShape(Shape& shape,
                                 const Ogre::Vector3& direction,
                                 float distance)
{
    // check if we have another shape already set here
#ifdef DEBUG
    if (shape.bulletShape() != 0) {
        debugWARNING("We already have a shape set. Be careful with a memory leak\n");
    }
#endif

    shape.setShape(new btStaticPlaneShape(BulletUtils::ogreToBullet(direction), distance));
    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
PhysicsBuilder::createShapeFromMesh(Shape& shape, const Ogre::Mesh* mesh)
{
    ASSERT(mesh);

#ifdef DEBUG
    if (shape.bulletShape() != 0) {
        debugWARNING("We already have a shape set. Be careful with a memory leak\n");
    }
#endif

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
        return false;
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

    // set the shape
    shape.setShape(simplifiedConvexShape);

    return true;
}

////////////////////////////////////////////////////////////////////////////
void
PhysicsBuilder::destroyShape(Shape& shape)
{
    delete shape.bulletShape();
    shape.setShape(0);
}


////////////////////////////////////////////////////////////////////////////
//                     CollisionObject construction
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
bool
PhysicsBuilder::createCollObjectFromShape(CollisionObject& co, Shape& shape)
{
    if (shape.bulletShape() == 0) {
        debugERROR("We cannot create a collision object from a null shape\n");
        return false;
    }

    // check if we have a collision object already created
#ifdef DEBUG
    if (co.bulletCollObj() != 0) {
        debugWARNING("We already have a collision object set. Be careful with a "
            "memory leak\n");
    }
#endif

    // create the collision object and set the shape
    btCollisionObject* result = new btCollisionObject;
    result->setCollisionShape(shape.bulletShape());
    co.setBullettCollObject(result);
    co.setShape(shape);

    return true;
}

////////////////////////////////////////////////////////////////////////////
void
PhysicsBuilder::destroyCollObjectAndShape(CollisionObject& co)
{
    destroyShape(co.shape());
    delete co.bulletCollObj();
    co.setBullettCollObject(0);
}

////////////////////////////////////////////////////////////////////////////
//                     RigidBody construction
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
bool
PhysicsBuilder::createRigidBodyFromShape(RigidBody& rb,
                                         Shape& shape,
                                         float mass,
                                         MotionState* motionState)
{
    // check if we have a shape
    if (shape.bulletShape() == 0) {
        debugERROR("We cannot construct a RigidBody without a shape!\n");
        return false;
    }

    // check if we have a rigid body already created
#ifdef DEBUG
    if (rb.bulletRigidBody() != 0) {
        debugWARNING("We already have a rigid body set. Be careful with a "
            "memory leak\n");
    }
#endif

    btVector3 localInertia(0,0,0);
    btTransform startTransform;
    startTransform.setIdentity();

    btCollisionShape* btshape = shape.bulletShape();

    if (mass != 0.f) {
        btshape->calculateLocalInertia(mass, localInertia);
    }

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,
                                                    motionState,
                                                    btshape,
                                                    localInertia);
    btRigidBody* rigidBody = new btRigidBody(rbInfo);

    // set the rigid body to the result
    rb.setBulletRigidBody(rigidBody);
    rb.setShape(shape);

    return true;
}


////////////////////////////////////////////////////////////////////////////
//                     Import / Export methods
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
bool
PhysicsBuilder::exportMesh(const Ogre::Mesh* mesh, const std::string& outFile)
{
    ASSERT(mesh);
    // get the mesh information
    Ogre::Vector3 vertices[BE_MAX_VERTS];
    core::size_t vCount = BE_MAX_VERTS;
    unsigned long indices[BE_MAX_VERTS];
    core::size_t iCount = BE_MAX_VERTS;

    if (!core::OgreUtil::getMeshInformation(mesh,
                                            vCount,
                                            vertices,
                                            iCount,
                                            indices)) {
        debugERROR("Error getting mesh %s information\n", mesh->getName().c_str());
        return false;
    }

    // now remove the duplicated vertices
    core::OgreUtil::removeDuplicated(vertices, vCount, indices, iCount);

    // now we have to save all the information into a file
    // open the file to be writed
    std::ofstream file(outFile.c_str(), std::ofstream::binary);
    if (!file) {
        debugERROR("Error openning the file %s\n", outFile.c_str());
        return false;
    }

    // write the header into the file
    unsigned int headerSize = strlen(BE_HEADER);
    file.write(BE_HEADER, headerSize);

    // write the number of vertices and the vertices first
    file.write(reinterpret_cast<const char*>(&vCount), sizeof(core::size_t));
    file.write(reinterpret_cast<const char*>(vertices),
               vCount * sizeof(Ogre::Vector3));

    // write the indices
    file.write(reinterpret_cast<const char*>(&iCount), sizeof(core::size_t));
    file.write(reinterpret_cast<const char*>(indices),
               iCount * sizeof(unsigned long));

    file.close();

    return file.good();
}

////////////////////////////////////////////////////////////////////////////
bool
PhysicsBuilder::buildShapeFromFile(Shape& shape, const std::string& filePath)
{
    // read the file
    std::ifstream file(filePath.c_str(), std::ifstream::binary);
    if (!file) {
        debugERROR("Error openning the file %s to be read\n", filePath.c_str());
        return false;
    }

    Ogre::Vector3 vertices[BE_MAX_VERTS];
    core::size_t vCount = 0;
    unsigned long indices[BE_MAX_VERTS];
    core::size_t iCount = 0;
    unsigned int headerSize = strlen(BE_HEADER);
    char buff[128];

    // check header
    file.read(buff, headerSize);
    buff[headerSize] = '\0';
    if (strcmp(buff, BE_HEADER) != 0) {
        debugERROR("Error reading the header file, wrong file format? Header %s "
                "and we expected %s\n", buff, BE_HEADER);
        return false;
    }

    // read the number of vertices and the vertices itself
    file.read(reinterpret_cast<char*>(&vCount), sizeof(core::size_t));
    if (vCount == 0) {
        debugERROR("No vertices were read\n");
        return false;
    }
    file.read(reinterpret_cast<char*>(vertices), sizeof(Ogre::Vector3) * vCount);

    // now read the indices
    file.read(reinterpret_cast<char*>(&iCount), sizeof(core::size_t));
    if (iCount == 0) {
        debugERROR("No indices readed... something is wrong with %s\n", filePath.c_str());
        return false;
    }
    file.read(reinterpret_cast<char*>(indices),
               iCount * sizeof(unsigned long));

    file.close();

    // now construct the using these information
    // now we will proceed to construct the btConvexHullShape
    btConvexHullShape originalShape((btScalar*) vertices,
                                    vCount,
                                    sizeof(Ogre::Vector3));
    btShapeHull hull(&originalShape);
    float margin = originalShape.getMargin();
    hull.buildHull(margin);
    btConvexHullShape* simplifiedConvexShape =
        new btConvexHullShape((btScalar*)hull.getVertexPointer(), hull.numVertices());

    // check if we have another shape already set here
#ifdef DEBUG
    if (shape.bulletShape() != 0) {
        debugWARNING("We already have a shape set. Be careful with a memory leak\n");
    }
#endif

    shape.setShape(simplifiedConvexShape);
    return true;
}


} /* namespace physics */
