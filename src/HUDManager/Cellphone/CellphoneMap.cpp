/*
 * CellphoneMap.cpp
 *
 *  Created on: 25/06/2012
 *      Author: agustin
 */

#include <OgreTextureManager.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayElement.h>
#include <algorithm>

#include "CellphoneMap.h"
#include "DebugUtil.h"
#include "GlobalObjects.h"
#include "PlayerUnit.h"
#include "GameUnit.h"
#include "CommonMath.h"


////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::createZombieElements(int n)
{
	ASSERT(n > 0);

	mZombieElemQueue.freeAll();
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	for(int i = 0; i < n; ++i){
		Ogre::String name = "cpze" + Ogre::StringConverter::toString(i);
		Ogre::OverlayElement *elem = om.createOverlayElementFromTemplate(
				CPM_ZOMBIE_ELEMENT, "Panel", name, true);
		ASSERT(elem);
		CellphoneElement *cpelem = new CellphoneElement;
		cpelem->element = elem;
		mZombieElemQueue.addNewObject(cpelem);
		configureElement(cpelem);
	}
}

////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::createPlayerElement(const Ogre::String &name)
{
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	Ogre::OverlayElement *elem = om.createOverlayElementFromTemplate(
			CPM_PLAYER_ELEMENT, "Panel", name, true);
	ASSERT(elem);
	CellphoneElement *cpelem = new CellphoneElement;
	cpelem->element = elem;
	mPlayerElements.push_back(cpelem);
	configureElement(cpelem);

}

////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::showZombieSet(void)
{
	int i = mActiveZombiesElem.size()-1;
	ZombieSet::iterator bit = mZombiesSeen.begin(), eit = mZombiesSeen.end();
	for(; bit != eit && i >= 0; ++bit, --i){
		// show the zombies in their respective position
		const sm::Vector2 &zpos = (*bit)->getPosition();
		positionElement(mActiveZombiesElem[i], zpos.x, zpos.y);
	}
}

////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::resizeActiveZombiesElem(void)
{
	int left = mActiveZombiesElem.size() - mZombiesSeen.size();
	if(left < 0){
		// we need to get more elements from the queue
		left = -left;
		for(int i = 0; i < left; ++i){
			CellphoneElement *elem = mZombieElemQueue.getAvailableObject();
			if(!elem) {
				debugWARNING("No hay suficientes elementos para mostrar a todos "
						"los zombies al mismo tiempo :(\n");
				break;
			}
			mActiveZombiesElem.push_back(elem);
		}
	} else if(left > 0){
		// we need to remove elements from the active zombies element vector
		for(int i = 0; i < left; ++i){
			CellphoneElement *elem = mActiveZombiesElem.back();
			ASSERT(elem->element);
			mContainer->removeChild(elem->element->getName());
			mZombieElemQueue.addNewObject(elem);
			mActiveZombiesElem.pop_back();
		}
	} else {
		// left == 0... we have to do nothing..
	}
}


CellphoneMap::CellphoneMap() :
mXFactor(0),
mYFactor(0),
mContainer(0),
mAccumTime(0)
{
	mContainer = static_cast<Ogre::OverlayContainer *> (
			Ogre::OverlayManager::getSingleton().createOverlayElement(
			"Panel", "cpmap"));
	ASSERT(mContainer);
}

CellphoneMap::~CellphoneMap()
{
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	// iterate over all the elements and remove all the overlays
	 std::deque<CellphoneElement*> &q = mZombieElemQueue.getQueue();
	 for(std::deque<CellphoneElement*>::iterator it = q.begin();
			 it != q.end(); ++it){
		 om.destroyOverlayElement((*it)->element, true);
	 }
	 mZombieElemQueue.freeAll();

	 removeAllPlayers();
	 removeAllStaticelements();

	// destroy texture
	GUIHelper::destroyMaterial(mContainer->getMaterial()->getName());

	GUIHelper::fullDestroyOverlayElement(mContainer);
}

////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::configure(int maxZombies)
{
	ASSERT(maxZombies > 0);
	createZombieElements(maxZombies);
}

////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::show(void)
{
	ASSERT(mContainer);
	mContainer->show();
}

