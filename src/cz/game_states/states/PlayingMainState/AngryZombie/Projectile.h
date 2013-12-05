/*
 * Projectile.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include <OgreSceneNode.h>
#include <OgreEntity.h>

#include <zombie_unit/ZombieBody.h>
#include <zombie_unit/RagDollQueue.h>
#include <zombie_unit/BodyPartQueue.h>
#include <physics/DynamicWorld.h>

#include "PhysicObject.h"

namespace demo_app {

class Projectile : public PhysicObject
{
public:
    Projectile(Ogre::SceneNode* node, Ogre::Entity* ent);
    virtual
    ~Projectile();

    // @brief Set the dynamic world instance before anything
    //
    static void
    setDynamicWorld(physics::DynamicWorld* dw) {sDynamicWorld = dw;}

    // @brief Reset the projectile
    //
    void
    reset(const Ogre::Vector3& pos);

    // @brief Apply a force to the "torax"
    //
    void
    applyForce(const Ogre::Vector3& force);

    // @brief Method called whenever some other physics object hits this one
    //
    virtual void
    performCollision(CollisionInfo* collInfo);

    // @brief update method used to update the logic of the element if needed
    // @return true if we need to continue updating | false otherwise
    //
    virtual bool
    update(float timeFrame);

private:
    static physics::DynamicWorld* sDynamicWorld;
    static cz::RagDollQueue<> sRagdollQueue;
    static cz::BodyPartQueue sBodyPartQueue;

    cz::ZombieBody mBody;
};

} /* namespace demo_app */
#endif /* PROJECTILE_H_ */
