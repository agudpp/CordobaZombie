/*
 * CollObjExporter.h
 *
 *  Created on: 23/05/2012
 *      Author: santiago
 */

#ifndef COLLOBJEXPORTER_H_
#define COLLOBJEXPORTER_H_

#include <OgreEntity.h>
#include <OgreMesh.h>
#include <OgreSubEntity.h>

#include "CollisionObject.h"
#include "Util.h"

class CollObjExporter
{
public:
	/**
	 * loads a .scene file and extracts the position of the collision objects
	 */
	static std::string CollObjExporter::sceneExtractor(Ogre::SceneNode* scene);
	static std::string CollObjExporter::createFromMesh(Ogre::Node* node, Ogre::Entity *ent,
			const Ogre::String &userDef);

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
