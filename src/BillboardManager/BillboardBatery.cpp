/*
 * BillboardBatery.cpp
 *
 *  Created on: 29/04/2012
 *      Author: agustin
 */

#include "GlobalObjects.h"
#include "BillboardBatery.h"


namespace billboard {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
BillboardBatery::BillboardBatery() :
mActualIndex(0),
mBillboardSet(0),
mNode(0)
{

}

BillboardBatery::~BillboardBatery()
{
	if(mBillboardSet->getParentSceneNode()){
		mBillboardSet->getParentSceneNode()->detachObject(mBillboardSet);
	}
	GLOBAL_SCN_MNGR->destroyBillboardSet(mBillboardSet);

	if(!mNode->getParentSceneNode()){
		GLOBAL_SCN_MNGR->getRootSceneNode()->addChild(mNode);
	}
	mNode->getParentSceneNode()->removeAndDestroyChild(mNode->getName());

}


////////////////////////////////////////////////////////////////////////////////
bool
BillboardBatery::createSet(Ogre::Real width, Ogre::Real height,
                                const Ogre::String &mn,
                                size_t count,
                                const std::vector<UVCoords> &coords)
{
	ASSERT(!mBillboardSet);

	mBillboardSet = GLOBAL_SCN_MNGR->createBillboardSet(count);

	// create the billboard
	//	result->createBillboard(Ogre::Vector3::ZERO, Ogre::ColourValue::Green);
	mBillboardSet->setDefaultDimensions(width, height);
	mBillboardSet->setBillboardType(Ogre::BBT_PERPENDICULAR_COMMON);
	mBillboardSet->setCommonDirection(Ogre::Vector3::UNIT_Y);
	mBillboardSet->setCommonUpVector(Ogre::Vector3::UNIT_Z);
	mBillboardSet->setMaterialName(mn);
	mBillboardSet->setVisible(true);

	// move all the billboards down
	for(size_t i = 0; i < count; ++i){
		Ogre::Billboard *b = mBillboardSet->createBillboard(0, -999, 0);
	}

	// create the scene node
	mNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

	mNode->attachObject(mBillboardSet);
	mActualIndex = 0;

	ASSERT(!coords.empty());
	mCoords = coords;

	return true;
}

////////////////////////////////////////////////////////////////////////////////
void
BillboardBatery::setVisible(bool v)
{
	mNode->setVisible(v);
}

////////////////////////////////////////////////////////////////////////////////
void
BillboardBatery::showBillboard(const Ogre::Vector3 &pos)
{
	ASSERT(mBillboardSet);

	// get the available billboard
	const size_t index = mActualIndex % mBillboardSet->getNumBillboards();
	++mActualIndex;

	// get the index in the coords
	const size_t coordsIndex = index % mCoords.size();

	Ogre::Billboard *b = mBillboardSet->getBillboard(index);
	ASSERT(b);

	// put the billboard in the corresponding position
	b->setPosition(pos);
	const UVCoords &coords = mCoords[coordsIndex];
	b->setTexcoordRect(coords.u0, coords.v0, coords.u1, coords.v1);

	// update the billboard dimension
	mBillboardSet->_updateBounds();
}


}
