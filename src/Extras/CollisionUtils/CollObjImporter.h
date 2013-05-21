/*
 * CollObjImporter.h
 *
 *  Get info for the Collision System from local plain text format
 *  (see CollObjExporter.h) and generate corresponding Collision Objects.
 *
 *  Created on: Apr 7, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef COLLOBJIMPORTER_H_
#define COLLOBJIMPORTER_H_

#include <vector>
#include <OgreString.h>

#include "CollisionObject.h"


class CollObjImporter
{
public:
	CollObjImporter  () {}
	~CollObjImporter () {}  // NOTE: no memory is released on destruction

	/**
	 ** @brief
	 ** Read plain text file in "path", parse collision information from it
	 ** and generate collision objects accordingly.
	 **
	 ** @remarks
	 ** Correctly parsed objects are allocated internally in dynamic mem.
	 ** "path" must point to a readable plain text file.
	 ** Expected text format is specified in function
	 ** CollObjExporter.h:dirExtractor()
	 **
	 ** @return
	 ** true if parsing and constructions successfull
	 ** false otherwise (+ error msg output to stderr)
	 **/
	bool
	importFromFile(Ogre::String path);

	/**
	 ** @brief
	 ** Return vector with Collision Objects (successfully) parsed so far.
	 **
	 ** @remarks
	 ** Vector's objects were allocated dynamically, dispose of with delete()
	 ** Consider aliasing before deleting.
	 **/
	inline std::vector<CollisionObject*>&
	getCollisionObjects(void);

private:
	std::vector<CollisionObject*>	mParsedCollObjs;
};


/******************************************************************************/
/****************************     INLINES     *********************************/

////////////////////////////////////////////////////////////////////////////////
std::vector<CollisionObject*>&
CollObjImporter::getCollisionObjects(void)
{
	return mParsedCollObjs;
}


#endif /* COLLOBJIMPORTER_H_ */
