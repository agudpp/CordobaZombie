/*
 * Triangle.cpp
 *
 *  Created on: 20/01/2012
 *      Author: agustin
 *
 */

#include <cstring>
#include <cmath>
#include "Triangle.h"
#include "IntersectDetect.h"

#include "DebugUtil.h"


static inline int getVertxPos(const sm::AlignedBox &r, const sm::Vertex *v)
{
	// first get the column
	int column;
	if(v->x <= r.tl.x){
		column = 0;
	} else if(v->x <= r.br.x) {
		column = 1;
	} else {
		column = 2;
	}

	// get the row
	if(v->y >= r.tl.y){
		// row 0
		return column;
	} else if(v->y >= r.br.y){
		//row 1;
		return 3 + column;
	} else {
		return 6 + column;
	}
}



Triangle::Triangle(const sm::Vertex *p1, const sm::Vertex *p2, const sm::Vertex *p3)
{
	ASSERT(p1);
	ASSERT(p2);
	ASSERT(p3);
	v1 = p1;
	v2 = p2;
	v3 = p3;
}

Triangle::Triangle(const Triangle &other)
{
	v1 = other.v1;
	v2 = other.v2;
	v3 = other.v3;

	ASSERT(v1);
	ASSERT(v2);
	ASSERT(v3);
}

Triangle::Triangle()
{
	ASSERT(false);
}

Triangle::~Triangle() {
	// TODO Auto-generated destructor stub
}


