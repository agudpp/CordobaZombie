/*
 * OgreUtils.h
 *
 *  Created on: May 26, 2013
 *      Author: agustin
 */

#ifndef OGREUTILS_H_
#define OGREUTILS_H_

#include <list>

#include <OgreMesh.h>
#include <OgreMatrix4.h>
#include <OgreVector3.h>
#include <OgreAnimationState.h>
#include <OgreSceneManager.h>
#include <OgreAnimation.h>
#include <OgreString.h>
#include <OgreSceneNode.h>
#include <OgreSubMesh.h>

// TODO: this should be removed at least there is a ticket for this (ID 0027)
#include <PathfindingSystem/ConvexPolygon/PolyStructsContainer.h>

// Forward
class TiXmlElement;

namespace game {
namespace OgreUtils {

/* Function used to load an AnimationState to a sceneNode.
 * Params:
 *  @scnManager     the SceneManager
 *  @node           The SceneNode to load the AnimationStates
 *  @elem           The TiXmlElement where is the animation
 *  animList        The animations result list
 * Returns:
 *  true            on success
 */
static bool
getAnimation(Ogre::SceneManager *scnManager,
             Ogre::SceneNode *node,
             TiXmlElement *elem,
             std::list<Ogre::AnimationState *> &animList);

/**
 * Get an animation from an xml document.
 *  @scnManager     the SceneManager
 *  @node           The SceneNode to load the AnimationStates
 *  @xmlFileName    The xml filename, where we will parse the <animation>
 *                  node (if more than one then we will parse the first one).
 */
static Ogre::AnimationState *
getAnimationFromFile(Ogre::SceneManager *scnManager,
                     Ogre::SceneNode *node,
                     const char *xmlFilename);

/* Removes all the entities of a sceneNode (recursively) */
static void
removeAllEntities(Ogre::SceneNode *node);

/* Remove all (entities and childrens scene nodes) and the node itself */
static void
removeAllFromNode(Ogre::SceneNode *node);

/* Creates a list of triangles and allocating all the vertexs from a mesh
 * Requires:
 *  @cont       The Vertex container where it will been put the vertexs
 *  @triangles  The Triangles container
 *  @mesh       The mesh to extract the triangles from
 *  @trmatrix   The mesh's parent SceneNode transformation matrix
 * Returns:
 *  true        on success
 *  false       otherwise
 */
static bool
getTrianglesFromMesh(PolyStructsContainer<core::Vertex *> &cont,
                     PolyStructsContainer<Triangle *> &triangles,
                     Ogre::MeshPtr mesh,
                     const Ogre::Matrix4 &trmatrix = Ogre::Matrix4::IDENTITY);

/**
 * Get the resource absolute path
 */
static int
getResourcePath(const Ogre::String& resourceGroup,
                const Ogre::String& resourceName,
                Ogre::String &resourcePath);
}
} /* namespace game */
#endif /* OGREUTILS_H_ */
