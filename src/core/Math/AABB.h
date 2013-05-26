/*
 * AABB.h
 *
 *  Created on: 21/02/2012
 *      Author: agustin
 *
 */

#ifndef AABB_H_
#define AABB_H_

#include "Vec2.h"

namespace math {

// Aligned box2D
struct AABB {
	Vector2 tl;
	Vector2 br;
	Vector2 pos;

	AABB(){}
	AABB(const Vector2 &topLeft, const Vector2 &bottomRight)
	{
		tl = topLeft;
		br = bottomRight;
		pos = (tl + br) * 0.5f;
	}

	// check if a point is inside of the box
	inline bool checkPointInside(const Point &p) const
	{
		return p.x >= tl.x && p.x <= br.x && p.y >= br.y && p.y <= tl.y;
	}

	// translate the bounding box
	inline void translate(const Vector2 &v)
	{
		tl += v;
		br += v;
		pos += v;
	}

	inline void setSize(const float x, const float y)
	{
		// init at the (0,0) pos
		tl.x = 0; tl.y = y;
		br.x = x; br.y = 0;
		pos.x = x *0.5f;
		pos.y = y *0.5f;
	}

	inline float getHeight(void) const {return tl.y - br.y;}
	inline float getWidth(void) const {return br.x - tl.x;}

	// set the AABB in position v as center
	inline void setPosition(const Vector2 &v)
	{
		float aux = (br.x - tl.x) * 0.5f;
		tl.x = v.x - aux; br.x = v.x + aux;
		aux = (tl.y - br.y) * 0.5f;
		tl.y = aux + v.y; br.y = v.y - aux;
		pos = v;
	}

	// check the collision
	inline bool collide(const AABB &o) const
	{
		return !((o.br.x < tl.x) || (o.tl.x > br.x) || (o.tl.y < br.y) ||
				(tl.y < o.br.y));
	}


};


typedef AABB AlignedBox;

}

#endif /* AABB_H_ */
