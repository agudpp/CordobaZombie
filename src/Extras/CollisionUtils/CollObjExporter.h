/*
 * CollObjExporter.h
 *
 *  Created on: Mar 23, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef COLLOBJEXPORTER_H_
#define COLLOBJEXPORTER_H_

#include <OgreEntity.h>
#include <OgreMesh.h>
#include <OgreSubEntity.h>

#include "CollisionObject.h"
#include "DebugUtil.h"
#include "Util.h"



class CollObjExporter
{
public:
	/**
	 ** @brief
	 ** Call sceneExtractor() for every .scene file in CWD
	 **
	 ** @return
	 ** String containing a sequence of 2D positions, as for sceneExtractor(),
	 ** one for each .scene file in CWD. Format is:
	 ** <SceneNode1 name>\n
	 ** <number of elements>\n
	 ** <elem1_name elem1_attr1 elem1_attr2 ... elem1_attrN1>\n
	 ** ...
	 ** <elemM_name elemM_attr1 elemM_attr2 ... elemM_attrNM>\n
	 ** \n
	 ** <SceneNode2 name>\n
	 ** <number of elements>\n
	 ** etc.
	 **
	 ** @remarks
	 ** See sceneExtractor() for detailed info about the returned string format.
	 **/
	static std::string
	dirExtractor(void);

	/**
	 ** @brief
	 ** Call entityExtractor() for every entity contained in given SceneNode.
	 **
	 ** @return
	 ** String containing a sequence of 2D positions in the format:
	 ** <SceneNode name>\n
	 ** <number of elements>\n
	 ** <elem1_name elem1_attr1 elem1_attr2 ... elem1_attrN1>\n
	 ** <elem2_name elem2_attr1 elem2_attr2 ... elem2_attrN2>\n
	 ** ...
	 ** <elemM_name elemM_attr1 elemM_attr2 ... elemM_attrNM>\n
	 ** \n
	 **/
	static std::string
	sceneExtractor(Ogre::SceneNode* scene);

	/**
	 ** @brief
	 ** Extract 2D positions from given Entity, for the collision system.
	 **
	 ** @return
	 ** String containing a sequence of 2D points positions in the format:
	 ** <elem_name  elem_attr1  elem_attr2  ...  elem_attrN>\n
	 **
	 ** @remarks
	 ** See create***Shape() for detailed info about the returned string format.
	 **/
	static std::string
	entityExtractor(Ogre::Node *node, Ogre::Entity *ent, const Ogre::String &userDef);

private:
	/**
	 ** @brief
	 ** Get mesh type from source name
	 **
	 ** @return
	 ** Either "box", "aabb", "edge", "circle" or "poly"
	 **/
	static std::string
	userDefExtractor(const Ogre::String &source);

	/**
	 ** @brief
	 ** Extract 2D positions from given polygon shape
	 **
	 ** @return
	 ** "poly  num_verts  vert0.x  vert0.y  ...  vertN.x  vertN.y\n"
	 ** "" on error
	 **
	 ** @remarks
	 ** Vertices are returned counterclockwise
	 **/
	static std::string
	createPolyShape(const Ogre::Vector3 &mod_pos,
					PolyStructsContainer<sm::Vertex *> &cont,
					PolyStructsContainer<Triangle *> &triangles);

	/**
	 ** @brief
	 ** Extract 2D positions from given circle shape
	 **
	 ** @return
	 ** "circle  center.x  center.y  radius\n"
	 ** "" on error
	 **/
	static std::string
	createCircleShape(const Ogre::Vector3 &mod_pos,
					  PolyStructsContainer<sm::Vertex *> &cont,
					  PolyStructsContainer<Triangle *> &triangles);

	/**
	 ** @brief
	 ** Extract 2D positions from given edge shape
	 **
	 ** @return
	 ** "edge  vert0.x  vert0.y  vert1.x  vert1.y\n"
	 ** "" on error
	 **/
	static std::string
	createEdgeShape(const Ogre::Vector3 &mod_pos,
					PolyStructsContainer<sm::Vertex *> &cont,
					PolyStructsContainer<Triangle *> &triangles);

	/**
	 ** @brief
	 ** Extract 2D positions from given box shape
	 **
	 ** @return
	 ** "box vert0.x vert0.y vert1.x vert1.y vert2.x vert2.y vert3.x vert3.y\n"
	 ** "" on error
	 **/
	static std::string
	createBoxShape(const Ogre::Vector3 &mod_pos,
				   PolyStructsContainer<sm::Vertex *> &cont,
				   PolyStructsContainer<Triangle *> &triangles);

	/**
	 ** @brief
	 ** Extract 2D positions from given AABB shape
	 **
	 ** @return
	 ** "box vert0.x vert0.y vert1.x vert1.y vert2.x vert2.y vert3.x vert3.y\n"
	 ** "" on error
	 **/
	static std::string
	createAABBShape(const Ogre::Vector3 &mod_pos,
					PolyStructsContainer<sm::Vertex *> &cont,
					PolyStructsContainer<Triangle *> &triangles);

	/**
	 ** @brief
	 ** Create a bounding box from a list of 2D vertices
	 **/
	static void
	getBoundingBox(std::vector<const sm::Vertex *> &v, sm::AABB &bb);
};

#endif /* COLLOBJEXPORTER_H_ */
