/*
 * CollObjExporter.cpp
 *
 *  Created on: Mar 23, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#include "CommonMath.h"
#include "Triangle.h"
#include "CollObjExporter.h"

/**
 ** TODO: revisar el código entero y reescribirlo
 **       La idea es que esto genere un archivo .txt con la info geométrica
 **       de colisiones de los entities del juego.
 **       Después, en tiempo de ejecución leemos ese .txt y con él generamos
 **       dinámicamente los CollisionObjects
 **/

struct TmpEdge {
	const sm::Vertex *v1;
	const sm::Vertex *v2;
};

void CollObjExporter::getBoundingBox(std::vector<sm::Vertex *> &v, sm::AABB &bb)
{
	ASSERT(v.size() >= 2);

	float minx, miny, maxx, maxy;
	minx = maxx = v[0]->x;
	miny = maxy = v[0]->y;

	for(int i = v.size()-1; i >= 0; --i){
		if(minx > v[i]->x){minx = v[i]->x;}
		if(maxx < v[i]->x){maxx = v[i]->x;}
		if(miny > v[i]->y){miny = v[i]->y;}
		if(maxy < v[i]->y){maxy = v[i]->y;}
	}
	bb.tl.x = minx;
	bb.tl.y = maxy;
	bb.br.x = maxx;
	bb.br.y = miny;
}

void CollObjExporter::getBoundingBox(std::vector<const sm::Vertex *> &v, sm::AABB &bb)
{
	ASSERT(v.size() >= 2);

	float minx, miny, maxx, maxy;
	minx = maxx = v[0]->x;
	miny = maxy = v[0]->y;

	for(int i = v.size()-1; i >= 0; --i){
		if(minx > v[i]->x){minx = v[i]->x;}
		if(maxx < v[i]->x){maxx = v[i]->x;}
		if(miny > v[i]->y){miny = v[i]->y;}
		if(maxy < v[i]->y){maxy = v[i]->y;}
	}
	bb.tl.x = minx;
	bb.tl.y = maxy;
	bb.br.x = maxx;
	bb.br.y = miny;
}


std::string CollObjExporter::sceneExtractor(Ogre::SceneNode* scene){
		std::stringstream output;
		// FIX ME output << nombre_del_scene;
		output << scene->getName() << "\n";
		output << scene->numChildren() << "\n";

		// we go through each scene child...
		Ogre::Node::ChildNodeIterator scene_it = scene->getChildIterator();
		while (scene_it.hasMoreElements()){
			//... and process the attached entity
			Ogre::Node* node = scene_it.getNext();
			Ogre::SceneNode::ObjectIterator node_it = ((Ogre::SceneNode *)node)->getAttachedObjectIterator();
			if (node_it.hasMoreElements()){
				Ogre::Entity* ent = (Ogre::Entity*)node_it.getNext();
				Ogre::String type = userDefExtractor(ent->getName());
				// createFromMesh returns type and all the vertex
				output << CollObjExporter::createFromMesh(node, ent, type);
			}else{
				return "scene error: bad form\n";
			}
		}
	return output.str();
}

/* Get mesh type from the name
 */
Ogre::String CollObjExporter::userDefExtractor(const Ogre::String &source){
			/* format:	<name="xxx_box">    ||
			 *			<name="xxx_aabb">   ||
			 *			<name="xxx_edge">   ||
			 *			<name="xxx_circle"> ||
			 *			<name="xxx_poly">
			 */
			return Ogre::StringUtil::getsplit(source,"_",2)[1];
}

/* Returns type, number of vertex and coords of the ent.
 */
std::string CollObjExporter::createFromMesh(Ogre::Node* node, Ogre::Entity *ent,
		const Ogre::String &userDef)
{
	ASSERT(ent);
	Ogre::Vector3 mod_pos = node->getPosition();
	PolyStructsContainer<sm::Vertex *> cont;
	PolyStructsContainer<Triangle *> triangles;
	std::stringstream data;

	if(!Common::Util::getTrianglesFromMesh(cont, triangles, ent->getMesh())){
		debug("Error getting triangles from mesh\n");
		return "";
	}
	
	// Identify specified type
	if(userDef == "poly"){
		data << createPolyShape(mod_pos, cont, triangles);
	} else if(userDef == "circle"){
		data << createCircleShape(mod_pos, cont, triangles);
	} else if(userDef == "edge"){
		data << createEdgeShape(mod_pos, cont, triangles);
	} else if(userDef == "box"){
		data << createBoxShape(mod_pos, cont, triangles);
	} else if(userDef == "aabb"){
		data << createAABBShape(mod_pos, cont, triangles);
	} else {
		debug("unknown type %s\n", userDef.c_str());
		ASSERT(false);
	}

	cont.removeAndFreeAll();
	triangles.removeAndFreeAll();

	return data.str();
}


