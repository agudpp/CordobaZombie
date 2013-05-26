/*
 * CollObjImporter.cpp
 *
 *  Created on: Apr 7, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#include <fstream>  // std::ifstream
#include <cstring>  // strncmp()

#include "CollObjImporter.h"
#include "DebugUtil.h"
#include "Triangle.h"	// core::Vertex


// According to CallObjExporter, newline character used is UNIX/MAC's '\n'
#define  char_eol  '\n'


namespace
{

/**
 ** @brief
 ** Create a bounding box from a list of 2D vertices
 **/
inline void
getBoundingBox(const std::vector<core::Vertex> &v, core::AABB &bb)
{
	ASSERT(v.size() >= 2);

	float minx, miny, maxx, maxy;
	minx = maxx = v[0].x;
	miny = maxy = v[0].y;

	for(int i = v.size()-1; i >= 0; --i){
		if(minx > v[i].x){minx = v[i].x;}
		if(maxx < v[i].x){maxx = v[i].x;}
		if(miny > v[i].y){miny = v[i].y;}
		if(maxy < v[i].y){maxy = v[i].y;}
	}
	bb.tl.x = minx;
	bb.tl.y = maxy;
	bb.br.x = maxx;
	bb.br.y = miny;
}



/**
 ** @brief
 ** Read next 8 floats from stream "f",
 ** and create box-type collision object accordingly.
 **
 ** @return
 ** Collision object created (dynamically) from extracted info.
 ** NULL pointer on error
 **/
inline CollisionObject *
readBoxLine(std::ifstream &f)
{
	std::vector<core::Vertex> v(4);
	CollisionObject *co(0);

	ASSERT(f.good() && f.is_open() && !f.eof());

	for (int i=0 ; i<4 ; i++) {
		f >> v[i].x;	// This vertex x coord
		f >> v[i].y;	// This vertex y coord
	}
	if (!f.good()) {
		v.clear();
		return co;
	}

	co = new CollisionObject;
	co->numEdges = 4;
	getBoundingBox(v, co->bb);

	// TODO: fill 'co' Box2D field
	debugRED("TODO: fill 'co' Box2D field\n");

	v.clear();
	return co;
}


/**
 ** @brief
 ** Read next 8 floats from stream "f",
 ** and create AABB-type collision object accordingly.
 **
 ** @return
 ** Collision object created (dynamically) from extracted info.
 ** NULL pointer on error
 **/
inline CollisionObject *
readAABBLine(std::ifstream &f)
{
	std::vector<core::Vertex> v(4);
	CollisionObject *co(0);

	ASSERT(f.good() && f.is_open() && !f.eof());

	for (int i=0 ; i<4 ; i++) {
		f >> v[i].x;	// This vertex x coord
		f >> v[i].y;	// This vertex y coord
	}
	if (!f.good()) {
		v.clear();
		return co;
	}

	co = new CollisionObject;
	co->numEdges = 4;
	getBoundingBox(v, co->bb);

	// TODO: fill 'co' with more relevant info
	debugRED("TODO: fill 'co' with more relevant info\n");

	v.clear();
	return co;
}


/**
 ** @brief
 ** Read next 4 floats from stream "f",
 ** and create edge-type collision object accordingly.
 **
 ** @return
 ** Collision object created (dynamically) from extracted info.
 ** NULL pointer on error
 **/
inline CollisionObject *
readEdgeLine(std::ifstream &f)
{
	float v0x(0.0f),	// Vertex #0 x coord
		  v0y(0.0f),	// Vertex #0 y coord
		  v1x(0.0f),	// Vertex #1 x coord
		  v1y(0.0f);	// Vertex #1 y coord
	CollisionObject *co(0);

	ASSERT(f.good() && f.is_open() && !f.eof());

	f >> v0x;
	if (!f.good() || f.eof()) return co;
	f >> v0y;
	if (!f.good() || f.eof()) return co;
	f >> v1x;
	if (!f.good() || f.eof()) return co;
	f >> v1y;
	if (!f.good()) return co;

	co = new CollisionObject;
	co->numEdges = 1;

	// TODO: fill 'co' with more relevant info
	debugRED("TODO: fill 'co' with more relevant info\n");

	return co;
}


/**
 ** @brief
 ** Read next 3 vertices info from stream "f",
 ** and create circle-type collision object accordingly.
 **
 ** @return
 ** Collision object created (dynamically) from extracted info.
 ** NULL pointer on error
 **/
