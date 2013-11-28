/*
 * CollObjectExporter.h
 *
 *  Created on: Nov 14, 2013
 *      Author: agustin
 */

#ifndef COLLOBJECTEXPORTER_H_
#define COLLOBJECTEXPORTER_H_

#include <OgreMesh.h>

#include <math/AABB.h>

namespace coll {

// Forward
//
struct CollPreciseInfo;

namespace CollObjectExporter {

// @brief Transform a .mesh object into a .2DColl object (or whatever the extension
//        will be). This will basically save the information needed to create
//        a PreciseInfo structure.
// @param mesh      The mesh to be exported
// @param outFile   The out filename where we will save the data
// @return true on success | false otherwise
//
bool
transform(const Ogre::Mesh* mesh, const std::string& outFile);

// @brief Construct a CollPrecise info from a given full .2DColl file path.
// @param collPath      The collision file path to be loaded
// @param result        The resulting collPreciseInfo just built. Note that
//                      if this function returns true & result == 0 is not an
//                      error, it means that the CollPreciseInfo is not necessary
//                      since the object is just an AABB
// @param aabb          The associated bounding box to the object
// @returns the allocated CollPreseiceInfo structure | 0 on error
// @note The allocated memory should be free by the caller
//
bool
buildFromFile(const std::string& collPath,
              CollPreciseInfo*& result,
              core::AABB& aabb);

}

}

#endif /* COLLOBJECTEXPORTER_H_ */
