/*
 * CellphoneMap.h
 *
 *  Created on: 25/06/2012
 *      Author: agustin
 */

#ifndef CELLPHONEMAP_H_
#define CELLPHONEMAP_H_

#include <OgreString.h>
#include <OgreTexture.h>
#include <OgreOverlayContainer.h>

#include <vector>
#include <set>

#include "DebugUtil.h"
#include "CommonMath.h"
#include "CellphoneMapInfo.h"
#include "CellphoneElement.h"
#include "QueueContainer.h"

class PlayerUnit;
class GameUnit;


// Some definitions to use
#define CPM_ZOMBIE_ELEMENT		"HUD/CP/Unit/Zombie"
#define CPM_PLAYER_ELEMENT		"HUD/CP/Unit/Player"

class CellphoneMap
{
	static const Ogre::Real		UPDATE_TIME		=	0.5f;
public:
	CellphoneMap();
	~CellphoneMap();

	/**
	 * Configure the Map to support a certain number of elements
	 * @param	maxPlayers
	 * @param	maxZombies		The max number of zombies that we want to show
	 * 							at the same time
	 */
	void configure(int maxZombies = 30);

	/**
	 * Hide/Show the element
	 */
	void show(void);
	void hide(void);

	/**
	 * Get the OverlayContainer used
	 */
	Ogre::OverlayContainer *getContainer(void);

	/**
	 * Set a new map to show
	 * @param 	m	The map to show
	 */
	void setMap(const CellphoneMapInfo &map);

	/**
	 * Add new static element to the cellphone. This objects doesn't need to
	 * be updated. Once we add it to the cellphone it can't be moved
	 * @param	elem	The static element
	 * @note	The elem musen't be freed by caller AND the caller is the owner
	 * 			of the object (must free the element once it was removed from
	 * 			this class)
	 */
	void addStaticElement(CellphoneElement *elem);
	void removeStaticElement(CellphoneElement *elem);
	void removeAllStaticelements(void);

	/**
	 * Add a player to track zombies. This player will be used to get all the
	 * nearby zombies and show it in the map.
	 */
	void addPlayer(PlayerUnit *pu);
	void removePlayer(PlayerUnit *pu);
	void removeAllPlayers(void);

	/**
	 * Update the logic of the map
	 */
	void update(void);

protected:
	/**
	 * Create zombie elements for the queue
	 */
	void createZombieElements(int n);

	/**
	 * Create a Player Element
	 */
	void createPlayerElement(const Ogre::String &name);

	/**
	 * Show all the elements of the zombie set
	 */
	void showZombieSet(void);

	/**
	 * Clear all the elements of the queue that will not be used
	 */
	void resizeActiveZombiesElem(void);

	/**
	 * Configure the element
	 */
	inline void configureElement(CellphoneElement *elem) const;

	/**
	 * Position the element in the corresponding position
	 */
	inline void positionElement(CellphoneElement *elem,
			const Ogre::Real posX, const Ogre::Real posY);

private:
	typedef std::vector<CellphoneElement *>		ElementVec;
	typedef std::vector<PlayerUnit *>			PlayerVec;
	typedef std::set<GameUnit *>				ZombieSet;
	typedef QueueContainer<CellphoneElement>	ZombieElementQueue;


	PlayerVec				mPlayers;
	Ogre::OverlayContainer	*mContainer;
	Ogre::Real				mXFactor;
	Ogre::Real				mYFactor;
	Ogre::Real				mAccumTime;
	ElementVec				mStaticElements;
	ElementVec				mPlayerElements;


	ZombieSet				mZombiesSeen;
	ElementVec				mActiveZombiesElem;
	ZombieElementQueue		mZombieElemQueue;

};




inline void CellphoneMap::configureElement(CellphoneElement *elem) const
{
	ASSERT(elem);
	ASSERT(elem->element);
	elem->element->setHorizontalAlignment(Ogre::GuiHorizontalAlignment::GHA_CENTER);
	elem->element->setVerticalAlignment(Ogre::GuiVerticalAlignment::GVA_CENTER);
}

inline void CellphoneMap::positionElement(CellphoneElement *elem,
		const Ogre::Real posX, const Ogre::Real posY)
{
	ASSERT(elem);
	ASSERT(elem->element);
	elem->element->setLeft(posX * mXFactor * mContainer->getWidth());
	elem->element->setTop(posY * mYFactor * mContainer->getHeight());
	debugBLUE("Position element: %f, %f, real pos: %f, %f\n",
			elem->element->getLeft(), elem->element->getTop(), posX, posY);
}


#endif /* CELLPHONEMAP_H_ */
