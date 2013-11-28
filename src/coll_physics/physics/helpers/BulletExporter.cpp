/*
 * BulletExporter.cpp
 *
 *  Created on: Nov 15, 2013
 *      Author: agustin
 */

#include "BulletExporter.h"

#include <string.h>
#include <fstream>

#include <OgreVector3.h>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>

#include <debug/DebugUtil.h>
#include <ogre_utils/OgreUtil.h>


// Some internal defines
#define BE_HEADER       "BEXPORTER0.1"
#define BE_MAX_VERTS    10000



namespace physics {

namespace BulletExporter {


////////////////////////////////////////////////////////////////////////////////
bool
transform(const Ogre::Mesh* mesh, const std::string& outFile)
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

////////////////////////////////////////////////////////////////////////////////
BulletShape*
buildShapeFromFile(const std::string& filePath)
{
    // read the file
    std::ifstream file(filePath.c_str(), std::ifstream::binary);
    if (!file) {
        debugERROR("Error openning the file %s to be read\n", filePath.c_str());
        return 0;
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
        return 0;
    }

    // read the number of vertices and the vertices itself
    file.read(reinterpret_cast<char*>(&vCount), sizeof(core::size_t));
    if (vCount == 0) {
        debugERROR("No vertices were read\n");
        return 0;
    }
    file.read(reinterpret_cast<char*>(vertices), sizeof(Ogre::Vector3) * vCount);

    // now read the indices
    file.read(reinterpret_cast<char*>(&iCount), sizeof(core::size_t));
    if (iCount == 0) {
        debugERROR("No indices readed... something is wrong with %s\n", filePath.c_str());
        return 0;
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

    return simplifiedConvexShape;
}

}

}
