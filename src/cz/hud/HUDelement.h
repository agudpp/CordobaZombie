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

#include <OgreOverlayContainer.h>



// XXX  XXX  XXX  XXX  XXX  XXX  XXX  XXX  XXX  XXX  XXX
//
// TODO: check ConfigState.{h,cpp} in http://goo.gl/uaYlB7
//		  for Ogre::{Overlay,OverlayContainer} usage
//
// XXX  XXX  XXX  XXX  XXX  XXX  XXX  XXX  XXX  XXX  XXX



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
	Ogre::OverlayContainer* mOvCont;
};


///////////////////////////////////////////////////////////////////////////////
inline void
HUDelement::setVisible(bool visible)
{
	if (!mOvCont) {
		debugERROR("Called before creating the overlay container.\n");

	} else {
		if (visible && !mOvCont->isVisible())
			mOvCont->show();
		else if (!visible && mOvCont->isVisible())
			mOvCont->hide();
	}
	return;
}



} /* namespace cz */
#endif /* HUDELEMENT_H_ */
