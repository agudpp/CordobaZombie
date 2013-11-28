/*
 * HUDelement.h
 *
 *  This is the base class for the HUD elements, which may include e.g.
 *  bullets left in magazine, life left, selected weapon, etc.
 *
 *  Created on: Nov 5, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef HUDELEMENT_H_
#define HUDELEMENT_H_

#include <OgreOverlay.h>
#include <debug/DebugUtil.h>


namespace cz {

class HUDelement
{
protected:
	// @brief Default initial weapon to show on HUD
	static const enum WeaponID initialWeapon = WID_9MM;

public:
	HUDelement() : mName(0) {}
	virtual ~HUDelement();

public:
	/**
	 * @brief Set element overlay container visibility to 'visible'
	 */
	virtual void
	setVisible(bool visible) = 0;

	/**
	 * @brief  Fill in the structure (create overlays, parse files, etc)
	 * @return true on success, false otherwise
	 */
	virtual bool
	build(Ogre::Overlay* ov) = 0;

	/**
	 * @brief Return non-modifyable HUDelement name
	 */
	const char*
	getName(void) const;

protected:
	const char* mName;
};

///////////////////////////////////////////////////////////////////////////////
inline HUDelement::~HUDelement() { /* Auto-generated destructor stub */ }

///////////////////////////////////////////////////////////////////////////////
inline const char* HUDelement::getName(void) const { return mName; }

} /* namespace cz */

#endif /* HUDELEMENT_H_ */
