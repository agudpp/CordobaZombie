/*
 * CollObjExporter.cpp
 *
 *  Created on: Mar 23, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#include <string>
#include <cstring>	// strnlen()
#include <sstream>	// std::stringstream
#include <fts.h>	// Filesystem handler (FTS, FTSENT, fts_open()...)

#include "CommonMath.h"
#include "Triangle.h"
#include "CollObjExporter.h"
#include "GlobalObjects.h"
#include "DotSceneLoader.h"


namespace {
struct TmpEdge {
	const sm::Vertex *v1;
	const sm::Vertex *v2;
};
}


////////////////////////////////////////////////////////////////////////////////
std::string
CollObjExporter::dirExtractor(void)
{
	const char *CWD[] = {".", 0};  // Current Working Directory
    FTS *ftsp(0);
    FTSENT *p(0);
    int fts_options = FTS_PHYSICAL | FTS_NOCHDIR;
	size_t fnamelen(0);
	std::stringstream output;

    // Try to open CWD
    ftsp = fts_open((char* const*)CWD, fts_options, NULL);
    if (!ftsp) {
    	debugERROR("Couldn't open current working directory.\n");
        return "";
    }

    // Check whether there are some files
    p = fts_children(ftsp, 0);
    if (!p) {
    	debugWARNING("Current working directory has no files to process.\n");
        return "";
    }
    // Open CWD for reading
    p = fts_read(ftsp);

    // Call sceneExtractor() for each .scene file in CWD
	int numfiles(0);
    while (true) {
    	// Read each element inside CWD
    	p = fts_read(ftsp);
        if (!p) {
        	// No more files to search
            break;
        } else if (p->fts_info == FTS_D) {
        	// Directory, no recursive search
            fts_set(ftsp, p, FTS_SKIP);

        } else if (p->fts_info == FTS_F) {
        	fnamelen = strnlen(p->fts_path, 100);
        	if (fnamelen > 6  // consider ".scene" extension
        		&& 0 == strncmp(&p->fts_path[fnamelen-6], ".scene", 6)) {
        	    Ogre::DotSceneLoader dsl;
        		// .scene file found! build SceneNode from it
        		debugBLUE("Extracting collision info from file \"%s\"\n", p->fts_path);
        		dsl.parseDotScene(p->fts_path, "Popular", GLOBAL_SCN_MNGR, 0);
        		output << sceneExtractor(dsl.mAttachNode);
        		numfiles++;
        	}
        }
    }
    debugBLUE("# of .scene files processed: %d\n", numfiles);

	return output.str();
}


////////////////////////////////////////////////////////////////////////////////
std::string
CollObjExporter::sceneExtractor(Ogre::SceneNode* scene)
{
	std::stringstream output;
	ASSERT(scene);
	output << scene->getName() << "\n";
	output << scene->numChildren() << "\n";

	// we go through each scene child...
	Ogre::SceneNode::ChildNodeIterator scene_it = scene->getChildIterator();
	while (scene_it.hasMoreElements()){
		//... and process the attached entity
		Ogre::Node* node = scene_it.getNext();
		ASSERT(node);
		Ogre::SceneNode::ObjectIterator node_it =
				((Ogre::SceneNode *)node)->getAttachedObjectIterator();
		if (node_it.hasMoreElements()){
			Ogre::Entity* ent = (Ogre::Entity*)node_it.getNext();
			Ogre::String type = userDefExtractor(ent->getName());
			// createFromMesh returns type and all the vertex
			output << entityExtractor(node, ent, type);
		} else {
			debugERROR("Scene error: bad form. ABORTING EXTRACTION\n");
			output << "\n...scene error: bad form. ABORTING EXTRACTION\n";
			return output.str().c_str();
		}
	}
	return output.str();
}


////////////////////////////////////////////////////////////////////////////////
std::string
CollObjExporter::entityExtractor(Ogre::Node* node,
								 Ogre::Entity *ent,
								 const Ogre::String &userDef)
{
	ASSERT(node);
	ASSERT(ent);

	Ogre::Vector3 mod_pos = node->getPosition();
	PolyStructsContainer<sm::Vertex *> cont;
	PolyStructsContainer<Triangle *> triangles;
	std::stringstream data;

	if(!Common::Util::getTrianglesFromMesh(cont, triangles, ent->getMesh())){
		debugERROR("Error getting triangles from mesh\n");
		return "";
	}
	
	// Identify specified type
	if (userDef == "poly"){
		data << createPolyShape(mod_pos, cont, triangles);
	} else if (userDef == "circle") {
		data << createCircleShape(mod_pos, cont, triangles);
	} else if (userDef == "edge") {
		data << createEdgeShape(mod_pos, cont, triangles);
	} else if (userDef == "box") {
		data << createBoxShape(mod_pos, cont, triangles);
	} else if (userDef == "aabb") {
		data << createAABBShape(mod_pos, cont, triangles);
	} else {
		debugERROR("Unknown shape type: \"%s\"\n", userDef.c_str());
		return "";
	}

	cont.removeAndFreeAll();
	triangles.removeAndFreeAll();

	return data.str();
}


////////////////////////////////////////////////////////////////////////////////
std::string
CollObjExporter::createPolyShape(const Ogre::Vector3 &mod_pos,
								 PolyStructsContainer<sm::Vertex *> &cont,
								 PolyStructsContainer<Triangle *> &triangles)
{
	std::vector<Triangle *> &tri = triangles.getObjs();
	std::vector<sm::Vertex *> &vert = cont.getObjs();
	std::stringstream data;
	const sm::Vertex *v = 0;
	int untouched = 1;  // '1' to enter loop

	// Find central vertex, which touches all triangles
	for (int j=0 ; j < vert.size() && untouched ; j++) {
		untouched = triangles.getSize();
		v = vert[j];  // Candidate for central vertex
		for (int i=0 ; i < tri.size() ; i++) {
			if (tri[i]->v1 == v || tri[i]->v2 == v || tri[i]->v3 == v) {
				if (--untouched == 0) break;  // Central vertex found
			}
		}
	}

	if (untouched > 0) {
		debugERROR("Wrong mesh, some triangle does not share the central vertex\n");
		return "";
	}

	// 'v' is the central vertex
	// Remove all edges connected to it, leaving only external edges.
	std::vector<TmpEdge> edges;
	TmpEdge edge;
	for (int i=0 ; i < tri.size() ; i++) {
		if (v == tri[i]->v1) {
			edge.v1 = tri[i]->v3;
			edge.v2 = tri[i]->v2;
		} else if (v == tri[i]->v2) {
			edge.v1 = tri[i]->v1;
			edge.v2 = tri[i]->v3;
		} else if (v == tri[i]->v3) {
			edge.v1 = tri[i]->v1;
			edge.v2 = tri[i]->v2;
		} else {
			debugERROR("Wrong mesh, some triangle does not share "
						"the central vertex\n");
			return "";
		}
		// save the edge
		edges.push_back(edge);
	}

	// Save the vertices of the external edges,
	// clockwise and without repetitions.
	std::vector<const sm::Vertex *> vSet;
	v = edges.front().v1;
	vSet.push_back(v);  // Save the first vertex first
	for (size_t numVert = 0 ; numVert < edges.size() ; numVert++) {
		// On each iteration we add a new vertex
		for (int i=0 ; i < edges.size() ; i++) {
			if (edges[i].v1 == vSet.back()) {
				vSet.push_back(edges[i].v2);
				break;
			} else if (edges[i].v2 == vSet.back()){
				vSet.push_back(edges[i].v1);
				break;
			}
		}
	}

	// Print name and number of vertices
	data << "poly  " << vSet.size();
	// Print all vertices
	for (int i=0 ; i < vSet.size() ; i++) {
		data << "  " << vSet[i]->x + mod_pos.x
			 << "  " << vSet[i]->y + mod_pos.z;
	}
	data << "\n";
	
	return data.str();
}


////////////////////////////////////////////////////////////////////////////////
std::string
CollObjExporter::createCircleShape(const Ogre::Vector3 &mod_pos,
								   PolyStructsContainer<sm::Vertex *> &cont,
								   PolyStructsContainer<Triangle *> &triangles)
{
	std::vector<sm::Vertex *> &vert = cont.getObjs();
	std::stringstream data;

	debugBLUE("# of vertex: %zu\n", vert.size());
	ASSERT(vert.size() == 2);

	data << "circle  "
		 << mod_pos.x << "  " << mod_pos.z << "  "			// center
		 << (vert[0]->squaredDistance(*vert[1])) << "\n";	// radius

	return data.str();
}


////////////////////////////////////////////////////////////////////////////////
std::string
CollObjExporter::createEdgeShape(const Ogre::Vector3 &mod_pos,
								 PolyStructsContainer<sm::Vertex *> &cont,
								 PolyStructsContainer<Triangle *> &triangles)
{
	std::vector<sm::Vertex *> &vert = cont.getObjs();
	std::stringstream data;

	ASSERT(vert.size() == 2);

	// Return vertices updated with object position
	data << "edge  "
		 << (vert[0]->x + mod_pos.x) << "  "
		 << (vert[0]->y + mod_pos.y) << "  "
		 << (vert[1]->x + mod_pos.x) << "  "
		 << (vert[1]->y + mod_pos.y) << "\n";

	return data.str();
}


////////////////////////////////////////////////////////////////////////////////
std::string
CollObjExporter::createBoxShape(const Ogre::Vector3 &mod_pos,
								PolyStructsContainer<sm::Vertex *> &cont,
								PolyStructsContainer<Triangle *> &triangles)
{
	std::vector<Triangle *> &tri = triangles.getObjs();
	std::stringstream data;

	if (tri.size() != 2) {
		debugERROR("Wrong number of triangles for BoxShape.\n"
					"Should be 2, %zu received.\n", tri.size());
		return "";
	}

	// Triangles shared-vertices recognition
	std::vector<const sm::Vertex *> shared;
	std::vector<const sm::Vertex *> notShared;

	const sm::Vertex *v = 0;
	Triangle *t_a = tri[0];
	Triangle *t_b = tri[1];

	// This method is slow, but has major error checking.
	if (t_a->v1==t_b->v1 || t_a->v1==t_b->v2 || t_a->v1==t_b->v3) {
		shared.push_back(t_a->v1);
	} else {
		notShared.push_back(t_a->v1);
	}
	if (t_a->v2==t_b->v1 || t_a->v2==t_b->v2 || t_a->v2==t_b->v3) {
		shared.push_back(t_a->v2);
	} else {
		notShared.push_back(t_a->v2);
	}
	if (t_a->v3==t_b->v1 || t_a->v3==t_b->v2 || t_a->v3==t_b->v3) {
		shared.push_back(t_a->v3);
	} else {
		notShared.push_back(t_a->v3);
	}
	if (find(shared.begin(), shared.end(), t_b->v1) == shared.end()) {
		notShared.push_back(t_b->v1);
	}
	if (find(shared.begin(), shared.end(), t_b->v2) == shared.end()) {
		notShared.push_back(t_b->v2);
	}
	if (find(shared.begin(), shared.end(), t_b->v3) == shared.end()) {
		notShared.push_back(t_b->v3);
	}

	// Check number of shared vertices is correct
	if (shared.size() != 2 || notShared.size() != 2) {
		debugERROR("Wrong shared vertices for triangles of BoxShape.\n"
					"Should have 2 shared and 2 not shared vertices,"
					"%zu and %zu (resp.) received.\n",
					shared.size(), notShared.size());
		return "";
	}

	// Push the vertices, clockwise or counterclockwise
	data << "box  "
		 << (shared[0]->x + mod_pos.x)    << "  "
		 << (shared[0]->y + mod_pos.z)    << "  "
		 << (notShared[0]->x + mod_pos.x) << "  "
		 << (notShared[0]->y + mod_pos.z) << "  "
		 << (shared[1]->x + mod_pos.x)    << "  "
		 << (shared[1]->y + mod_pos.z)    << "  "
		 << (notShared[1]->x + mod_pos.x) << "  "
		 << (notShared[1]->y + mod_pos.z) << "\n";
	
	return data.str();
}


////////////////////////////////////////////////////////////////////////////////
std::string
CollObjExporter::createAABBShape(const Ogre::Vector3 &mod_pos,
								 PolyStructsContainer<sm::Vertex *> &cont,
								 PolyStructsContainer<Triangle *> &triangles)
{
	sm::AABB shape;
	std::stringstream data;
	std::vector<sm::Vertex *> vert = cont.getObjs();

	if (vert.size() != 4) {
		debugERROR("Wrong number of vertices for AABBShape.\n"
					"Should be 4, %zu received.\n", vert.size());
		return "";
	}
	
	// Correct the position
	for(int i= 0; i< 4; i++){
		vert[i]->x += mod_pos.x;
		vert[i]->y += mod_pos.z;
	}
	

	getBoundingBox(vert, shape);
	// 0 ****** 1
	// *        *
	// *  AABB  *
	// *        *
	// 3 ****** 2
	data << "aabb  "
		 << shape.tl.x << "  " << shape.br.y << "  "	// 0
		 << shape.tl.x << "  " << shape.tl.y << "  "	// 1
		 << shape.br.x << "  " << shape.tl.y << "  "	// 2
		 << shape.br.x << "  " << shape.br.y << "\n";	// 3

	return data.str();
}
