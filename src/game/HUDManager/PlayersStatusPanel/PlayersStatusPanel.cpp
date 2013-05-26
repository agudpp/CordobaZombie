/*
 * PlayersStatusPanel.cpp
 *
 *  Created on: 16/06/2012
 *      Author: agustin
 */



#include "PlayersStatusPanel.h"

#include <OgreOverlayManager.h>

#include "DebugUtil.h"
#include "PlayerUnit.h"
#include "PlayerStatus.h"
#include "PlayerStatusBuilder.h"
#include "PlayerUnitManager.h"

namespace hud {

#ifdef DEBUG
bool PlayersStatusPanel::checkUnitInList(PlayerUnit *u)
{
	int i = mPlayerStatus.size();
	int j = 0;
	for(; j < i; j++)if(mPlayerStatus[j]->getPlayerUnit() == u) return true;
	return false;
}
#endif


////////////////////////////////////////////////////////////////////////////////
PlayersStatusPanel::PlayersStatusPanel() :
mOverlay(0)
{

}


////////////////////////////////////////////////////////////////////////////////
PlayersStatusPanel::~PlayersStatusPanel()
{
	// TODO: eliminar todos los player status panel
	unload();
}

////////////////////////////////////////////////////////////////////////////////
void PlayersStatusPanel::load(void)
{
	ASSERT(mPlayerSelected.empty());
	ASSERT(mPlayerStatus.empty());

	// load the overlay
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	mOverlay = om.getByName(PLAYERSP_OVERLAY);
	ASSERT(mOverlay);
	mOverlay->show();

	// Create the status panels
	PlayerStatusBuilder psb;
	PlayerUnitManager &pum = PlayerUnitManager::getInstance();

	PlayerStatus *ps = psb.create(PlayerStatusBuilder::PLAYER_CARDENAL,
			pum.getPlayer(PlayerUnitManager::PLAYER_CARDENAL));
	ASSERT(ps);
	mPlayerStatus.push_back(ps);

	ps = psb.create(PlayerStatusBuilder::PLAYER_CORAL,
				pum.getPlayer(PlayerUnitManager::PLAYER_CORAL));
	ASSERT(ps);
	mPlayerStatus.push_back(ps);

	ps = psb.create(PlayerStatusBuilder::PLAYER_TULIAN,
				pum.getPlayer(PlayerUnitManager::PLAYER_TULIAN));
	ASSERT(ps);
	mPlayerStatus.push_back(ps);

	debugGREEN("All the PlayerStatus was builded OK\n");

	// Create the big life bar
	mBigLoadingBar.setOverlayContainer(static_cast<Ogre::OverlayContainer *>(
			om.getOverlayElement(PLAYERSP_LOADINGBAR)));

}
////////////////////////////////////////////////////////////////////////////////
void PlayersStatusPanel::unload(void)
{
	for(int i = 0; i < mPlayerStatus.size(); ++i){
		delete mPlayerStatus[i];
	}
	mPlayerStatus.clear();
	mPlayerSelected.clear();
}


////////////////////////////////////////////////////////////////////////////////
void PlayersStatusPanel::show(void)
{
	mOverlay->show();
	mBigLoadingBar.show();
}

////////////////////////////////////////////////////////////////////////////////
void PlayersStatusPanel::hide(void)
{
	mOverlay->hide();
	mBigLoadingBar.hide();
}


///								INTERFACE								///

////////////////////////////////////////////////////////////////////////////////
void PlayersStatusPanel::selectOneUnit(PlayerUnit *unit)
{
	ASSERT(unit);
#ifdef DEBUG
	ASSERT(checkUnitInList(unit));
#endif

	// if we have player selected, we unselect all
	if(!mPlayerSelected.empty()) unselectAll();

	// select this unit
	int size = mPlayerStatus.size();
	PlayerStatus *pu = 0;
	for(int i = 0; i < size; ++i)
		if(mPlayerStatus[i]->getPlayerUnit() == unit){
			pu = mPlayerStatus[i];
			break;
		}
	ASSERT(pu);
	mPlayerSelected.push_back(pu);
	pu->setSelected(true);

	// we are the only unit selected.. we use the main bar
	pu->useMainLifeBar();
	mBigLoadingBar.show();
}

////////////////////////////////////////////////////////////////////////////////
void PlayersStatusPanel::selectAnotherUnit(PlayerUnit *unit)
{
	ASSERT(unit);
#ifdef DEBUG
	ASSERT(checkUnitInList(unit));
#endif
	// check if we already have the element
	int size = mPlayerSelected.size();
	for(int i = 0; i < size; ++i) if(mPlayerSelected[i]->getPlayerUnit() == unit) return;

	// else we are selecting a new one
	size = mPlayerStatus.size();
	PlayerStatus *pu = 0;
	for(int i = 0; i < size; ++i)
		if(mPlayerStatus[i]->getPlayerUnit() == unit){
			pu = mPlayerStatus[i];
			break;
		}
	ASSERT(pu);
	mPlayerSelected.push_back(pu);
	pu->setSelected(true);
	if(mPlayerSelected.size() == 1){
		// we are the only unit... use the main bar
		pu->useMainLifeBar();
		mBigLoadingBar.show();
	}

}

////////////////////////////////////////////////////////////////////////////////
void PlayersStatusPanel::selectAll(void)
{
	int size = mPlayerStatus.size();
	for(int i = 0; i < size; ++i){
		selectAnotherUnit(mPlayerStatus[i]->getPlayerUnit());
	}
}

////////////////////////////////////////////////////////////////////////////////
void PlayersStatusPanel::unselectUnit(PlayerUnit *unit)
{
	ASSERT(unit);
#ifdef DEBUG
	ASSERT(checkUnitInList(unit));
#endif

	// get the unit from the selected ones
	int size = mPlayerSelected.size();
	int i = 0;
	PlayerStatus *pu = 0;
	for(; i < size; ++i)
		if(mPlayerSelected[i]->getPlayerUnit() == unit){
			pu = mPlayerSelected[i];
			break;
		}

	ASSERT(pu);
	if(!pu) return; // unselecting an unselected unit? WTF

	// remove from the selected vector
	mPlayerSelected[i] = mPlayerSelected[size-1];
	mPlayerSelected.pop_back();

	pu->setSelected(false);

	// if no player selected, hide the main bar
	if(mPlayerSelected.empty()) mBigLoadingBar.hide();
}

////////////////////////////////////////////////////////////////////////////////
void PlayersStatusPanel::unselectAll(void)
{
	int size = mPlayerSelected.size();
	for(int i = 0; i < size; ++i)
		mPlayerSelected[i]->setSelected(false);

	mPlayerSelected.clear();
	mBigLoadingBar.hide();
}


////////////////////////////////////////////////////////////////////////////////
bool PlayersStatusPanel::isUnitSelected(PlayerUnit *unit)
{
	int size = mPlayerSelected.size();
	for(int i = 0; i < size; ++i)
		if(mPlayerSelected[i]->getPlayerUnit() == unit) return true;

	return false;
}

}

