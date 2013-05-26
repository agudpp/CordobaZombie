/*
 * GameUnit.cpp
 *
 *  Created on: 02/12/2012
 *      Author: agustin
 */

#include "GameUnitHelper.h"

#include <OgreSceneNode.h>
#include <OgreEntity.h>


#include "GlobalObjects.h"
#include "GameUnit.h"

namespace helper {

void
GameUnitHelper::destroyUnit(GameUnit *unit)
{
    if (unit == 0) return;

    Ogre::SceneNode *node = unit->getSceneNode();
    Ogre::Entity *ent = unit->getEntity();
    if (node != 0){
        node->detachAllObjects();
        GLOBAL_SCN_MNGR->destroySceneNode(node);
    }

    GLOBAL_SCN_MNGR->destroyEntity(ent);
    delete unit;
}


}
