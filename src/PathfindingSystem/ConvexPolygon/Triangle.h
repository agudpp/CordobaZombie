/*
 * Triangle.h
 *
 *  Created on: 20/01/2012
 *      Author: agustin
 *
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "PolygonStructs.h"
#include "DebugUtil.h"

class Triangle {
public:
	const sm::Vertex			*v1;
	const sm::Vertex			*v2;
	const sm::Vertex			*v3;

public:
	// Ensures that the vertex will have valid info
	Triangle(const sm::Vertex *p1, const sm::Vertex *p2, const sm::Vertex *p3);
	Triangle(const Triangle &);
	~Triangle();

	// check if contains a point
	inline bool containsPoint(const sm::Point &p) const
	{
		ASSERT(v1);
		ASSERT(v2);
		ASSERT(v3);
		// TODO: see a better way (faster)
		return checkInside(p);
	}

	// Check if intersects a box
	bool intersectAlignedBox(const sm::AlignedBox &r) const;

	// Get the center point of the triangle (slow)
	void getCenterPoint(sm::Point &p) const;

	/**
	 * Get a random point inside the triangle
	 */
	void getRandomPoint(sm::Point &r) const;

private:
	// TODO: implementar esto mejor mas rapido
	float Sign(const sm::Point &p1, const sm::Point &p2, const sm::Point &p3) const
	{
	  return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}

	inline bool checkInside(const sm::Point &p) const
	{
		bool b1, b2, b3;

		b1 = Sign(p, *v1, *v2) <= 0.0f;
		b2 = Sign(p, *v2, *v3) <= 0.0f;
		b3 = Sign(p, *v3, *v1) <= 0.0f;

		return ((b1 == b2) && (b2 == b3));
	}

	inline bool getIntersectionPoint(const sm::Point &start1,
			const sm::Point &end1, const sm::Point &start2, const sm::Point &end2,
			sm::Point &result) const
	{
		float denom = ((end1.x - start1.x) * (end2.y - start2.y)) - ((end1.y
				- start1.y) * (end2.x - start2.x));

		//  AB & CD are parallel
		if (denom == 0)
			return false;
		denom = 1.0/denom;

		float numer = ((start1.y - start2.y) * (end2.x - start2.x))
				- ((start1.x - start2.x) * (end2.y - start2.y));

		float r = numer * denom;

		float numer2 = ((start1.y - start2.y) * (end1.x - start1.x))
				- ((start1.x - start2.x) * (end1.y - start1.y));

		float s = numer2 * denom;

		if ((r < 0 || r > 1) || (s < 0 || s > 1))
			return false;

		// Find intersection point
		result.x = start1.x + (r * (end1.x - start1.x));
		result.y = start1.y + (r * (end1.y - start1.y));

		return true;
	}

	bool boxLineIntersection(const sm::Point &start1, const sm::Point &end1,
			const sm::AlignedBox &b, sm::Point &result) const
	{
		if(getIntersectionPoint(start1, end1, b.tl, sm::Point(b.br.x, b.tl.y), result) ||
				getIntersectionPoint(start1, end1, b.tl, sm::Point(b.tl.x, b.br.y), result) ||
				getIntersectionPoint(start1, end1, b.br, sm::Point(b.br.x, b.tl.y), result) ||
				getIntersectionPoint(start1, end1, b.br, sm::Point(b.tl.x, b.br.y), result)){
			return true;
		}
		return false;
	}

	Triangle();

private:


};

#endif /* TRIANGLE_H_ */
