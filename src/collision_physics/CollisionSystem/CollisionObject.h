/*
 * CollisionObject.h
 *
 *  Created on: 16/02/2012
 *      Author: agustin
 *
 */

#ifndef COLLISIONOBJECT_H_
#define COLLISIONOBJECT_H_

#include <vector>

#include <Box2D/Collision/b2Collision.h>
#include "CollisionTypedefs.h"
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>

namespace c_p {

struct CollisionObject {
    struct Box2DData {
        Box2DData() : realShape(0)
        {
        }

        ~Box2DData()
        {
            // TODO: si tenemos algun puntero aca entonces lo tenemos
            // que eliminar como corresponde (llamando alguna funcion de
            // box 2d probablemente).
            delete realShape;
        }

        b2Shape         *realShape; // for an accurate shape if we have one
        b2Transform     transform;  //Position and rotation of the real shape
    };

	core::AABB		bb;	// bounding box
	int				numEdges;	//Numbre of edeges of the real shape. If it is a
								//circle, numEdges = 0;
	mask_t			maskFlag;	// the flags of this collision object
	mask_t			groupMask;	// group mask
	objectType_t	type;		// identify the object
	void			*userDefined; // add user info
	Box2DData       *b2dData;
								// at the same time where:
								// 0 ************ 1
								//	 *			*
								//	 *	 AABB	*
								//	 *			*
								// 2 ************ 3




	CollisionObject() : b2dData(0), maskFlag(0), groupMask(0),
			userDefined(0)
	{}
	~CollisionObject()
	{
	    delete b2dData;
	}

	inline const core::Vector2 &getPosition(void) const {return bb.pos;}
	
	inline void setPosition(const core::Vector2 &p)
	{
		if(b2dData) b2dData->transform.Set(b2Vec2(p.x, p.y),0);
		bb.setPosition(p);

	}


	////////////////////////////////////////////////////////////////////////////
	//		Functions used to get the intersection points					  //
	////////////////////////////////////////////////////////////////////////////

	/**
	 * Get the intersection point from an AABB and the object
	 */
	void getIntPoints(const core::AABB &aabb, std::vector<core::Vector2> &result) const;

	/**
	 * Get the intersection between this object and a line
	 */
	void getIntPoints(const core::Vector2 &p1, const core::Vector2 &p2,
			std::vector<core::Vector2> &result) const;
	
	/**
	 * Get the intersection between this object and other object
	 */
	void getIntPoints(const CollisionObject *other,
			std::vector<core::Vector2> &result) const;
	


//	inline bool checkCollision(realShape, transform, CollisionObject* objectB){
//		static b2Manifold checkerManifold;
//
//		static b2PolygonShape* objectBShape;
//		static b2Transform objectBTransform;
//		static b2Vec2 objectBPos;
//
//		objectBShape->setAsBox( objectB.bb.getWidth()* 0.5f ,
//				objectB.bb.getHeight() * 0.5f ) ;
//		objectBPos = b2Vec2(objectB.bb.pos.x, objectB.bb.pos.y);
//		objectBTransform.Set(objectBPos, 0.0f);
//
//
//		if (numEdges == 0) //We check collision for a cirle and a box
//			b2CollidePolygonAndCircle(&checkerManifold, objectBShape , objectBTransform, static_cast<b2CircleShape*>(realShape) , transform);
//
//		else //We check collision for any other polygon shape and a box (triangles, pentagons, etc are all b2polygonShape)
//			b2CollidePolygons(&checkerManifold, objectBShape , objectBTransform, static_cast<b2PolygonShape*>(realShape) , transform);
//
//
//		//The Collision has already been processed. We check whether collision really happened
//
//		if(checkerManifold.pointCount > 0){
////			std::cout << "chocan\n";
//			return true;
//		}
//		else{
////			std::cout << "No chocan\n";
//			return false;
//		}
	
	
//	}

};

}

#endif /* COLLISIONOBJECT_H_ */
