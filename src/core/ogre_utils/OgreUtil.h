/*
 * OgreUtil.h
 *
 *  Created on: Aug 7, 2013
 *      Author: agustin
 */

#ifndef OGREUTIL_H_
#define OGREUTIL_H_

#include <OgreMatrix4.h>

#include <types/basics.h>

namespace core {
namespace OgreUtil {

// @brief Get the mesh information (triangles and vertices from a mesh).
// @param mesh      The mesh that we want to extract the information
// @param vCount    [in|out] The max number of vertices we can handle
//                           And the resulting number of vertices read.
// @param vertices  The vertex buffer of size vCount where we will read all the
//                  vertices.
// @param iCount    [in|out] The size of the indices buffer.
//                           And the resulting number of indices read.
// @param indices   The buffer of size iCount where we will put the indices of
//                  the triangles.
// @param transform The transform we want to apply to each vertex, if
//                  transform == Ogre::Matrix4::IDENTITY no transformation will be
//                  applied
// @return true on success | false otherwise
//
bool
getMeshInformation(const Ogre::Mesh* const mesh,
                   core::size_t &vCount,
                   Ogre::Vector3* vertices,
                   core::size_t &iCount,
                   unsigned long* indices,
                   const Ogre::Matrix4& transform = Ogre::Matrix4::IDENTITY);

// @brief Remove all the duplicated vertices and re-organize the triangles to
//        maintain the same structure.
// @param vertices      The list of vertices to manipulate
// @param vCount        The number of vertices
// @param indices       The indices conforming the triangles
// @param iCount        The number of indices
// @note that we will modify that values, so if you cannot modify them do a
//       copy before calling this method.
//
void
removeDuplicated(Ogre::Vector3* vertices,
                 unsigned int& vCount,
                 unsigned long* indices,
                 unsigned int& iCount);

} /* namespace OgreUtil */
} /* namespace core */
#endif /* OGREUTIL_H_ */
