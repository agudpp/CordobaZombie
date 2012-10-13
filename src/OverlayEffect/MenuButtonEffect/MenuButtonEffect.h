/*
 * MenuButtonEffect.h
 *
 *  Created on: 23/08/2012
 *      Author: agustin
 *
 */

#ifndef MENUBUTTONEFFECT_H_
#define MENUBUTTONEFFECT_H_

#include <boost/bind.hpp>
#include <boost/signal.hpp>

#include "DebugUtil.h"
#include "OverlayEffect.h"

class MenuButton;

namespace OvEff {

class MenuButtonEffect {
public:
	MenuButtonEffect(MenuButton	*button = 0, OverlayEffect *effect = 0);
	virtual ~MenuButtonEffect();

	// Implement the copy and = operator (because we need to register the
	// callbacks again
	//
	inline MenuButtonEffect(const MenuButtonEffect &other);
	inline MenuButtonEffect &operator=(const MenuButtonEffect &other);

	/**
	 * Set / Get the button/effect
	 */
	inline void setEffect(OverlayEffect *e);
	inline OverlayEffect *getEffect(void);

	inline void setButton(MenuButton *mb);
	inline MenuButton *getButton(void);

	/**
	 * Function to be implemented
	 * @param	id	The event ID
	 */
	void operator()(OverlayEffect::EventID id);

private:
	MenuButton		*mMenuButton;
	OverlayEffect	*mEffect;
	boost::signals::scoped_connection mConnection;
};



inline
MenuButtonEffect::MenuButtonEffect(const MenuButtonEffect &other)
{
    mMenuButton = other.mMenuButton;
    mEffect = other.mEffect;
    if (mEffect != 0) {
        mConnection = mEffect->addCallback(boost::bind(
                &MenuButtonEffect::operator(), this, _1));
    }
}
inline MenuButtonEffect &
MenuButtonEffect::operator=(const MenuButtonEffect &other)
{
    mMenuButton = other.mMenuButton;
    if (mConnection.connected()) mConnection.disconnect();
    mEffect = other.mEffect;
    if (mEffect != 0) {
        mConnection = mEffect->addCallback(boost::bind(
                &MenuButtonEffect::operator(), this, _1));
    }
}

inline void MenuButtonEffect::setEffect(OverlayEffect *e)
{
	ASSERT(e);
	// if we had an old effect, first remove the callback
	if (mConnection.connected()) mConnection.disconnect();

	mEffect = e;
	mConnection = mEffect->addCallback(boost::bind(
	               &MenuButtonEffect::operator(), this, _1));
}
inline OverlayEffect *MenuButtonEffect::getEffect(void)
{
	return mEffect;
}

inline void MenuButtonEffect::setButton(MenuButton *mb)
{
	ASSERT(mb);
	mMenuButton = mb;
}
inline MenuButton *MenuButtonEffect::getButton(void)
{
	return mMenuButton;
}

}

#endif /* MENUBUTTONEFFECT_H_ */
