/*
 * CollObjBuilder.h
 *
 *  Created on: 23/04/2012
 *      Author: agustin
 */

#ifndef COLLOBJBUILDER_H_
#define COLLOBJBUILDER_H_

#include <OgreEntity.h>
#include <OgreMesh.h>
#include <OgreSubEntity.h>

#include "CollisionObject.h"
#include "Util.h"

class CollObjBuilder
{
public:
	/**
	 * Create a collision object from an entity.
	 * @param 	entity
	 * @param	userDef	The type of the object to construct:
	 * 			"aabb", "circle", "poly", "edge", "box" (this is not an axis
	 * 			aligned box
	 *
	 * @return	CollisionObject
	 */
	static CollisionObject *createFromMesh(Ogre::Entity *ent,
			const Ogre::String &userDef);

private:
	/**
	 * Create Polygon shape (counterclockwise vertex)
	 */
	static CollisionObject *createPolyShape(PolyStructsContainer<sm::Vertex *> &cont,
			PolyStructsContainer<Triangle *> &triangles);

	/**
	 * Create circle shape
	 */
	static CollisionObject *createCircleShape(PolyStructsContainer<sm::Vertex *> &cont,
			PolyStructsContainer<Triangle *> &triangles);

	/**
	 * Edge shape
	 */
	static CollisionObject *createEdgeShape(PolyStructsContainer<sm::Vertex *> &cont,
			PolyStructsContainer<Triangle *> &triangles);

	/**
	 * Box shape
	 */
	static CollisionObject *createBoxShape(PolyStructsContainer<sm::Vertex *> &cont,
			PolyStructsContainer<Triangle *> &triangles);

	/**
	 * AABB shape
	 */
	static CollisionObject *createAABBShape(PolyStructsContainer<sm::Vertex *> &cont,
			PolyStructsContainer<Triangle *> &triangles);

	/**
	 * Get the bounding box of a list of vertex
	 */
	static void getBoundingBox(std::vector<sm::Vertex *> &v, sm::AABB &bb);
	static void getBoundingBox(std::vector<const sm::Vertex *> &v, sm::AABB &bb);
};

#endif /* COLLOBJBUILDER_H_ */
