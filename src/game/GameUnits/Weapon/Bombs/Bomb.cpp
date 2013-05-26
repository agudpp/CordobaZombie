/*
 * Bomb.cpp
 *
 *  Created on: 30/04/2012
 *      Author: agustin
 */

#include "Bomb.h"
#include "DebugUtil.h"
#include "GameUnitDefines.h"
#include "GameObject.h"

c_p::CollisionManager *Bomb::mCollMngr = 0;
c_p::CollisionResult Bomb::mCollResult;

/**
 * Check if there are some object in the middle between us and the target
 */
bool
Bomb::checkObjectBetween(const core::Vector2 &p1, const CollisionObject *t)
{
    ASSERT(t);
    const core::Vector2 &p2 = t->getPosition();
    mCollMngr->getCollisionObjects(p1, p2, c_p::BOMB_NOT_AFFECT_MASK,
        mCollResult);

    // get the closest
    std::vector<core::Point> intersecPoints;
    float sqrDist = p1.squaredDistance(p2);
    float dist;
    for (int i = mCollResult.size() - 1; i >= 0; --i) {
        if (mCollResult[i] == t) {
            continue;
        }
        // else.. is other object, get the intersection points
        mCollResult[i]->getIntPoints(p1, p2, intersecPoints);
        if (intersecPoints.empty())
            continue;

        // get the closer point of all this ones
        for (int j = intersecPoints.size() - 1; j >= 0; --j) {
            dist = intersecPoints[j].squaredDistance(p1);
            if (dist < sqrDist) {
                // there are other object first than the target
                return true;
            }
        }
    }

    return false;
}

/**
 * Perform explosion
 * @param	sqrRadius	The squared radius that affect the bomb
 */
void
Bomb::performExplosion(float sqrRadius, float power)
{
    // perform an aabb query
    core::AABB query;
    query.setSize(2.0f * sqrRadius, 2.0f * sqrRadius);

    // set the actual position
    ASSERT(mNode);
    const Ogre::Vector3 &pos = mNode->getPosition();
    query.setPosition(core::Vector2(pos.x, pos.z));
    static c_p::CollisionResult objects;
    mCollMngr->getCollisionObjects(query, c_p::BOMB_AFFECTABLE_MASK, objects);

    debugRED("Tenemos que poner el power real aca, estamos"
    " usando directamente una fuerza lineal, tenemos que disminuir"
    " el poder a medida que nos alejamos del centro de explosion\n");

    // iterate over all the objects affected
    const CollisionObject *co;
    Hit_t hit;
    hit.shooter = 0;
    for (int i = objects.size() - 1; i >= 0; --i) {
        co = objects[i];
        if (checkObjectBetween(query.pos, co)) {
            // not affect that object..
            continue;
        }

        // else we affect the object with the corresponding power
        GameObject *go = static_cast<GameObject *>(co->userDefined);
        ASSERT(go);
        hit.power = power;
        hit.hitDir = query.pos - co->getPosition();
        go->beenHit(hit);
    }

}

Bomb::Bomb(int t) :
    mEntity(0), mNode(0), mType(t)
{
    ASSERT(mCollMngr);
    // create a SceneNode
    mNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
}

Bomb::~Bomb()
{
    if (!mNode->getParentSceneNode()) {
        GLOBAL_SCN_MNGR->getRootSceneNode()->addChild(mNode);
    }
    GLOBAL_SCN_MNGR->getRootSceneNode()->removeAndDestroyChild(
        mNode->getName());
    GLOBAL_SCN_MNGR->destroyEntity(mEntity);
}

/**
 * Set the Collision Manager
 */
void
Bomb::setCollisionManager(c_p::CollisionManager *cm)
{
    ASSERT(cm);
    mCollMngr = cm;
}

void
Bomb::setEntity(Ogre::Entity *ent)
{
    ASSERT(ent);
    ASSERT(!mEntity);

    mEntity = ent;
    mNode->attachObject(ent);
}
