/*
 * BillboardStack.h
 *
 *  Created on: Oct 3, 2013
 *      Author: agustin
 */

#ifndef BILLBOARDSTACK_H_
#define BILLBOARDSTACK_H_

#include <OgreBillboard.h>
#include <OgreBillboardSet.h>
#include <OgreSceneNode.h>

#include <global_data/GlobalData.h>
#include <debug/DebugUtil.h>
#include <types/basics.h>

#include "UVCoordsDefs.h"

namespace cz {


// @brief This class will hold all the Billboards with the similar dimensions
//        and materials (but not the UV's).
//

class BillboardStack
{
public:

    // Helper class used to configure the stack
    //
    struct Args {
        unsigned int numBillboards;
        float width;
        float height;
        Ogre::Vector3 upVector;
        Ogre::Vector3 direction;
        Ogre::String materialName;
    };

public:
    inline BillboardStack();
    inline ~BillboardStack();

    // @brief Configure the BillboardStack
    // @param args      The arguments used to configure the instance
    //
    inline void
    configure(const Args& args);

    // @brief Get a billboard element
    // @param uvCoords  [optional] Coordinates to be used for this billboard
    // @return billboard * or 0 on error
    //
    inline Ogre::Billboard*
    getBillboard(void);
    inline Ogre::Billboard*
    getBillboard(const UVCoord& uvCoords);
    inline Ogre::Billboard*
    getBillboard(const UVCoord& uvCoords,
                 const Ogre::Vector3& position);

    // @brief Set the billboardStack (and all of its billboards) visible / invisible
    // @param visible       Set it visible / invisible
    //
    inline void
    setVisible(bool visible);
    inline bool
    isVisible(void) const;

private:
    Ogre::BillboardSet* mSet;
    Ogre::SceneNode* mNode;
    unsigned int mCurrentIndex;
};





////////////////////////////////////////////////////////////////////////////////
// Inline methods
//


inline BillboardStack::BillboardStack() :
    mSet(0)
,   mNode(0)
,   mCurrentIndex(0)
{
    mNode = GlobalData::sceneMngr->createSceneNode();
    GlobalData::sceneMngr->getRootSceneNode()->addChild(mNode);
}

inline BillboardStack::~BillboardStack()
{
    GlobalData::sceneMngr->destroySceneNode(mNode);
}

inline void
BillboardStack::configure(const Args& args)
{
    ASSERT(mNode);
    ASSERT(mSet == 0);

    // create the set
    mSet = GlobalData::sceneMngr->createBillboardSet(args.numBillboards);
    mSet->setDefaultDimensions(args.width, args.height);
    mSet->setBillboardType(Ogre::BBT_PERPENDICULAR_COMMON);
    mSet->setCommonDirection(args.direction);
    mSet->setCommonUpVector(args.upVector);
    mSet->setMaterialName(args.materialName);
    mSet->setVisible(true);

    // move all the billboards down
    for(core::size_t i = 0; i < args.numBillboards; ++i){
        Ogre::Billboard *b = mSet->createBillboard(0, -999, 0);
    }

    // create the scene node
    mNode->attachObject(mSet);
    mCurrentIndex = 0;
}

inline Ogre::Billboard*
BillboardStack::getBillboard(void)
{
    ASSERT(mSet);

    // get the available billboard
    const unsigned int index = mCurrentIndex % mSet->getNumBillboards();
    ++mCurrentIndex;

    // get the index in the coords
    Ogre::Billboard *b = mSet->getBillboard(index);
    ASSERT(b);
    return b;
}

inline Ogre::Billboard*
BillboardStack::getBillboard(const UVCoord& uvCoords)
{
    Ogre::Billboard* b = getBillboard();
    b->setTexcoordRect(uvCoords.u0, uvCoords.v0, uvCoords.u1, uvCoords.v1);
    return b;
}

inline Ogre::Billboard*
BillboardStack::getBillboard(const UVCoord& uvCoords, const Ogre::Vector3& position)
{
    Ogre::Billboard* b = getBillboard(uvCoords);

    // put the billboard in the corresponding position
    b->setPosition(position);

    // update the billboard dimension
    mSet->_updateBounds();
    return b;
}


inline void
BillboardStack::setVisible(bool visible)
{
    mNode->setVisible(visible);
}
inline bool
BillboardStack::isVisible(void) const
{
    return mSet->isVisible();
}

} /* namespace cz */
#endif /* BILLBOARDSTACK_H_ */
