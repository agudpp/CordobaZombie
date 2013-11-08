/*
 * HUD.h
 *
 *  This is the highest level class of the HUD system.
 *  It offers the methods to callback from other game classes.
 *  It handles internally the HUDelement objects.
 *
 *  Created on: Nov 1, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#include <vector>
#include <main_player/weapon/Weapon.h>
#include "HUDDefines.h"
#include "HUDelement.h"

// TODO locate EventType definition file and namespace
#include <trigger_system/TriggerSystemDefines.h>
#include <ia/fsm/FSMMachine.h>

namespace cz {

class HUD
{
public:
	HUD();
	virtual ~HUD();

public:
	/*************************************************************************/
	/**  XXX  Methods offered for callbacks binding	                        **/

	// TODO: document usage after implementation
	void
	updateWeapon(WeaponID id);

	// TODO: document usage after implementation
	void
	updateBullets(Weapon *w, TriggerCallback::EventType e);

	/**
	 * @brief Set visibility of all elements loaded in HUD to 'visible'
	 */
	void
	setVisible(bool visible);

	/**	 <END>  Methods offered for callbacks binding						**/
	/*************************************************************************/

private:
	// Prevent the compiler from generating methods to copy the instance.
	HUD(HUD const&);              // Don't implement!
	void operator=(HUD const&);  // Don't implement!

private:
	std::vector<HUDelement> mElements;
};


///////////////////////////////////////////////////////////////////////////////
inline void
HUD::setVisible(bool visible)
{
	for (uint i=0 ; i < mElements.size() ; i++) {
		mElements[i].setVisible(visible);
	}
}


}