/*
 * Poly shape extractor (counterclockwise vertex)
 */
std::string CollObjExporter::createPolyShape(const Ogre::Vector3 &mod_pos, PolyStructsContainer<sm::Vertex *> &cont,
		PolyStructsContainer<Triangle *> &triangles)
{
	// get the center vertex
	std::vector<Triangle *> &tri = triangles.getObjs();
	std::vector<sm::Vertex *> &vert = cont.getObjs();
	sm::Vertex *v = 0;
	bool found = false;
	std::stringstream data;

	debug("Creating poly with %zd triangles and %zd vertex\n",
			triangles.getSize(), vert.size());

	for(int j = vert.size()-1; j >= 0 && !found; --j){
		int middleCount = triangles.getSize();
		v = vert[j];
		for(int i = tri.size() - 1; i >= 0; --i){
			if(tri[i]->v1 == v || tri[i]->v2 == v || tri[i]->v3 == v){
				middleCount--;
				if(middleCount <= 0){
					found = true;
					break;
				}
			}
		}
	}

	if(!found){
		debug("The mesh is not right, we have a triangle that not share "
				"the middle vertex\n");
		ASSERT(false);
		return "";
	}

	debug("\n");

	// else we have the middle vertex that is v, we have now to remove all the
	// edges that are connected with this edge
	std::vector<TmpEdge> edges;
	TmpEdge edge;
	for(int i = tri.size()-1; i >= 0; --i){
		if(v == tri[i]->v1){edge.v1 = tri[i]->v3; edge.v2 = tri[i]->v2;}
		else if(v == tri[i]->v2){edge.v1 = tri[i]->v1; edge.v2 = tri[i]->v3;}
		else if(v == tri[i]->v3){edge.v1 = tri[i]->v1; edge.v2 = tri[i]->v2;}
		else {ASSERT(false);}

		// save the edge
		edges.push_back(edge);
	}
	debug("edges calculated: %zd\n", edges.size());

	// save the vector of vertices without repetition
	// FUCKING UGLY SLOW!
	//TODO check this part -> erase the duplicates
	std::vector<const sm::Vertex *> vSet;
	vSet.push_back(edges[edges.size()-1].v1);
	for(int i = edges.size()-1; i >= 0; --i){
		for(int j = 0; j < edges.size(); ++j){
			if(edges[i].v1 == vSet.back()) {
				vSet.push_back(edges[i].v2);
				break;
			} else if(edges[i].v2 == vSet.back()){
				vSet.push_back(edges[i].v1);
				break;
			} else {
				continue;
			}
		}
	}

	debug("Real number of vertex %zd\n", vSet.size());
	int arraySize = vSet.size();
	
	data << "poly\t";
	data << arraySize << "\t";

	// print every vertex
	for(int i= 0; i< arraySize; i++){
		data << "\t" << vSet[i]->x + mod_pos.x << " " << vSet[i]->y + mod_pos.z;
	}
	data << "\n";
	
	return data.str();
}

/*
 * Edge shape extractor
 */
std::string CollObjExporter::createEdgeShape(const Ogre::Vector3 &mod_pos, PolyStructsContainer<sm::Vertex *> &cont,
		PolyStructsContainer<Triangle *> &triangles)
{
	std::vector<Triangle *> &tri = triangles.getObjs();
	std::vector<sm::Vertex *> &vert = cont.getObjs();
	std::stringstream data;
	
	ASSERT(vert.size() == 2);
	
	for(int i= 0; i< 2; i++){
		vert[i]->x += mod_pos.x;
		vert[i]->y += mod_pos.z;
	}

	data << "edge\t";
	data << vert[0]->x << " " << vert[0]->y << "\t";
	data << vert[1]->x << " " << vert[1]->y << "\n";

	return data.str();
}


