/*
 * BillboardManager.cpp
 *
 *  Created on: 20/03/2012
 *      Author: agustin
 *
 */

#include "GlobalObjects.h"
#include "GUIHelper.h"

#include "BillboardManager.h"

BillboardManager::BillboardManager() :
mBillboardSet(0),
mNode(0),
mAtlasSize(-1)
{
	// TODO Auto-generated constructor stub

}

BillboardManager::~BillboardManager()
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
bool BillboardManager::createSet(Ogre::Real r, const Ogre::String &mn, int size,
		int aSize)
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

	// create the scene node
	mNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

	// Configure the atlas size if we need it
	ASSERT(aSize > 0);
	if(aSize == 1){
		return true;
	}
	int textSize = GUIHelper::getTextureWidth(mn);
	mAtlasSize = static_cast<float>(textSize) / static_cast<float>(size);
	mAtlasSize = mAtlasSize / static_cast<float>(textSize);

	// enqueue the billboards
	for(int i = 0; i < size; ++i){
		Ogre::Billboard *b = mBillboardSet->createBillboard(Ogre::Vector3(0,-9999.0f,0));
		ASSERT(b);
		mBillboards.push_back(b);
	}



	return true;
}

/**
 * Returns if the manager is created or not
 */
bool BillboardManager::isCreated(void)
{
	return mBillboardSet != 0;
}

/**
 * Set the billboards visible/invisible
 */
void BillboardManager::setVisible(bool visible)
{
	ASSERT(mBillboardSet);
	mBillboardSet->setVisible(visible);
}

/**
 * Set the "bounds" where the billboards will be seen (normally the size of
 * the level by now).
 */
void BillboardManager::setBounds(const Ogre::AxisAlignedBox &bb, Ogre::Real radius)
{
	ASSERT(mBillboardSet);
	mBillboardSet->setBounds(bb, radius);
	mBillboardSet->setCullIndividually(true);
}

/**
 * Get a new Billboard. If have no more billboard then 0 is returned
 */
Ogre::Billboard *BillboardManager::getNewBillboard(int atlasNumber)
{
	ASSERT(mBillboardSet);
	if(mBillboards.empty()){
		return 0;
	}
	// check if we have to attach to the scene node
	if(mBillboards.size() == mBillboardSet->getPoolSize()){
		// this is the first that we will remove
		mNode->attachObject(mBillboardSet);
	}

	Ogre::Billboard *result = mBillboards.front();
	mBillboards.pop_front();

	// configure the atlas if needed
	if(atlasNumber >= 0){
		Ogre::Real x1 = (mAtlasSize * atlasNumber);
		ASSERT(x1 + mAtlasSize <= 1.0f);
		result->setTexcoordRect(x1, 0.0f, x1+mAtlasSize, 1.0f);
	}

	return result;
}

/**
 * Change the atlas of a billboard
 * @param	bb			The billboard to modify
 * @param	atlasNum	The atlas number to be set
 */
void BillboardManager::changeAtlas(Ogre::Billboard *bb, int atlasNumber)
{
	ASSERT(bb);
	ASSERT(atlasNumber >= 0);

	Ogre::Real x1 = (mAtlasSize * atlasNumber);
	ASSERT(x1 + mAtlasSize <= 1.0f);
	bb->setTexcoordRect(x1, 0.0f, x1+mAtlasSize, 1.0f);
}

/**
 * Returns the atlas id of a billboard
 */
int BillboardManager::getAtlas(Ogre::Billboard *bb)
{
	ASSERT(bb);
	const Ogre::Real x1 = bb->getTexcoordRect().left;
	return (x1 / mAtlasSize);
}

/**
 * Let a new billboard available
 */
void BillboardManager::letAvailable(Ogre::Billboard *b)
{
	ASSERT(b);
	ASSERT(mBillboards.size()+1 <= mBillboardSet->getPoolSize());

	mBillboards.push_back(b);

	// check if it was the last one
	if(mBillboards.size() == mBillboardSet->getPoolSize()){
		mNode->detachObject(mBillboardSet);
	}

	// hide it
	b->setPosition(0,-9999.0f,0);
}
