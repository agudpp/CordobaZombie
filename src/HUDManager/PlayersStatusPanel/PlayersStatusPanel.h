/*
 * PlayersStatusPanel.h
 *
 *  Created on: 16/06/2012
 *      Author: agustin
 */

#ifndef PLAYERSSTATUSPANEL_H_
#define PLAYERSSTATUSPANEL_H_

#include <OgreOverlay.h>

#include <vector>

#include "HudElement.h"
#include "LoadingBar.h"

class PlayerStatus;
class PlayerUnit;



#define PLAYERSP_OVERLAY		"HUD/PlayersStatusPanel"
#define PLAYERSP_LOADINGBAR		"HUD/PSP/MainBar"

class PlayersStatusPanel : public HudElement
{
	typedef std::vector<PlayerStatus *>		StatusVec;

public:
	PlayersStatusPanel();
	virtual ~PlayersStatusPanel();

	/**
	 * Load/Unload all the resources of the HudElement
	 */
	virtual void load(void);
	virtual void unload(void);

	/**
	 * Hide/Show the element
	 */
	virtual void show(void);
	virtual void hide(void);


	///								INTERFACE								///

	/**
	 * Select one unit (and only one). If other units are selected this will
	 * unselect the other first.
	 * @param	unit		The unit to be selected
	 */
	void selectOneUnit(PlayerUnit *unit);

	/**
	 * Add another unit to be selected (if any unit is already selected, this
	 * will add the specified unit to be selected too, if no unit are selected
	 * then this one will be selected).
	 * @param	unit		The unit to be added to the selection
	 */
	void selectAnotherUnit(PlayerUnit *unit);

	/**
	 * Select all the units
	 */
	void selectAll(void);

	/**
	 * Unselect one or all the units.
	 * @param	unit		The unit to be unselected
	 */
	void unselectUnit(PlayerUnit *unit);
	void unselectAll(void);

	/**
	 * Check if a unit is already selected
	 * @param	unit		The unit to be checked if is already selected
	 */
	bool isUnitSelected(PlayerUnit *unit);

private:
#ifdef DEBUG
	bool checkUnitInList(PlayerUnit*);
#endif

private:
	StatusVec			mPlayerStatus;
	StatusVec			mPlayerSelected;
	LoadingBar			mBigLoadingBar;
	Ogre::Overlay		*mOverlay;
};

#endif /* PLAYERSSTATUSPANEL_H_ */
