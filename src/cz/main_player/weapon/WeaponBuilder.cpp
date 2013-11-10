/*
 * WeaponBuilder.cpp
 *
 *  Created on: Oct 10, 2013
 *      Author: agustin
 */

#include "WeaponBuilder.h"

#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>

#include <debug/DebugUtil.h>
#include <global_data/GlobalData.h>

#include "Weapon.h"
#include "bullet_based/WeaponFAL.h"
#include "bullet_based/Weapon9MM.h"

namespace cz {

namespace WeaponBuilder {



Weapon*
buildWeapon(WeaponID wid)
{
    Weapon* result = 0;
    Ogre::Entity* entity = 0;

    debugWARNING("Values hardcoded, check issue #155 also\n");
    ASSERT(GlobalData::sceneMngr);

    switch (wid)
    {
    case WeaponID::WID_9MM:
        result = new Weapon9MM;
        entity = GlobalData::sceneMngr->createEntity("9mm.mesh");
        break;

    case WeaponID::WID_FAL:
        result = new WeaponFAL;
        entity = GlobalData::sceneMngr->createEntity("fal.mesh");
        break;

    default:
        debugERROR("Invalid weapon type: %d\n", wid);
        return 0;
    }

    ASSERT(result);
    ASSERT(entity);

    // create the SceneNode
    Ogre::SceneNode* node = GlobalData::sceneMngr->createSceneNode();
    node->attachObject(entity);
    result->setOgreStuff(entity);

    return result;
}

}

} /* namespace cz */
