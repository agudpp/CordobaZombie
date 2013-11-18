/*
 * StaticWorldObject.cpp
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#include "StaticWorldObject.h"

#include <debug/DebugUtil.h>
#include <physics/DynamicWorld.h>
#include <CZMasksDefines.h>

namespace cz {

////////////////////////////////////////////////////////////////////////////////
StaticWorldObject::StaticWorldObject()
{
    // we will set the current bullet user pointer to point this instance
    mPhysicsRep.collObject.setUserPointer(this);
}

////////////////////////////////////////////////////////////////////////////////
StaticWorldObject::~StaticWorldObject()
{
    // delete the associated shape if we have one
    if (mPhysicsRep.shape()) {
        ASSERT(sDynamicWorld);
        sDynamicWorld->removeObject(mPhysicsRep);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
StaticWorldObject::checkImpact(HitInfo& hitInfo) const
{
    // we always want to process the impact
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
StaticWorldObject::processImpactInfo(const HitInfo& hitInfo)
{
    // get the effect from the queue and reproduce if we have an available effect
    debugERROR("TODO: reproduce the effect here, issue #255\n");

    // get the sound to be reproduce and reproduce it if we can
    debugERROR("TODO: reproduce the sound here: issue #256\n");
}

////////////////////////////////////////////////////////////////////////////////
void
StaticWorldObject::configurePosition(const Ogre::Vector3& pos)
{
    // configure the position of the 2dColl and entity 3d
    set3DPosition(pos);
    // configure the physics representation
    mPhysicsRep.setPosition(pos);
}

////////////////////////////////////////////////////////////////////////////////
void
StaticWorldObject::configureRotation(const Ogre::Quaternion& rot)
{
    // configure the 2D coll rotation
    setOrientation(rot);
    // set the physics orientation
    mPhysicsRep.setRotation(rot);
}

////////////////////////////////////////////////////////////////////////////////
void
StaticWorldObject::configure(Ogre::SceneNode* node, Ogre::Entity* entity)
{
    ASSERT(node && "Invalid node");
    ASSERT(entity && "Invalid entity");

    // check if the entity is already attached
    if (entity->getParentNode()) {
//        ASSERT(entity->getParentNode() == node);
    } else {
        // attach it
        node->attachObject(entity);
    }

    setOgreStuff(node, entity);

    // set the pointers for the collision object and the OgreAny
    mCollObj->setUserDef(this);
    Ogre::UserObjectBindings& binding = entity->getUserObjectBindings();
    binding.setUserAny(Ogre::Any(this));

    // configure the masks for raytracing and collisions
    entity->setQueryFlags(CZRayMask::CZRM_WORLD_STATIC);
    setCollMask(~0); // we want to collide with everything

    // configure the size of the coll object
    setCollisionType(true); // wow, this is rare XD, probably using an enum will be
                            // more easy to understand
    enableCollisions(true); // disable the collisions for now

    ASSERT(sDynamicWorld);
    ASSERT(mPhysicsRep.shape() != 0 && "We need to set a shape before calling this");

    // configure the position and rotation of the object
    configurePosition(node->getPosition());
    configureRotation(node->getOrientation());

    debugWARNING("Check this flags! fucking bullet!\n");
    sDynamicWorld->addObject(mPhysicsRep,
                             CZRayMask::CZRM_ZOMBIE_BODY_PART |
                             CZRayMask::CZRM_RAGDOLL |
                             CZRayMask::CZRM_WORLD_DYNAMIC |
                             CZRayMask::CZRM_RAYCASTABLE,              // collision mask
                             CZRayMask::CZRM_ZOMBIE_BODY_PART |
                             CZRayMask::CZRM_RAGDOLL |
                             CZRayMask::CZRM_WORLD_DYNAMIC |
                             CZRayMask::CZRM_RAYCASTABLE);       // collide against
}

} /* namespace cz */
