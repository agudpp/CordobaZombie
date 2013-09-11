/*
 * ZombieUnit.cpp
 *
 *  Created on: Sep 9, 2013
 *      Author: agustin
 */

#include "ZombieUnit.h"

#include <cmath>

#include "CZMasksDefines.h"

// helper stuff
//
namespace {

//      ZA_RUN1,
//      ZA_RUN2,
//      ZA_HIT_SHOULDER_R,
//      ZA_HIT_SHOULDER_L,
//      ZA_FALL,
//      ZA_DRAG,
//      ZA_DRAG_DIE,
//      ZA_HEAD_DIE,
//      ZA_NORMAL_DIE1,
//      ZA_NORMAL_DIE2,
static const char* ZA_ANIMS[] = {
    "corre1",
    "corre2",
    /*"golpe_hombrod",
    "golpe_hombroi",
    "cae",
    "arrastra",
    "muere_arrastra",
    "muere_cabeza",
    "muere_normal1",
    "muere_normal2",*/
    0
};

}

namespace cz {

ZombieTTable ZombieUnit::sTTable;

////////////////////////////////////////////////////////////////////////////////
ZombieUnit::ZombieUnit() :
    WorldObject()
,   mFSM(sTTable)
,   mVelocity(15.f)
{
    mFSM.setRef(this);
}

////////////////////////////////////////////////////////////////////////////////
ZombieUnit::~ZombieUnit()
{
    debugWARNING("we should remove the scenenode and entity here?\n");
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieUnit::configure(Ogre::SceneNode* node, Ogre::Entity* entity)
{
    ASSERT(node && "Invalid node");
    ASSERT(entity && "Invalid entity");

    // check if the entity is already attached
    if (entity->getParentNode()) {
        ASSERT(entity->getParentNode() == node);
    } else {
        // attach it
        node->attachObject(entity);
    }

    setOgreStuff(node, entity);

    // set the pointers for the collision object and the OgreAny
    mCollObj->setUserDef(this);
    Ogre::UserObjectBindings& binding = entity->getUserObjectBindings();
    binding.setUserAny(Ogre::Any(this));

    // configure the animation table
    mAnimTable.loadAnims(ZA_ANIMS, entity);

    // configure the masks for raytracing and collisions
    entity->setQueryFlags(CZRayMask::CZRM_ZOMBIE);
    setCollMask(~0); // we want to collide with everything

    // configure the size of the coll object
    const Ogre::AxisAlignedBox& ogreBB = entity->getBoundingBox();
    const Ogre::Vector3 ogreBBSize = ogreBB.getHalfSize();
    const core::AABB bb(ogreBBSize.y, -ogreBBSize.x, -ogreBBSize.y, ogreBBSize.x);
    setCollisionInformation(0, bb);
    setCollisionType(false); // wow, this is rare XD, probably using an enum will be
                             // more easy to understand
    enableCollisions(false); // disable the collisions for now

    // set the radius of the pathHandler
    mPathHandler.setRadius(std::sqrt(sqrRadius()));

    // TODO: load sounds here
    debugRED("TODO: \n\t* load sounds here... "
        "\n\t* physics information? "
        "\n\t* Create the skeletal information for firing system and for the ragdolls?\n");

}

////////////////////////////////////////////////////////////////////////////////
void
ZombieUnit::reset(void)
{
    debugERROR("TODO\n");
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieUnit::born(void)
{
    debugERROR("TODO\n");

    // enable the collision
    enableCollisions(true);

    // reset the fsm
    mFSM.reset();
    mFSM.start();
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieUnit::dead(void)
{
    debugERROR("TODO\n");
}

} /* namespace cz */
