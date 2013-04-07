/*
 * Util.h
 *
 *  Created on: 08/11/2011
 *      Author: agustin
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <OgreAnimationState.h>
#include <OgreSceneManager.h>
#include <OgreAnimation.h>
#include <OgreString.h>
#include <OgreSceneNode.h>
#include <OgreMesh.h>
#include <OgreSubMesh.h>

#include <list>

#include "DebugUtil.h"
#include "PolyStructsContainer.h"
#include "Triangle.h"
#include "tinyxml.h"

class TiXmlElement;
class TiXmlDocument;

namespace Common
{

class Util
{
public:
	/* Function used to load an AnimationState to a sceneNode.
	 * Params:
	 * 	@scnManager		the SceneManager
	 * 	@node			The SceneNode to load the AnimationStates
	 * 	@elem			The TiXmlElement where is the animation
	 * 	animList		The animations result list
	 * Returns:
	 * 	true			on success
	 */
	static bool getAnimation(Ogre::SceneManager *scnManager,
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
	static Ogre::AnimationState *getAnimationFromFile(Ogre::SceneManager *scnManager,
	                                                  Ogre::SceneNode *node,
	                                                  const char *xmlFilename);


	/* Loads an XmlDocument from a filename.
	 * Requires:
	 * 	@filename		The xml filename
	 * Returns:
	 * 	@doc			The xmlDocument or 0 if error
	 * Note: This function allocates a xmlDocument. you have to remove after
	 * using it
	 */
	static TiXmlDocument *loadXmlDocument(const char *fname);


	/* Removes all the entities of a sceneNode (recursively) */
	static void removeAllEntities(Ogre::SceneNode *node);

	/* Remove all (entities and childrens scene nodes) and the node itself */
	static void removeAllFromNode(Ogre::SceneNode *node);


	/* Creates a list of triangles and allocating all the vertexs from a mesh
	 * Requires:
	 * 	@cont		The Vertex container where it will been put the vertexs
	 * 	@triangles	The Triangles container
	 * 	@mesh		The mesh to extract the triangles from
	 * 	@trmatrix	The mesh's parent SceneNode transformation matrix
	 * Returns:
	 * 	true		on success
	 * 	false		otherwise
	 */
	static bool getTrianglesFromMesh(PolyStructsContainer<sm::Vertex *> &cont,
			PolyStructsContainer<Triangle *> &triangles,
			Ogre::MeshPtr mesh,
			const Ogre::Matrix4 &trmatrix = Ogre::Matrix4::IDENTITY);

	/**
	 * Read a environment variable
	 * @param	var		The varname
	 * @param	result	The result of the environment variable
	 * @return 	True on success, false otherwise
	 */
	static bool readEnvVar(const std::string &var, std::string &result);

	/**
	 * Get the resource absolute path
	 */
	static int getResourcePath( Ogre::String resourceGroup
							  , Ogre::String resourceName
							  , Ogre::String &resourcePath
							  );

private:
	static Ogre::Quaternion parseQuaternion(TiXmlElement *XMLNode);
	static Ogre::Vector3 parseVector3(TiXmlElement *XMLNode);

	static void getMeshInformation(const Ogre::Mesh* const mesh,
	                        size_t &vertex_count,
	                        Ogre::Vector3* &vertices,
	                        size_t &index_count,
	                        unsigned long* &indices,
	                        const Ogre::Vector3 &position = Ogre::Vector3::ZERO,
	                        const Ogre::Quaternion &orient = Ogre::Quaternion::IDENTITY,
	                        const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE);

	// Find a vertex from a vector
	static sm::Vertex *findVertex(const std::vector<sm::Vertex *> &vertexs,
			const Ogre::Vector3 &ve);




};

}

#endif /* UTIL_H_ */
