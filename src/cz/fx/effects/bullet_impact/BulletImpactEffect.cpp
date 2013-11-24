/*
 * BulletImpactEffect.cpp
 *
 *  Created on: Nov 20, 2013
 *      Author: agustin
 */

#include "BulletImpactEffect.h"

#include <OgreSceneManager.h>
#include <OgreParticleEmitter.h>

#include <global_data/GlobalData.h>
#include <sound/SoundEnums.h>

namespace cz {

const float BulletImpactEffect::ACTIVE_TIME_SECS = 1.;

////////////////////////////////////////////////////////////////////////////////
BulletImpactEffect::BulletImpactEffect() :
    mNode(0)
,   mParticleSystem(0)
,	mCurrentTime(-1.0f)
{
	/* Default constructor suffices */
}

////////////////////////////////////////////////////////////////////////////////
BulletImpactEffect::~BulletImpactEffect() { destroy(); }

////////////////////////////////////////////////////////////////////////////////
bool
BulletImpactEffect::construct(const Ogre::String& particleName,
								  const Ogre::String* soundName)
{
    destroy();  // Erase any previous information

    static char charCounter = '0';
    Ogre::SceneManager* scnMngr = GlobalData::sceneMngr;
    Ogre::String name = "BIE";
    name.push_back(charCounter++);
    mParticleSystem = scnMngr->createParticleSystem(name, particleName);
    mNode = scnMngr->createSceneNode();
    mNode->attachObject(mParticleSystem);

    if (!soundName) {
    	debugERROR("NULL sound name given.\n");
    	return false;
    }
    mm::SSerror err = mSound.setSound(*soundName);
    if (err != mm::SSerror::SS_NO_ERROR) {
    	debugERROR("Failed to load effect sound \"%s\": %s\n",
    				soundName->c_str(), SSenumStr(err));
    	return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
BulletImpactEffect::destroy(void)
{
    if (mNode) {
        if (mNode->getParentSceneNode()) {
            mNode->getParentSceneNode()->removeChild(mNode);
        }
        mNode->detachAllObjects();
        GlobalData::sceneMngr->destroySceneNode(mNode);
        mNode = 0;
    }
    if (mParticleSystem) {
        GlobalData::sceneMngr->destroyParticleSystem(mParticleSystem);
        mParticleSystem = 0;
    }
	if (mSound.active())
		mSound.stop();
	mSound.setSound("");
	mSound.setSceneNode(0);
}

////////////////////////////////////////////////////////////////////////////////
void
BulletImpactEffect::setParams(const Ogre::Vector3& position,
                              const Ogre::Vector3& surfaceNormal,
                              const Ogre::Vector3& bulletDir,
                              float power)
{
    ASSERT(mNode && "You Should construct the effect first");
    ASSERT(mParticleSystem && "You Should construct the effect first");

    // set the position to the same position than the impact
    mNode->setPosition(position);

    // we will set the surface normal for now but we can do something like:
    // set the direction as the average of the surfaceNormal and bulletDirection
    mNode->lookAt(position + surfaceNormal, Ogre::Node::TS_WORLD);

    debugWARNING("We are not using the power here, probably this can be used to"
        " set the number of particles and the velocity of them, size or whatever\n");

    mSound.setSceneNode(mNode);
}

////////////////////////////////////////////////////////////////////////////////
void
BulletImpactEffect::beforeStart(void)
{
    ASSERT(mNode && "You Should construct the effect first");
    ASSERT(mParticleSystem && "You Should construct the effect first");

    // we should attach the scene node to the scene manager
    GlobalData::sceneMngr->getRootSceneNode()->addChild(mNode);
    mParticleSystem->setEmitting(true);
    for (unsigned int i = 0, size = mParticleSystem->getNumEmitters(); i < size; ++i) {
        mParticleSystem->getEmitter(i)->setEnabled(true);
    }
    mCurrentTime = 0.f;

    mm::SSerror err = mSound.play();
    if (err != mm::SSerror::SS_NO_ERROR)
    	debugERROR("Couldn't start effect sound: %s\n", SSenumStr(err));
}

////////////////////////////////////////////////////////////////////////////////
bool
BulletImpactEffect::update(float timeFrame)
{
    mCurrentTime += timeFrame;
    return mCurrentTime < ACTIVE_TIME_SECS;
}


////////////////////////////////////////////////////////////////////////////////
void
BulletImpactEffect::afterFinish(void)
{
    ASSERT(mNode && "You Should construct the effect first");
    ASSERT(mParticleSystem && "You Should construct the effect first");

    // remove the node from the scene manager and turn of the particle system
    GlobalData::sceneMngr->getRootSceneNode()->removeChild(mNode);
    mParticleSystem->setEmitting(false);
    for (unsigned int i = 0, size = mParticleSystem->getNumEmitters(); i < size; ++i) {
        mParticleSystem->getEmitter(i)->setEnabled(false);
    }
    // we will put this particle again in the queue
    letThisFree();

    // Don't touch the sound, it may last longer that the effect.
    // The SoundHandler takes care of the cleanup once the sound finishes.
}

} /* namespace cz */