/*
 * Circle shape extractor
 */
std::string CollObjExporter::createCircleShape(const Ogre::Vector3 &mod_pos, PolyStructsContainer<sm::Vertex *> &cont,
		PolyStructsContainer<Triangle *> &triangles)
{
	std::vector<Triangle *> &tri = triangles.getObjs();
	std::vector<sm::Vertex *> &vert = cont.getObjs();
	std::stringstream data;

	ASSERT(vert.size() == 2);

	// get the radius
	float radius = vert[0]->squaredDistance(*vert[1]);
	data << "circle\t";
	data << mod_pos.x << " " << mod_pos.z << "\t";
	data << radius << "\n";

	return data.str();
}


/*
 * Box shape extractor
 */
std::string CollObjExporter::createBoxShape(const Ogre::Vector3 &mod_pos, PolyStructsContainer<sm::Vertex *> &cont,
		PolyStructsContainer<Triangle *> &triangles)
{
	std::vector<Triangle *> &tri = triangles.getObjs();
	std::vector<sm::Vertex *> &vert = cont.getObjs();
	std::stringstream data;

	ASSERT(vert.size() == 4);

	std::vector<const sm::Vertex *> shared;
	std::vector<const sm::Vertex *> notShared;

	const sm::Vertex *v = 0;
	Triangle *t_a = tri[0];
	Triangle *t_b = tri[1];
	
	if (t_a->v1==t_b->v1 || t_a->v1==t_b->v2 || t_a->v1==t_b->v3) {shared.push_back(t_a->v1);}
	else {notShared.push_back(t_a->v1);}

	if (t_a->v2==t_b->v1 || t_a->v2==t_b->v2 || t_a->v2==t_b->v3) {shared.push_back(t_a->v2);}
	else {notShared.push_back(t_a->v2);}

	if (t_a->v3==t_b->v1 || t_a->v3==t_b->v2 || t_a->v3==t_b->v3) {shared.push_back(t_a->v3);}
	else {notShared.push_back(t_a->v3);}

	if (find (shared.begin(), shared.end(), t_b->v1)==shared.end()){notShared.push_back(t_b->v1);}
	if (find (shared.begin(), shared.end(), t_b->v2)==shared.end()){notShared.push_back(t_b->v2);}
	if (find (shared.begin(), shared.end(), t_b->v3)==shared.end()){notShared.push_back(t_b->v3);}

	ASSERT(notShared.size() == 2);
	ASSERT(shared.size() == 2);

	// create the vector
	std::vector<const sm::Vertex *> vertexs;
	
	vertexs.push_back(shared[0]);
	vertexs.push_back(notShared[0]);
	vertexs.push_back(shared[1]);
	vertexs.push_back(notShared[1]);
	
	data << "box\n";
	for(int i= 0; i< 4; i++){
		data << vertexs[i]->x + mod_pos.x << " " << vertexs[i]->y + mod_pos.z << "\n";
	}
	
	return data.str();
}

/*
 * AABB shape extractor
 */
std::string CollObjExporter::createAABBShape(const Ogre::Vector3 &mod_pos, PolyStructsContainer<sm::Vertex *> &cont,
		PolyStructsContainer<Triangle *> &triangles)
{
	std::stringstream data;
	std::vector<Triangle *> &tri = triangles.getObjs();
	std::vector<sm::Vertex *> &vert = cont.getObjs();

	ASSERT(vert.size() == 4);

	sm::AABB shape;
	
	//correct the position
	for(int i= 0; i< 4; i++){
		vert[i]->x += mod_pos.x;
		vert[i]->y += mod_pos.z;
	}
	
	getBoundingBox(vert, shape);
	// 0 ******** 1
	//	 * AABB *
	// 3 ******** 2
	data << "aabb\t";
	data << shape.tl.x << " " << shape.br.y << "\t";  // 0
	data << shape.tl.x << " " << shape.tl.y << "\t";  // 1
	data << shape.br.x << " " << shape.tl.y << "\t";  // 2
	data << shape.br.x << " " << shape.br.y << "\n";  // 3

	return data.str();
}