inline CollisionObject *
readCircleLine(std::ifstream &f)
{
	float cx(0.0f),	// center x coord
		  cy(0.0f),	// center y coord
		  r(0.0f);	// radius
	CollisionObject *co(0);

	ASSERT(f.good() && f.is_open() && !f.eof());

	f >> cx;
	if (!f.good() || f.eof()) return co;
	f >> cy;
	if (!f.good() || f.eof()) return co;
	f >> r;
	if (!f.good()) return co;

	co = new CollisionObject;
	co->numEdges = 0;

	// TODO: fill 'co' with more relevant info
	debugRED("TODO: fill 'co' with more relevant info\n");

	return co;
}


/**
 ** @brief
 ** Read next N? vertices info from stream "f",
 ** and create poly-type collision object accordingly.
 **
 ** @return
 ** Collision object created (dynamically) from extracted info.
 ** NULL pointer on error
 **/
inline CollisionObject *
readPolyLine(std::ifstream &f)
{
	CollisionObject *co(0);

	ASSERT(f.good() && f.is_open() && !f.eof());

	// TODO
	debugRED("TODO\n");

	return co;
}


/**
 ** @brief
 ** Read collision object info from next line of stream "f",
 ** and create collision object accordingly.
 **
 ** @remarks
 ** Get pointer of stream "f" is moved 1 line forward.
 ** Expected text line format is specified in function
 ** CollObjExporter.h:entityExtractor()
 **
 ** @return
 ** Collision object created (dynamically) from extracted info
 ** NULL pointer on error
 **/
CollisionObject *
readCollObjLine(std::ifstream &f)
{
	char type[128];

	ASSERT(f.good() && f.is_open() && !f.eof());
	f >> type;

	if (0 == strncmp(type, "box", 3)) {
		return readBoxLine(f);
	} else if (0 == strncmp(type, "aabb", 4)) {
		return readAABBLine(f);
	} else if (0 == strncmp(type, "edge", 4)) {
		return readEdgeLine(f);
	} else if (0 == strncmp(type, "circle", 6)) {
		return readCircleLine(f);
	} else if (0 == strncmp(type, "poly", 4)) {
		return readPolyLine(f);
	} else
		debugERROR("Unknown collision object type: \"%s\"", type);

	return 0;
}


/**
 ** @brief
 ** Read stream "f" for collision information from *only one* .scene file,
 ** and fill up "v" vector accordingly.
 **
 ** @remarks
 ** Correctly parsed objects are allocated in dynamic mem.
 ** Expected text format is specified in function
 ** CollObjExporter.h:sceneExtractor()
 ** Get pointer of stream "f" is modified.
 **
 ** @return
 ** true if all parsed information was correct
 ** false otherwise
 **/
bool
readCollObjScene(std::ifstream &f, std::vector<CollisionObject*> &v)
{
	char snName[1024];
	int numObjects(0);

	ASSERT(f.is_open());

	// Get SceneNode name (discarded)
	f >> snName;
	// Get number of collision objects to parse
	f >> numObjects;
	if (numObjects < 1) {
		debugERROR("Could not get # of collision objects to parse.\n");
		return false;
	}

	// Fill up vector "v" with parsed objects
	for (int i=0 ; i < numObjects ; i++) {
		CollisionObject *co = readCollObjLine(f);
		if (co)
			v.push_back(co);
		else
			return false;  // Stop on first error
	}

	return true;
}

}



////////////////////////////////////////////////////////////////////////////////
bool
CollObjImporter::importFromFile(Ogre::String path)
{
	bool parseCorrect(true);
	unsigned int numScenes(0);
	std::ifstream f(path.c_str(), std::ifstream::in);

	if (!f.is_open()) {
		debugERROR("Could not open file \"%s\" for reading.\n", path.c_str());
		return false;
	}

	// Get # of .scene files to parse from text file
	f >> numScenes;
	if (f.tellg() == 0 || numScenes < 1) {
		debugERROR("Could not retrieve # of .scene extracts to parse.\n");
		f.close();
		return false;
	}

	for (int i=0 ; i < numScenes && parseCorrect; i++) {
		// Stop parsing on first error
		parseCorrect = readCollObjScene(f, mParsedCollObjs);
	}

	f.close();
	return parseCorrect;
}



