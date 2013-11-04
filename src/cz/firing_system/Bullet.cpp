/*
 * Bullet.cpp
 *
 *  Created on: Sep 27, 2013
 *      Author: agustin
 */

#include "Bullet.h"

#include "BulletQueue.h"


namespace cz {

const float Bullet::MIN_BULLET_SIZE = 2.f;
const float Bullet::MAX_BULLET_SIZE = 5.f;


////////////////////////////////////////////////////////////////////////////////
void
Bullet::create(void)
{
    ASSERT(!mNode);
    ASSERT(!mEntity);

    Ogre::SceneManager* scnMngr = GlobalData::sceneMngr;
    ASSERT(scnMngr);

    mNode = scnMngr->createSceneNode();
    mEntity = scnMngr->createManualObject();

    // TODO: tenemos que usar un material aca predeterminado
    mEntity->begin("", Ogre::RenderOperation::OT_LINE_STRIP);

    Ogre::Vector3 p1(Ogre::Vector3::ZERO),p2(Ogre::Vector3::ZERO);
    const Ogre::Real size = Ogre::Math::RangeRandom(MIN_BULLET_SIZE, MAX_BULLET_SIZE);
    p1.z = size * 0.5f;
    p2.z = -size * 0.5f;

    mEntity->position(p1);  // start position
    mEntity->position(p2);  // end position

    mEntity->end();

    mNode->attachObject(mEntity);
}

////////////////////////////////////////////////////////////////////////////////
void
Bullet::configure(const Ogre::Vector3& pos, const Ogre::Vector3& dir)
{
    mPosition = pos;
    mNormalizedDir = dir.normalisedCopy();

    // update the direction and position
    ASSERT(mNode);
    mNode->setPosition(pos);
    mNode->lookAt(pos + dir, Ogre::Node::TS_WORLD);

    // we will show the bullet in the scene
    if (mNode->getParentSceneNode()) {
        return;
    }
    GlobalData::sceneMngr->getRootSceneNode()->addChild(mNode);
}


////////////////////////////////////////////////////////////////////////////////
void
Bullet::update(float time)
{
    ASSERT(mNode);
    // update the node position translating them
    mNode->translate(mNormalizedDir * (mVelocity * time));
    mPosition = mNode->getPosition();
}

// @brief This method will be called when we don't use this instance anymore
//        and we want to put it again in the queue.
//
void
Bullet::disable(void)
{
    ASSERT(mNode);
    if (!mNode->getParentSceneNode()) {
        return;
    }

    // remove from the scene and put the bullet into the queue again
    GlobalData::sceneMngr->getRootSceneNode()->removeChild(mNode);
    ASSERT(mQueue);
    mQueue->letAvailable(this);

}




}



