/*
 * LevelPathfindLoader.h
 *
 *  Created on: 09/05/2012
 *      Author: agustin
 *
 */

#ifndef LEVELPATHFINDLOADER_H_
#define LEVELPATHFINDLOADER_H_

#include <vector>

#include "Loader.h"
#include "tinyxml.h"
#include "PathfindingManager.h"

class TiXmlElement;


class LevelPathfindLoader : public Loader {
public:
	LevelPathfindLoader();
	virtual ~LevelPathfindLoader();

	// Functoin used to load something
	virtual int load(TiXmlElement*, LoaderData *data);

	// Unload the information?
	virtual int unload();


private:
	PathfindingManager		*mPathfindingManager;

};

#endif /* LEVELPATHFINDLOADER_H_ */
