/*
 * IntersectDetect.h
 *
 *  Created on: 12/04/2012
 *      Author: agustin
 *
 */

#ifndef INTERSECTDETECT_H_
#define INTERSECTDETECT_H_

#include "CommonMath.h"
#include <algorithm>

namespace core {

class IntersectDetect {
	enum {
		LEFT 	= 1,
		RIGHT 	= 2,
		BOTTOM 	= 4,
		TOP 	= 8,
	};
public:

	/**
	 * Check if a line intersects a AABB
	 */
	static bool checkLineAABB(const core::Vector2 &p1, const core::Vector2 &p2,
			const core::AABB &aabb);


	/**
	 * Line with line intersection check
	 */
	static inline bool lineIntersec(float x1, float y1, float x2, float y2, float x3,
			float y3, float x4, float y4);
	static inline bool lineIntersec(float x1, float y1, float x2, float y2, float x3,
			float y3, float x4, float y4, float &resultX, float &resultY);


	/*
	 * Returns intersections points from an AABB and a line.
	 * @param	result	The vector where we will set the intersections points
	 * @return	the number of intersections point
	 */
	static inline int getIntPointsAABBLine(core::Vector2 p1, core::Vector2 p2,
			const core::AABB &aabb,
		core::Vector2 result[2]);

private:
	static inline int CompSecPoint(const core::Vector2 &p, const core::AABB &aabb);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
inline bool IntersectDetect::lineIntersec(float x1, float y1, float x2, float y2, float x3,
			float y3, float x4, float y4)
{
	float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	if (d == 0)
		return false;
	float xi = ((x3 - x4) * (x1 * y2 - y1 * x2) - (x1 - x2) * (x3 * y4 - y3
			* x4)) / d;
	float yi = ((y3 - y4) * (x1 * y2 - y1 * x2) - (y1 - y2) * (x3 * y4 - y3
			* x4)) / d;
	if (x3 == x4) {
		if (yi < ((y1<y2)?y1:y2) || yi > std::max(y1, y2))
			return false;
	}
	//Point2D.Double p = new Point2D.Double(xi,yi);
	if (xi < std::min(x1, x2) || xi > std::max(x1, x2))
		return false;
	if (xi < std::min(x3, x4) || xi > std::max(x3, x4))
		return false;

	return true;
//	return p;
}

////////////////////////////////////////////////////////////////////////////////
inline bool IntersectDetect::lineIntersec(float x1, float y1, float x2, float y2, float x3,
			float y3, float x4, float y4, float &xi, float &yi)
{
	// TODO Optimizar esta bosta!
    const float firstLineSlopeX = x2 - x1;
    const float firstLineSlopeY = y2 - y1;

    const float secondLineSlopeX = x4 - x3;
    const float secondLineSlopeY = y4 - y3;

    float s, t;
    s = (-firstLineSlopeY * (x1 - x3) + firstLineSlopeX * (y1 - y3)) / (-secondLineSlopeX * firstLineSlopeY + firstLineSlopeX * secondLineSlopeY);
    t = (secondLineSlopeX * (y1 - y3) - secondLineSlopeY * (x1 - x3)) / (-secondLineSlopeX * firstLineSlopeY + firstLineSlopeX * secondLineSlopeY);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
    	xi = x1 + (t * firstLineSlopeX);
    	yi = y1 + (t * firstLineSlopeY);

        // Collision detected
        return true;
    }

    return false; // No collision
}


/* Computing Sector Point, gives the relative position of the point */
inline int IntersectDetect::CompSecPoint(const core::Vector2 &p,
		const core::AABB &aabb)
{
	int rst_flag = 0;
	float x,y;
	if (p.y > aabb.tl.y) rst_flag |= TOP;
	else if (p.y < aabb.br.y) rst_flag |= BOTTOM;

	if (p.x > aabb.br.x) rst_flag |= RIGHT;
	else if (p.x < aabb.tl.x) rst_flag |= LEFT;
	return rst_flag;
}

/* This is the algorithm of Cohen-Sutherland*/
inline int IntersectDetect::getIntPointsAABBLine(core::Vector2 p1, core::Vector2 p2,
		const core::AABB &aabb, core::Vector2 result[2])
{
	bool done = false, intersection_point1 = false;
	bool intersection_point2 = false;
	core::Vector2 newpoint;
	int collitions = 0;
	int pos_p1 = CompSecPoint(p1, aabb);
	int pos_p2 = CompSecPoint(p2, aabb);
	int operation_flag;
	while (!done)
	{
		if (!pos_p1 && !pos_p2)
			{
				done = true;
				/* yes! in trivialy accept do not set the intersection_point *
				 * because these 2 point live in the Viewport, the result of *
				 * collitions is 0, but 0 is false, the return -1 to say no  *
				 * have intersect but occurs collition                       */
				if(!intersection_point1 && !intersection_point2) return -1;
			}
		else if (pos_p1 & pos_p2) // No point intersects with the Viewport
		{

			done = true;
			intersection_point1 = false;
			intersection_point2 = false;
		}

		else // some point are in the Viewport
		{

			if (pos_p1){ // this analyse if the first point is not in Viewport
				operation_flag = pos_p1;
				intersection_point1 = true;
			}
			else{
				operation_flag = pos_p2;
				intersection_point2 = true;
			}

			if (operation_flag & TOP)
			{
				newpoint.x = p1.x + (p2.x - p1.x) * (aabb.tl.y - p1.y) /
				                            (p2.y - p1.y);
				newpoint.y = aabb.tl.y;
			}
			else if (operation_flag & BOTTOM)
			{
				newpoint.x = p1.x + (p2.x - p1.x) * (aabb.br.y - p1.y) /
				                            (p2.y - p1.y);
				newpoint.y = aabb.br.y;
			}

			if (operation_flag & RIGHT)
			{
				newpoint.x = aabb.br.x;
				newpoint.y = p1.y + (p2.y - p1.y) * (aabb.br.x - p1.x) /
				                            (p2.x - p1.x);
			}
			else if (operation_flag & LEFT)
			{
				newpoint.x = aabb.tl.x;
				newpoint.y = p1.y + (p2.y - p1.y) * (aabb.tl.x - p1.x) /
				                            (p2.x - p1.x);
			}

			if (operation_flag == pos_p1)
			{
				pos_p1 = CompSecPoint(newpoint, aabb);
				p1.x = newpoint.x;
				p1.y = newpoint.y;
			}
			else
			{
				pos_p2 = CompSecPoint(newpoint, aabb);
				p2.x = newpoint.x;
				p2.y = newpoint.y;
			}

		}
	}
	if(intersection_point1 || intersection_point2)
	{
		if(p1.x == p2.x && p1.y == p2.y) // equals points
			result[collitions++] = p1;
		else{
			if(intersection_point1) result[collitions++] = p1;
			if(intersection_point2) result[collitions++] = p2;
		}
	}
	return collitions;
}

}

#endif /* INTERSECTDETECT_H_ */
