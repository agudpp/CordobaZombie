/*
 * Cellphone.h
 *
 *  Created on: 16/06/2012
 *      Author: agustin
 */

#ifndef CELLPHONE_H_
#define CELLPHONE_H_

#include <OgreOverlay.h>
#include <OgreOverlayContainer.h>

#include <vector>

#include "HudElement.h"
#include "CellphoneMap.h"
#include "CellphoneElement.h"


class PlayerUnit;
class ZombieUnit;


#define HUD_CELLPHONE_OVERLAY		"HUD/Cellphone"
#define HUD_CELLPHONE_MAP_OVERLAY	"HUD/CP/Map"



class Cellphone : public HudElement
{
	static const Ogre::Real		UPDATE_TIME		=	0.5f;
public:
	Cellphone();
	virtual ~Cellphone();

	/**
	 * Load/Unload all the resources of the HudElement
	 */
	virtual void load(void) = 0;
	virtual void unload(void) = 0;

	/**
	 * Hide/Show the element
	 */
	virtual void show(void) = 0;
	virtual void hide(void) = 0;

	/**
	 * Set a new map to show
	 * @param 	m	The map to show
	 */
	void setMap(const CellphoneMap &map);

	/**
	 * Add new static element to the cellphone. This objects doesn't need to
	 * be updated. Once we add it to the cellphone it can't be moved
	 * @param	elem	The static element
	 * @note	The elem musen't be freed by caller.
	 */
	void addStaticElement(CellphoneElement *elem);
	void removeStaticElement(CellphoneElement *elem);

	/**
	 * Add a player to track zombies. This player will be used to get all the
	 * nearby zombies and show it in the map.
	 */
	void addPlayer(PlayerUnit *pu);
	void removePlayer(PlayerUnit *pu);

	/**
	 * Update the logic of the Cellphone
	 */
	void update(void);

protected:
	/**
	 * Add new zombie to show it
	 */

private:
	typedef std::vector<CellphoneElement *>	ElementVec;


	Ogre::Overlay			*mOverlay;
	Ogre::OverlayContainer	*mMap;



};

#endif /* CELLPHONE_H_ */
