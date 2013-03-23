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

/**
 ** TODO: revisar el código entero y reescribirlo
 **       La idea es que esto genere un archivo .txt con la info geométrica
 **       de colisiones de los entities del juego.
 **       Después, en tiempo de ejecución leemos ese .txt y con él generamos
 **       dinámicamente los CollisionObjects
 **/


class CollObjExporter
{
public:
	/**
	 ** @brief
	 ** Extract all 2D positions from given SceneNode,
	 ** which are relevant to the collision system.
	 **
	 ** @return
	 ** String containing a sequence of 2D positions in the format:
	 ** <SceneNode name>\n
	 ** <number of elements>\n
	 ** <elem1_name elem1_attr1 elem1_attr2 ... elem1_attrN1>\n
	 ** <elem2_name elem2_attr1 elem2_attr2 ... elem2_attrN2>\n
	 ** ...
	 ** <elemM_name elemM_attr1 elemM_attr2 ... elemM_attrNM>\n
	 **/
	static std::string
	sceneExtractor(Ogre::SceneNode* scene);

	/**
	 ** @brief
	 ** Extract 2D positions from given Entity, for the collision system.
	 **
	 ** @return
	 ** String containing a sequence of 2D points positions in the format:
	 ** <elem_name elem_attr1 elem_attr2 ... elem_attrN>\n
	 **/
	static std::string
	createFromMesh(Ogre::Node *node, Ogre::Entity *ent, const Ogre::String &userDef);

private:
	static Ogre::String userDefExtractor(const Ogre::String &source);
	/**
	 * Create Polygon shape (counterclockwise vertex)
	 */
	static std::string createPolyShape(const Ogre::Vector3 &mod_pos, PolyStructsContainer<sm::Vertex *> &cont,
			PolyStructsContainer<Triangle *> &triangles);

	/**
	 * Create circle shape
	 */
	static std::string createCircleShape(const Ogre::Vector3 &mod_pos, PolyStructsContainer<sm::Vertex *> &cont,
			PolyStructsContainer<Triangle *> &triangles);

	/**
	 * Edge shape
	 */
	static std::string createEdgeShape(const Ogre::Vector3 &mod_pos, PolyStructsContainer<sm::Vertex *> &cont,
			PolyStructsContainer<Triangle *> &triangles);

	/**
	 * Box shape
	 */
	static std::string createBoxShape(const Ogre::Vector3 &mod_pos, PolyStructsContainer<sm::Vertex *> &cont,
			PolyStructsContainer<Triangle *> &triangles);

	/**
	 * AABB shape
	 */
	static std::string createAABBShape(const Ogre::Vector3 &mod_pos, PolyStructsContainer<sm::Vertex *> &cont,
			PolyStructsContainer<Triangle *> &triangles);

	/**
	 * Get the bounding box of a list of vertex
	 */
	static void getBoundingBox(std::vector<sm::Vertex *> &v, sm::AABB &bb);
	static void getBoundingBox(std::vector<const sm::Vertex *> &v, sm::AABB &bb);
};

#endif /* COLLOBJEXPORTER_H_ */