////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::hide(void)
{
	ASSERT(mContainer);
	mContainer->hide();
}

////////////////////////////////////////////////////////////////////////////////
Ogre::OverlayContainer *CellphoneMap::getContainer(void)
{
	return mContainer;
}

////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::setMap(const CellphoneMapInfo &map)
{
	ASSERT(mContainer);
	if(!mContainer->getMaterial().isNull()){
		GUIHelper::destroyMaterial(mContainer->getMaterial()->getName());
	}
	mContainer->setMaterialName(map.materialName);
	mXFactor = map.getXFactor();
	ASSERT(mXFactor > 0);
	mYFactor = map.getYFactor();
	ASSERT(mYFactor > 0);
}

////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::addStaticElement(CellphoneElement *elem)
{
	ASSERT(elem);
	mStaticElements.push_back(elem);

	// add to the container
	ASSERT(elem->element);
	ASSERT(mContainer);
	configureElement(elem);
	mContainer->addChild(elem->element);
}
////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::removeStaticElement(CellphoneElement *elem)
{
	int i = mStaticElements.size() - 1;
	for(; i >= 0; --i){
		if(mStaticElements[i] == elem){
			break;
		}
	}
	ASSERT(i >= 0);
	mStaticElements[i] = mStaticElements.back();
	mStaticElements.pop_back();
	mContainer->removeChild(elem->element->getName());
}
////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::removeAllStaticelements(void)
{
	for(int i = mStaticElements.size()-1; i >= 0; --i){
		mContainer->removeChild(mStaticElements[i]->element->getName());
	}
	mStaticElements.clear();
}

////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::addPlayer(PlayerUnit *pu)
{
	ASSERT(pu);
#ifdef DEBUG
	for(int i = 0; i < mPlayers.size(); ++i){
		ASSERT(mPlayers[i] != pu);
	}
#endif
	mPlayers.push_back(pu);
	createPlayerElement("mapUni"+pu->getEntity()->getName());
	mContainer->addChild(mPlayerElements[mPlayerElements.size()-1]->element);
	GUIHelper::reposContainer(static_cast<Ogre::OverlayContainer *>(
					mPlayerElements[mPlayerElements.size()-1]->element));
}
////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::removePlayer(PlayerUnit *pu)
{
	int i = 0;
	for(; i < mPlayers.size(); ++i){
		if(mPlayers[i] == pu) break;
	}
	ASSERT(i < mPlayers.size());
	mPlayers[i] = mPlayers.back();
	mPlayers.pop_back();

	ASSERT(i < mPlayerElements.size());
	// destroy the element
	mContainer->removeChild(mPlayerElements[i]->element->getName());
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	om.destroyOverlayElement(mPlayerElements[i]->element, true);
	delete mPlayerElements[i];
	mPlayerElements[i] = mPlayerElements.back();
	mPlayerElements.pop_back();
}
////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::removeAllPlayers(void)
{
	PlayerVec aux = mPlayers;
	for(int i = mPlayers.size()-1 ; i >= 0; --i) removePlayer(aux[i]);

	ASSERT(mPlayers.empty());
	ASSERT(mPlayerElements.empty());
}

////////////////////////////////////////////////////////////////////////////////
void CellphoneMap::update(void)
{
	// if is hided return
	if(!mContainer->isVisible()) return;
	// is time to update?
	mAccumTime += GLOBAL_TIME_FRAME;
	if(mAccumTime < UPDATE_TIME){
		return;
	}
	mAccumTime = 0.0f;

	// get all the zombies and update the players
	mZombiesSeen.clear();
	for(int i = 0; i < mPlayers.size(); ++i){
		// update the position of the player
		ASSERT(i < mPlayerElements.size());
		const sm::Vector2 &ppos = mPlayers[i]->getPosition();
		positionElement(mPlayerElements[i], ppos.x, ppos.y);

		PlayerUnit::ZombieUnitVec &vec = mPlayers[i]->getNearbyZombies();
		mZombiesSeen.insert(vec.begin(), vec.end());
	}

	// remove all the points in the map that we don't need
	resizeActiveZombiesElem();

	// show the new zombies
	showZombieSet();
}

