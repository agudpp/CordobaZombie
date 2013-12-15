/*
 * AlphaOverlayEffect.cpp
 *
 *  Created on: Nov 28, 2013
 *      Author: agustin
 */

#include "AlphaOverlayEffect.h"

#include <OgreMaterial.h>
#include <OgrePass.h>
#include <OgreTechnique.h>

#include <debug/DebugUtil.h>

namespace ui {

AlphaOverlayEffect::AlphaOverlayEffect() :
    mTime(0)
,   mAccumTime(0)
,   mTexture(0)
{
}

AlphaOverlayEffect::~AlphaOverlayEffect()
{
}

////////////////////////////////////////////////////////////////////////////////
void
AlphaOverlayEffect::beforeStart(void)
{
    ASSERT(overlayElement());

    // configure everything
    Ogre::MaterialPtr mat = overlayElement()->getMaterial();
    ASSERT(!mat.isNull());
    mTexture = mat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
    ASSERT(mTexture);

    // configure the effect
    switch(mType)
    {
    case FADE_OUT:
        mAccumTime = mTime;
        break;

    case FADE_IN:
        mAccumTime = 0;
        break;

    default:
        debugERROR("Invalid Alpha transition type\n");
        ASSERT(false);
        break;
    }

    // init the state of the texture
    mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
                            Ogre::LBS_TEXTURE, mAccumTime / mTime);
}

////////////////////////////////////////////////////////////////////////////////
bool
AlphaOverlayEffect::update(float timeFrame)
{
    switch(mType)
    {
    case FADE_OUT:
        // Slowly dissappear: reduce alpha value
        mAccumTime -= timeFrame;
        mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
                        Ogre::LBS_TEXTURE, mAccumTime / mTime);
        if(mAccumTime <= 0){
            // Effect finished
            return false;
        }
        break;

    case FADE_IN:
        // Slowly materialize: increase alpha value
        mAccumTime += timeFrame;
        mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
                        Ogre::LBS_TEXTURE, mAccumTime / mTime);
        if(mAccumTime >= mTime){
            // Effect finished
            return false;
        }
        break;

    default:
        debugERROR("Invalid Alpha transition type\n");
        ASSERT(false);
        break;
    }

    // we need to continue
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
AlphaOverlayEffect::afterFinish(void)
{
    if (mTexture) {
        // Remove fading and set internal textue to NULL
        mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
                                Ogre::LBS_TEXTURE, 1.0);
        mTexture = 0;
    }
}


////////////////////////////////////////////////////////////////////////////////
bool
AlphaOverlayEffect::complement()
{
    if (mType == FADE_IN) {
        mType = FADE_OUT;
    } else {
        mType = FADE_IN;
    }

    return true;
}

} /* namespace ui */
