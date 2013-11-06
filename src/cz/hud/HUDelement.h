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

namespace cz {

class HUDelement
{
public:
	HUDelement();
	virtual ~HUDelement();

public:
	/**
	 * @brief Set element overlay visibility to 'visible'
	 */
	void
	setVisible(bool visible);

private:
	Ogre::Overlay mOverlay;
};


///////////////////////////////////////////////////////////////////////////////
inline void
HUDelement::setVisible(bool visible)
{
	if (visible && !mOverlay.isVisible())
		mOverlay.show();
	else if (!visible && mOverlay.isVisible())
		mOverlay.hide();
	return
}



} /* namespace cz */
#endif /* HUDELEMENT_H_ */
