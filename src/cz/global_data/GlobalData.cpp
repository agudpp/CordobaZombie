/*
 * GlobalData.cpp
 *
 *  Created on: Sep 10, 2013
 *      Author: agustin
 */

#include "GlobalData.h"

namespace cz {

Ogre::SceneManager* GlobalData::sceneMngr = 0;
Ogre::Camera* GlobalData::camera = 0;

coll::CollisionHandler* GlobalData::collHandler = 0;
float GlobalData::lastTimeFrame = 0.f;

} /* namespace cz */
