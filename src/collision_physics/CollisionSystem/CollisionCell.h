/*
 * CollisionCell.h
 *
 *  Created on: 16/02/2012
 *      Author: agustin
 *
 */

#ifndef COLLISIONCELL_H_
#define COLLISIONCELL_H_

#include "DebugUtil.h"
#include "CollisionObject.h"
#include "IntersectDetect.h"

// TODO: alomejor cambiar vector por un arbol u otra estructura
#include <vector>
#include <set>


class CollisionCell {
public:
	CollisionCell(){};
	virtual ~CollisionCell(){};

	// add new object
	inline void addObject(const CollisionObject *o)
	{
#ifdef DEBUG
		const int le = mObjs.size();
		for(int i = 0; i < le; ++i){
			if(mObjs[i] == o){
				ASSERT(false);
			}
		}
#endif
		mObjs.push_back(o);

	}


	// remove object
	inline void removeObject(const CollisionObject *o)
	{
		// TODO: muy lento, modificar esto
		// Para solucionarlo lo que podemos hacer es meter dentro del objeto
		// los indices en la posicion del arreglo que esta en esta celda,
		// si asumimos que un objeto no puede estar mas que en 4 celdas (osea,
		// su tamanio no excede 2 celdas, podemos enumerar las celdas de forma
		// tal que exitan 4 shorts en cada CollisionObject que indiquen la posicion
		// en el arreglo en cada una de las celdas (4 posibles celdas que puede
		// colisionar.....
		const int le = mObjs.size();
		for(int i = 0; i < le; ++i){
			if(mObjs[i] == o){
				removeElement(i, le-1);
				return;
			}
		}
	}

	// remove all the objects
	inline void removeAllObjects(void)
	{
		mObjs.clear();
	}

	// get all the collision object intersecting one
	inline void getAllCollisions(const CollisionObject *o,
			std::vector<const CollisionObject *> &objs) const
	{
		// TODO: mejorar esto, si ordemoos el arreglo por orden de aabb en el
		// eje X entonces es mas rapido encontrar quines intersecan.
		// Otra forma puede ser usando bitwise operators pero nos limitamos
		// a 32 objetos por celda.
		// Otra forma mas es usar un arbol o un quadtree para cada celda..
		const CollisionObject *aux;

		// get the dynamic ones
		const int le = mObjs.size();
		for(int i = 0; i < le; ++i){
			aux = mObjs[i];
			if(o != aux && o->bb.collide(aux->bb)){
				// check for furder collision here... or not?
				objs.push_back(aux);
			}
		}


		// TODO: tendriamos que implementar otra matriz aca de N lugares, o
		// un arreglo de N lugares y hacemos nuevamente un segundo hashing
		// para determinar entre que posiciones deberia ir cada objeto...
	}

	// get all the collision object intersecting one and using mask
	inline void getAllCollisionsMask(const CollisionObject *o,
			std::vector<const CollisionObject *> &objs) const
	{
		// TODO: mejorar esto, si ordemoos el arreglo por orden de aabb en el
		// eje X entonces es mas rapido encontrar quines intersecan.
		// Otra forma puede ser usando bitwise operators pero nos limitamos
		// a 32 objetos por celda.
		// Otra forma mas es usar un arbol o un quadtree para cada celda..
		const CollisionObject *aux;

		const int le = mObjs.size();
		for(int i = 0; i < le; ++i){
			aux = mObjs[i];
			if(o != aux && (o->groupMask & aux->maskFlag) && o->bb.collide(aux->bb)){
				// check for furder collision here... or not?
				objs.push_back(aux);
			}
		}

		// TODO: tendriamos que implementar otra matriz aca de N lugares, o
		// un arreglo de N lugares y hacemos nuevamente un segundo hashing
		// para determinar entre que posiciones deberia ir cada objeto...
	}


	// Returns all the collision objects using only mask group
	inline void getAllCollisionsMask(std::set<const CollisionObject *> &objs,
			mask_t groupMask, const math::AABB &aabb) const
	{
		const CollisionObject *aux;
		const int le = mObjs.size();
		for(int i = 0; i < le; ++i){
			aux = mObjs[i];
			if((groupMask & aux->maskFlag) && aux->bb.collide(aabb)){
				// check for furder collision here... or not?
				objs.insert(aux);
			}
		}
	}

	// Returns all the collision objects using mask group and a certain point
	inline void getAllCollisionMask(std::vector<const CollisionObject *> &objs,
			mask_t groupMask, const math::Vector2 &p) const
	{
		const CollisionObject *aux;
		const int le = mObjs.size();
		for(int i = 0; i < le; ++i){
			aux = mObjs[i];
			if((groupMask & aux->maskFlag) && aux->bb.checkPointInside(p)){
				// check for furder collision here... or not?
				objs.push_back(aux);
			}
		}
	}

	// Returns all the collision objects using mask group and a certain point
	inline void getAllCollisionMask(std::set<const CollisionObject *> &objs,
			mask_t groupMask, const math::Vector2 &p1, const math::Vector2 &p2) const
	{
		const CollisionObject *aux;
		const int le = mObjs.size();
		for(int i = 0; i < le; ++i){
			aux = mObjs[i];
			if((groupMask & aux->maskFlag) &&
					IntersectDetect::checkLineAABB(p1,p2,aux->bb)){
				// check for furder collision here... or not?
				objs.insert(aux);
			}
		}
	}

private:
	inline void removeElement(int i, int le)
	{
		mObjs[i] = mObjs[le]; mObjs.pop_back();
	}

private:
	std::vector<const CollisionObject *>	mObjs;
};

#endif /* COLLISIONCELL_H_ */
