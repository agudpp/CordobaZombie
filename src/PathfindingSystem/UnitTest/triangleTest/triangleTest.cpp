/*
 * triangleTest.cpp
 *
 *  Created on: 20/01/2012
 *      Author: agustin
 *
 */



#include <iostream>
#include "Triangle.h"
#include "DebugUtil.h"
#include "PolyStructsContainer.h"

typedef PolyStructsContainer<Vertex *>	VertexContainer;

static Triangle *createTriangle(float x1, float y1,
		float x2, float y2, float x3, float y3)
{
	Vertex *v1 = new Vertex(x1,y1);
	Vertex *v2 = new Vertex(x2, y2);
	Vertex *v3 = new Vertex(x3,y3);
	VertexContainer::getInstance().addObj(v1);
	VertexContainer::getInstance().addObj(v2);
	VertexContainer::getInstance().addObj(v3);

	return new Triangle(v1,v2,v3);
}

int main(void)
{
	Triangle *t1 = createTriangle(0,0,10,50,15,20);
	Point pInside1(0,0);
	Point pInside2(10,30);
	Point pInside3(15,20);
	Point pOutside1(0,1);
	Point pOutside2(10,51);
	Point pOutside3(0,10);

	std::cout << "Checking 1\n";
	ASSERT(t1->containsPoint(pInside1));
	ASSERT(t1->containsPoint(pInside2));
	ASSERT(t1->containsPoint(pInside3));
	ASSERT(!t1->containsPoint(pOutside1));
	ASSERT(!t1->containsPoint(pOutside2));
	ASSERT(!t1->containsPoint(pOutside3));

	// check box intersection
	AlignedBox box;

	box.tl.x = -1; box.tl.y = 10;
	box.br.x = -0.1; box.br.y = 0;
	ASSERT(!t1->intersectAlignedBox(box));

	box.tl.x = 16; box.tl.y = 50;
	box.br.x = 50; box.br.y = 0;
	ASSERT(!t1->intersectAlignedBox(box));

	box.tl.x = 0; box.tl.y = 10;
	box.br.x = 5; box.br.y = 0;
	ASSERT(t1->intersectAlignedBox(box));

	box.tl.x = -10; box.tl.y = 100;
	box.br.x = 50; box.br.y = 0;
	ASSERT(t1->intersectAlignedBox(box));

	box.tl.x = -10; box.tl.y = 500;
	box.br.x = 550; box.br.y = -10;
	ASSERT(t1->intersectAlignedBox(box));

	box.tl.x = 5; box.tl.y = 25;
	box.br.x = 6; box.br.y = 24;
	ASSERT(t1->intersectAlignedBox(box));



	std::cout << "ALL IS OK\n";

	delete t1;
	VertexContainer::getInstance().removeAndFreeAll();
	return 0;
}
