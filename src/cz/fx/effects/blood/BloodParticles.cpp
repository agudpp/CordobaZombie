/*
 * BloodParticles.cpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agustin
 */

#include "BloodParticles.h"

#include <OgreSceneManager.h>
#include <OgreParticleEmitter.h>

#include <global_data/GlobalData.h>
#include <debug/DebugUtil.h>

namespace cz {

const float BloodParticles::ACTIVE_TIME_SECS = 4.5f;


////////////////////////////////////////////////////////////////////////////////
BloodParticles::BloodParticles() :
    mNode(0)
,   mParticleSystem(0)
{

}

////////////////////////////////////////////////////////////////////////////////
BloodParticles::~BloodParticles()
{
    if (mNode) {
        if (mNode->getParentSceneNode()) {
            mNode->getParentSceneNode()->removeChild(mNode);
        }
        mNode->detachAllObjects();
        GlobalData::sceneMngr->destroySceneNode(mNode);
    }
    if (mParticleSystem) {
        GlobalData::sceneMngr->destroyParticleSystem(mParticleSystem);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
BloodParticles::construct(void)
{
    ASSERT(!mNode);
    ASSERT(!mParticleSystem);
    static char charCounter = '0';

    Ogre::SceneManager* scnMngr = GlobalData::sceneMngr;
    Ogre::String name = "BPS";
    name.push_back(charCounter++);
    mParticleSystem = scnMngr->createParticleSystem(name, BLOOD_PART_NAME);
    mNode = scnMngr->createSceneNode();
    mNode->attachObject(mParticleSystem);
}

////////////////////////////////////////////////////////////////////////////////
void
BloodParticles::configure(const Ogre::Vector3& position,
                          const Ogre::Vector3& direction,
                          float intensity)
{
    ASSERT(mNode && "You Should construct the effect first");
    ASSERT(mParticleSystem && "You Should construct the effect first");
    mNode->setPosition(position);
    mNode->setDirection(direction);

    // set the number of particles here... using the intensity if needed
    debugWARNING("We are not setting the intensity here, check if we want or not\n");
}

////////////////////////////////////////////////////////////////////////////////
void
BloodParticles::beforeStart(void)
{
    ASSERT(mNode);

    // we should attach the scene node to the scene manager
    GlobalData::sceneMngr->getRootSceneNode()->addChild(mNode);
    mParticleSystem->setEmitting(true);
    for (unsigned int i = 0, size = mParticleSystem->getNumEmitters(); i < size; ++i) {
        mParticleSystem->getEmitter(i)->setEnabled(true);
    }
    mCurrentTime = 0.f;
}

////////////////////////////////////////////////////////////////////////////////
bool
BloodParticles::update(float timeFrame)
{
    mCurrentTime += timeFrame;
    return mCurrentTime < ACTIVE_TIME_SECS;
}

////////////////////////////////////////////////////////////////////////////////
void
BloodParticles::afterFinish(void)
{
    ASSERT(mNode);
    // remove the node from the scene manager and turn of the particle system
    GlobalData::sceneMngr->getRootSceneNode()->removeChild(mNode);
    mParticleSystem->setEmitting(false);
    for (unsigned int i = 0, size = mParticleSystem->getNumEmitters(); i < size; ++i) {
        mParticleSystem->getEmitter(i)->setEnabled(false);
    }
    // we will put this particle again in the queue
    letThisFree();
}


} /* namespace effect */
