/*
 * LevelManager.cpp
 *
 *  Created on: 26/02/2012
 *      Author: agustin
 *
 */

#ifdef DEBUG
#include "TriNavMeshBuilder.h"
#include "Util.h"
#include <OgreManualObject.h>
#include "GlobalObjects.h"
#endif

#include "LevelManager.h"


////////////////////////////////////////////////////////////////////////////////
LevelManager::LevelManager()
{
	mCollMngr = new CollisionManager;
	mPathfindingMngr = &(PATHFINDER_INSTANCE);
	Common::GlobalObjects::levelMngr = this;
}

////////////////////////////////////////////////////////////////////////////////
LevelManager::~LevelManager()
{
	clear();
	delete mCollMngr;
	Common::GlobalObjects::levelMngr = 0;
}


////////////////////////////////////////////////////////////////////////////////
void LevelManager::clear(void)
{
	mCollMngr->removeAllObjects();
	mPathfindingMngr->unloadActualLevel();

}


#ifdef DEBUG
////////////////////////////////////////////////////////////////////////////////
void LevelManager::load(const Ogre::Entity *levelMesh,
		cScalar ssX, cScalar ssY, int cnX, int cnY)
{

	/* UNDEFINED - DO NOT USE */

	debugERROR("WTF!... no es porque estaba esto asi... no se ni quien lo puso"
			" pero dejo de funcionar todo xD\n");

	/* UNDEFINED - DO NOT USE */

	ASSERT(levelMesh);
	ASSERT(mCollMngr);
	ASSERT(mPathfindingMngr);

	mCollMngr->build(ssX, ssY, cnX, cnY);
	const bool b = Common::Util::getTrianglesFromMesh(mVertexs,
	                                                  mTriangles,
	                                                  levelMesh->getMesh());
	ASSERT(b);
	const int r = mPathfindingMngr->loadLevel(mTriangles.getObjs(), mVertexs.getObjs());
	ASSERT(r == 0);

	// raycastmanager
	mRaycastManager.setCollisionManager(mCollMngr);
	// tODO: estamos seteando la altura del nivel en la posicion siguiente
	mRaycastManager.build(5);
	debug("RaycastManager level height: %f\n", 5.0f);


}

void LevelManager::drawTriangle(const Triangle *t)
{
	Ogre::ManualObject* manual = GLOBAL_SCN_MNGR->createManualObject();
	manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);

	Ogre::Real height = 5.0f;
	manual->position(t->v1->x, height, t->v1->y);  // start position
	manual->position(t->v2->x, height, t->v2->y);  // start position
	manual->position(t->v3->x, height, t->v3->y);  // start position
	manual->position(t->v1->x, height, t->v1->y);  // start position

	manual->end();
	Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	node->attachObject(manual);

}

////////////////////////////////////////////////////////////////////////////////
void LevelManager::showTriangles(void)
{
	for(int i = mTriangles.getObjs().size()-1; i>=0; --i){
		drawTriangle(mTriangles.getObjs()[i]);
	}
	// check if the pathfinding manager has the triangles
	const std::vector<Triangle *> triangles = mPathfindingMngr->triangles();
	for(size_t i = 0, size = triangles.size(); i < size; ++i){
	    drawTriangle(triangles[i]);
	}
}
#endif
