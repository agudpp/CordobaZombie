/*
 * Test.cpp
 * 18 de junio del 2012
 * Raul Monti
 * Test module for TriangleCache module
 */


#include "Test.h"
#include "Graph.h"
#include "TriangleCache.h"
#include "Triangle.h"
#include "GNode.h"
#include "Vec2.h"
#include "DebugUtil.h"



int main(void){

	TriangleCache::TriangleCache TC(50,2);

	sm::Point p11(10,10);
	sm::Point p12(20,20);
	sm::Point p13(30,30);
	sm::Point p21(40,40);
	sm::Point p22(50,50);
	sm::Point p23(60,60);

	Triangle t1(&p11, &p12, &p13);
	Triangle t2(&p21, &p22, &p23);
	GNode gn1(&t1);
	GNode gn2(&t2);

	GNode gn3(&t2);
	GNode gn4(&t2);
	GNode gn5(&t1);
	GNode gn6(&t1);

	const GNode ** path1 = (const GNode **)calloc(3, sizeof(GNode *));
	path1[0] = &gn3;
	path1[1] = &gn4;
	path1[2] = 0;
	const GNode ** path = (const GNode **)calloc(3, sizeof(GNode *));
	path[0] = &gn1;
	path[1] = &gn2;
	path[2] = 0;
	const GNode ** path2 = (const GNode **)calloc(4, sizeof(GNode *));
	path2[0] = &gn5;
	path2[1] = &gn3;
	path2[2] = &gn6;
	path2[3] = 0;
	bool f = false;

	debugGREEN("TEST FIND WHEN THERE IS NOTHING\n");
	const GNode **r = TC.getPath( &gn1, &gn2, f);
	ASSERT(!r);
	debugRED("OK... \n");

	debugGREEN("TEST ADD AND SEARCH ONE ELEMENT\n");
	TC.addPath( &gn1, &gn2, path);
	const GNode **ret = TC.getPath( &gn1, &gn2, f);
	ASSERT(ret);
	ASSERT(f);
	ASSERT(&gn1 == ret[0]);
	ASSERT(&gn2 == ret[1]);
	debugRED("OK... \n");

	debugGREEN("TEST SEARCH THE SAME ELEMENT BUT IN REVERSE ORDER\n");
	ASSERT(TC.getPath( &gn2, &gn1, f));
	ASSERT(!f);
	debugRED("OK... \n");

	debugGREEN("TEST ADD 500000 MORE ELEMENTS gn2-gn1 and look for gn1-gn2 and gn2-gn1\n");
	for (int i = 0; i < 50000; i++){
		TC.addPath( &gn2, &gn1, path);
	}
	ASSERT(TC.getPath( &gn1, &gn2, f));
	ASSERT(TC.getPath( &gn2, &gn1, f));
	debugRED("OK... \n");

	debugGREEN("TEST ADD path, then path1, then path2, and then find path 1\n");
	TC.addPath( &gn1, &gn2, path);
	TC.addPath( &gn3, &gn4, path1);
	TC.addPath( &gn5, &gn6, path2);
	ASSERT(!TC.getPath( &gn1, &gn2, f));
	ASSERT(!TC.getPath( &gn2, &gn1, f));
	ASSERT(TC.getPath( &gn4, &gn3, f));
	ASSERT(TC.getPath( &gn3, &gn4, f));
	ASSERT(TC.getPath( &gn5, &gn6, f));
	ASSERT(TC.getPath( &gn6, &gn5, f));
	debugRED("OK... \n");

	debugGREEN("TEST CLEAR CACHE\n");
	TC.clear();
	ASSERT(!TC.getPath( &gn1, &gn2, f));
	ASSERT(!TC.getPath( &gn2, &gn1, f));
	ASSERT(!TC.getPath( &gn4, &gn3, f));
	ASSERT(!TC.getPath( &gn3, &gn4, f));
	ASSERT(!TC.getPath( &gn5, &gn6, f));
	ASSERT(!TC.getPath( &gn6, &gn5, f));
	debugRED("OK... \n");

	debugGREEN("TEST REFRESHING\n");
	TC.addPath( &gn2, &gn1, path);
	TC.addPath( &gn3, &gn4, path1);
	TC.addPath( &gn2, &gn1, path);
	TC.addPath( &gn5, &gn6, path2);
	ASSERT(!TC.getPath( &gn4, &gn3, f));
	ASSERT(TC.getPath( &gn1, &gn2, f));
	ASSERT(TC.getPath( &gn5, &gn6, f));
	debugRED("OK... \n");

	debugBLUE("END TESTS (ALL OK)\n");

	free(path);
	free(path1);
	free(path2);

	return 0;
}
