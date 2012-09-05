/*
 * BillboardBatery.cpp
 *
 *  Created on: 29/04/2012
 *      Author: agustin
 */

#include "GlobalObjects.h"
#include "BillboardBatery.h"

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


/**
 * Load a Billboardset using a radius create the billboards and
 * a material
 * @param r		The radius to be used
 * @param mn	Material Name to be used
 * @param size	The number of billboard to allocate
 *
 * @return 		True on success false otherwise
 */
bool BillboardBatery::createSet(Ogre::Real r, const Ogre::String &mn, int size)
{
	ASSERT(!mBillboardSet);

	mBillboardSet = GLOBAL_SCN_MNGR->createBillboardSet(size);

	// create the billboard
	//	result->createBillboard(Ogre::Vector3::ZERO, Ogre::ColourValue::Green);
	mBillboardSet->setDefaultDimensions(r, r);
	mBillboardSet->setBillboardType(Ogre::BBT_PERPENDICULAR_COMMON);
	mBillboardSet->setCommonDirection(Ogre::Vector3::UNIT_Y);
	mBillboardSet->setCommonUpVector(Ogre::Vector3::UNIT_Z);
	mBillboardSet->setMaterialName(mn);
	mBillboardSet->setVisible(true);

	// move all the billboards down
	for(int i = 0; i < size; ++i){
		Ogre::Billboard *b = mBillboardSet->createBillboard(-999,-999,-999);
//		b->setPosition(-9999,-9999,-9999);
	}
	debug("Num billboards: %d\n", mBillboardSet->getNumBillboards());

	// create the scene node
	mNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

	mNode->attachObject(mBillboardSet);
	mActualIndex = 0;

	return true;
}


/**
 * Hide all the billboards
 */
void BillboardBatery::setVisible(bool v)
{
	mNode->setVisible(v);
}

/**
 * Show a new billboard in some position with some normal vector
 */
void BillboardBatery::showBillboard(const Ogre::Vector3 &pos)
{
	ASSERT(mBillboardSet);

	// get the available billboard
	int index = mActualIndex % mBillboardSet->getNumBillboards();
	mActualIndex++;

	Ogre::Billboard *b = mBillboardSet->getBillboard(index);
	ASSERT(b);

	// put the billboard in the corresponding position
	b->setPosition(pos);

	// update the billboard dimension
	mBillboardSet->_updateBounds();
}


