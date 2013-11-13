/*
 * OgreUtil.h
 *
 *  Created on: Aug 7, 2013
 *      Author: agustin
 */

#ifndef OGREUTIL_H_
#define OGREUTIL_H_

#include <OgreMatrix4.h>
#include <OgreString.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>

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
                 core::size_t& vCount,
                 unsigned long* indices,
                 core::size_t iCount);


// @brief Get the contour of a mesh mapped into a 2D list of vertices in CCW order
// @param mesh      The mesh that we want to extract the information
// @param vCount    [in|out] The max number of vertices we can handle
//                           And the resulting number of vertices read.
// @param vertices  The vertex buffer of size vCount where we will read all the
//                  vertices and put them in CCW order.
// @param translate The position of the mesh in the world (scene node position).
//                  This way we can translate each vertex to the correct position.
// @return true on success | false otherwise
// @note The second version (signature) could be called with the list of vertices
//       and indices obtained directly from calling getMeshInformation().
//       The vertices don't need to be sorted nor be unique, this function will
//       remove the duplicated vertices
//
bool
getContourVertices(const Ogre::Mesh* const mesh,
                   core::size_t &vCount,
                   Ogre::Vector3* vertices,
                   const Ogre::Vector3& translate = Ogre::Vector3::ZERO);
bool
getContourVertices(Ogre::Vector3* vertices,
                   core::size_t &vCount,
                   unsigned long* indices,
                   core::size_t iCount);

// @brief Create an entity from a mesh name and return if we could load it or not.
// @param name      The mesh name
// @param sceneMngr The scene manager
// @return loaded entity pointer or 0 on error
//
Ogre::Entity*
loadEntity(const Ogre::String& name, Ogre::SceneManager* sceneMngr);


} /* namespace OgreUtil */
} /* namespace core */
#endif /* OGREUTIL_H_ */
