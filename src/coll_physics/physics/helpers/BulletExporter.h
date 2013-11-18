/*
 * BulletExporter.h
 *
 *  Created on: Nov 15, 2013
 *      Author: agustin
 */

#ifndef BULLETEXPORTER_H_
#define BULLETEXPORTER_H_

#include <string>
#include <OgreMesh.h>

#include <bullet/btBulletCollisionCommon.h>

namespace physics {

namespace BulletExporter {

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
bool
transform(const Ogre::Mesh* mesh, const std::string& outFile);

// @brief Create a bullet shape representation from a file (.3DColl).
// @param file      The file name where the info is.
// @return the new allocated bullet shape | 0 on error.
// @note that the caller of this method is the owner of the memory now.
//
btCollisionShape*
buildShapeFromFile(const std::string& file);

}

}

#endif /* BULLETEXPORTER_H_ */
