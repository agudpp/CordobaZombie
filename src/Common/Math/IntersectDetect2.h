/*
 * IntersectDetect.h
 *
 *  Created on: 02/07/2012
 *      Author: emanuel
 *
 */

#ifndef INTERSECTDETECT_H_
#define INTERSECTDETECT_H_

#include "CommonMath.h"

#define TOP 8
#define BOTTOM 4
#define LEFT 1
#define RIGHT 2

class IntersectDetect {
public:

	/**
	 * Check if a line intersects a AABB
	 */
	static bool checkLineAABB(const sm::Point &p1, const sm::Point &p2,
			const sm::AABB &aabb);

	
	static inline int getIntPointsAABBLine(sm::Point p1, sm::Point p2, const sm::AABB &aabb, 
		sm::Point result[2]);
};


/* Computing Sector Point, gives the relative position of the point */
static inline int CompSecPoint(const sm::Point &p, const sm::AABB &aabb)
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

inline int IntersectDetect::getIntPointsAABBLine(sm::Point p1, sm::Point p2, const sm::AABB &aabb, sm::Point result[2])
{
	bool done = false, intersection_point1 = false;
	bool intersection_point2 = false;
	sm::Point newpoint;
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

#endif