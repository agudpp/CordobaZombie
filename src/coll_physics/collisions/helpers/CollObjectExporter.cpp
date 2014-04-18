/*
 * CollObjectExporter.cpp
 *
 *  Created on: Nov 14, 2013
 *      Author: agustin
 */

#include "CollObjectExporter.h"

#include <fstream>
#include <string.h>

#include <debug/DebugUtil.h>
#include <ogre_utils/OgreUtil.h>
#include <math/Vec2.h>
#include <types/StackVector.h>
#include <collisions/CollPreciseInfoBuilder.h>

#define COE_HEADER      "COLLEXPORTER_0.1"
#define COE_MAX_VERT    2048


namespace coll {

namespace CollObjectExporter {


////////////////////////////////////////////////////////////////////////////////
bool
transform(const Ogre::Mesh* mesh, const std::string& outFile)
{
    ASSERT(mesh);

    // read the vertices of the mesh
    Ogre::Vector3 verts[COE_MAX_VERT];
    core::size_t vCount = COE_MAX_VERT;
    if (!core::OgreUtil::getContourVertices(mesh,
                                            vCount,
                                            verts)){
        debugERROR("Error getting contour vertices for mesh %s \n",
            mesh->getName().c_str());
        // something happens
        return false;
    }

    // transform to the vertices we will use
    core::StackVector<core::Vector2, COE_MAX_VERT> points;
    for (unsigned int i = 0; i < vCount; ++i) {
        Ogre::Vector3& v = verts[i];
        points.push_back(core::Vector2(v.x, v.y));
    }

    // open the file to be writed
    std::ofstream file(outFile.c_str(), std::ofstream::binary);
    if (!file) {
        debugERROR("Error openning the file %s\n", outFile.c_str());
        return false;
    }

    // write the header into the file
    unsigned int headerSize = strlen(COE_HEADER);
    file.write(COE_HEADER, headerSize);

    // write the vertices into the file
    file.write(reinterpret_cast<const char*>(&vCount), sizeof(core::size_t));
    file.write(reinterpret_cast<const char*>(points.begin()),
               points.size() * sizeof(core::Vector2));
    file.close();

    return file.good();
}

////////////////////////////////////////////////////////////////////////////////
bool
buildFromFile(const std::string& collPath,
              CollPreciseInfo*& result,
              core::AABB& aabb)
{
    // we will do same than above:
    // - Read the header
    // - Read the number of vertices
    // - Read the vertices
    // - construct the CollPreciseInfo from that
    //
    std::ifstream file(collPath.c_str(), std::ifstream::binary);
    if (!file) {
        debugERROR("Error openning the file %s to be read\n", collPath.c_str());
        return false;
    }
    char buff[128];
    unsigned int headerSize = strlen(COE_HEADER);
    core::size_t vCount = 0;
    core::Vector2 points[COE_MAX_VERT];

    // check header
    file.read(buff, headerSize);
    buff[headerSize] = '\0';
    if (strcmp(buff, COE_HEADER) != 0) {
        debugERROR("Error reading the header file, wrong file format? Header %s "
                "and we expected %s\n", buff, COE_HEADER);
        return false;
    }

    // read the number of vertices and the vertices itself
    file.read(reinterpret_cast<char*>(&vCount), sizeof(core::size_t));
    if (vCount == 0) {
        debugERROR("No vertices were read\n");
        return false;
    }

    file.read(reinterpret_cast<char*>(&points), sizeof(core::Vector2) * vCount);

    // now construct the coll Precise info structure
    CollPreciseInfoBuilder builder;
    builder.setInfo(points, vCount);

    result = builder.constructPreciseInfo();
    aabb = builder.getBoundingBox();

    return true;
}


}

}
