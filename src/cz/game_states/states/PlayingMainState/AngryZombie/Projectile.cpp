/*
 * Projectile.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#include "Projectile.h"

#include <OgreSceneManager.h>

#include <debug/DebugUtil.h>

namespace demo_app {

cz::RagDollQueue<> Projectile::sRagdollQueue;
cz::BodyPartQueue Projectile::sBodyPartQueue;
physics::DynamicWorld* Projectile::sDynamicWorld = 0;

////////////////////////////////////////////////////////////////////////////////
Projectile::Projectile(Ogre::SceneNode* node, Ogre::Entity* ent)
{
    ASSERT(node);
    ASSERT(ent);
    static bool build = false;

    if (!build) {
        build = true;
        ASSERT(sDynamicWorld);
        sRagdollQueue.configure(&sDynamicWorld->bulletDynamicWorld(),
                                ent->getSkeleton(),
                                node);
    }

    // configure the body
    mBody.setRagDollQueue(&sRagdollQueue);
    mBody.setBodyPartQueue(&sBodyPartQueue);

    mBody.setEntity(ent);
    mBody.setSceneNode(node);
    mBody.setSkeleton(ent->getSkeleton());
    mBody.build();
}

////////////////////////////////////////////////////////////////////////////////
Projectile::~Projectile()
{
    Ogre::SceneNode* node = mBody.sceneNode();
    Ogre::Entity* ent = mBody.entity();

    // destroy the node and entity if we have
    if (node) {
        ASSERT(ent);
        Ogre::SceneManager* manager = node->getCreator();
        manager->destroyEntity(ent);
        manager->destroySceneNode(node);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
Projectile::reset(const Ogre::Vector3& pos)
{
    mBody.resetBody();
    ASSERT(mBody.sceneNode());
    mBody.sceneNode()->setPosition(pos);
    mBody.setRagdollEnable(true);
}

////////////////////////////////////////////////////////////////////////////////
void
Projectile::applyForce(const Ogre::Vector3& force)
{
    mBody.applyForce(force, cz::ZombieBody::BodyPart::BP_SPINE);
}

////////////////////////////////////////////////////////////////////////////////
void
Projectile::performCollision(CollisionInfo* collInfo)
{
    // do nothing for now
    debugBLUE("Something collide against a projectile!\n");
}

////////////////////////////////////////////////////////////////////////////////
bool
Projectile::update(float timeFrame)
{
    return mBody.update();
}

} /* namespace demo_app */