// Check if intersects a box
bool Triangle::intersectAlignedBox(const sm::AlignedBox &r) const
{
	// TODO: cambiar esta funcion por algo mucho mas rapido, por el momento
	// vamos a chequear si alguno de los puntos del triangulo esta dentro de la
	// caja o si alguno de los puntos esta dentro del triangulo. LENTISIMO

	if(containsPoint(r.tl) || containsPoint(r.br) ||
			containsPoint(sm::Point(r.tl.x, r.br.y)) || containsPoint(sm::Point(r.br.x, r.tl.y))){
		return true;
	}

	static const sm::Vertex *matrix[9];
//	std::memset(matrix, 0, sizeof(Vertex *)*9);
	for(int i = 8; i >= 0; --i) matrix[i] = 0;

	// first get the vertexs position in the matrix
	matrix[getVertxPos(r, v1)] = v1;
	matrix[getVertxPos(r, v2)] = v2;
	matrix[getVertxPos(r, v3)] = v3;

	// first check if some of the points are in the position 4
	if(matrix[4]){
		// there are intersection
		return true;
	}
	if((matrix[1] && matrix[7]) || (matrix[3] && matrix[5])){
		return true;
	}
	if(matrix[0]){
		if(matrix[1] && matrix[2]){return false;}
		if(matrix[3] && matrix[6]){return false;}
		if(matrix[5]){
			return IntersectDetect::lineIntersec(matrix[0]->x, matrix[0]->y,
					matrix[5]->x, matrix[5]->y, r.tl.x, r.tl.y, r.br.x, r.tl.y);
		/*check 0-5*/
		}
		if(matrix[8]){
			/*check 0-8*/
			return IntersectDetect::lineIntersec(matrix[0]->x, matrix[0]->y,
					matrix[8]->x, matrix[8]->y, r.tl.x, r.tl.y, r.br.x, r.tl.y) ||
					IntersectDetect::lineIntersec(matrix[0]->x, matrix[0]->y,
							matrix[8]->x, matrix[8]->y, r.tl.x, r.tl.y, r.tl.x, r.br.y);
		}
		if(matrix[7]){
			/*check 0-7*/
			return IntersectDetect::lineIntersec(matrix[0]->x, matrix[0]->y,
					matrix[7]->x, matrix[7]->y, r.tl.x, r.tl.y, r.tl.x, r.br.y);
		}
	}
	if(matrix[1]){
		if(matrix[3]){
			/*check 1-3*/
			return IntersectDetect::lineIntersec(matrix[1]->x, matrix[1]->y,
					matrix[3]->x, matrix[3]->y, r.tl.x, r.tl.y, r.br.x, r.tl.y);
		}
		if(matrix[5]){
			/*check 1-5*/
			return IntersectDetect::lineIntersec(matrix[1]->x, matrix[1]->y,
					matrix[5]->x, matrix[5]->y, r.tl.x, r.tl.y, r.br.x, r.tl.y);
		}
		if(matrix[6]){
			/*check 1-6*/
			return IntersectDetect::lineIntersec(matrix[1]->x, matrix[1]->y,
					matrix[6]->x, matrix[6]->y, r.tl.x, r.tl.y, r.br.x, r.tl.y);
		}
		if(matrix[8]){
			/*check 1-8*/
			return IntersectDetect::lineIntersec(matrix[1]->x, matrix[1]->y,
					matrix[8]->x, matrix[8]->y, r.tl.x, r.tl.y, r.br.x, r.tl.y);
		}
	}
	if(matrix[2]){
		if(matrix[5] && matrix[8]){return false;}
		if(matrix[3]){
			/*check 2-3*/
			return IntersectDetect::lineIntersec(matrix[2]->x, matrix[2]->y,
					matrix[3]->x, matrix[3]->y, r.tl.x, r.tl.y, r.br.x, r.tl.y);
		}
		if(matrix[6]){
			/*check 2-6*/
			return IntersectDetect::lineIntersec(matrix[2]->x, matrix[2]->y,
					matrix[6]->x, matrix[6]->y, r.tl.x, r.tl.y, r.br.x, r.tl.y);
		}
		if(matrix[7]){
			/*check 2-7*/
			return IntersectDetect::lineIntersec(matrix[2]->x, matrix[2]->y,
					matrix[7]->x, matrix[7]->y, r.tl.x, r.tl.y, r.br.x, r.tl.y) ||
					IntersectDetect::lineIntersec(matrix[2]->x, matrix[2]->y,
							matrix[7]->x, matrix[7]->y, r.br.x, r.tl.y, r.br.x, r.br.y);
		}
	}
	if(matrix[3]){
		if(matrix[8]){
			/*check 3-8*/
			return IntersectDetect::lineIntersec(matrix[3]->x, matrix[3]->y,
					matrix[8]->x, matrix[8]->y, r.tl.x, r.tl.y, r.tl.x, r.br.y);
		}
		if(matrix[7]){
			/*check 3-7*/
			return IntersectDetect::lineIntersec(matrix[3]->x, matrix[3]->y,
					matrix[7]->x, matrix[7]->y, r.tl.x, r.tl.y, r.tl.x, r.br.y);
		}
	}
	if(matrix[5]){
		if(matrix[6]){
			/*check 5-6*/
			return IntersectDetect::lineIntersec(matrix[5]->x, matrix[5]->y,
					matrix[6]->x, matrix[6]->y, r.br.x, r.tl.y, r.br.x, r.br.y);
		}
		if(matrix[7]){
			/*check 5-7*/
			return IntersectDetect::lineIntersec(matrix[5]->x, matrix[5]->y,
					matrix[7]->x, matrix[7]->y, r.br.x, r.tl.y, r.br.x, r.br.y);
		}
	}

	return false;

}

// Get the center point of the triangle (slow)
void Triangle::getCenterPoint(sm::Point &p) const
{
	// TODO: improve this to get a more accurate and efficient result
	ASSERT(v1);
	ASSERT(v2);
	ASSERT(v3);

	p.x = (v1->x + v2->x + v3->x) * 0.33333f;
	p.y = (v1->y + v2->y + v3->y) * 0.33333f;
}


void Triangle::getRandomPoint(sm::Point &r) const
{
	static const float INV_MAX_RND = 1.0f/RAND_MAX;
	float a = std::rand() * INV_MAX_RND;
	float b = std::rand() * INV_MAX_RND;
	if((a + b) > 1.0f){
		a = 1-a;
		b = 1-b;
	}
	float c = 1-a-b;
	r = (*v1) * a + (*v2) * b + (*v3) * c;
}
