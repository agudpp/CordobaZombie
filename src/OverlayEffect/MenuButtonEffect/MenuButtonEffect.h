/*
 * MenuButtonEffect.h
 *
 *  Created on: 23/08/2012
 *      Author: agustin
 *
 */

#ifndef MENUBUTTONEFFECT_H_
#define MENUBUTTONEFFECT_H_

#include "DebugUtil.h"

#include "EffectCb.h"
#include "OverlayEffect.h"

class MenuButton;

namespace OvEff {

class MenuButtonEffect : public EffectCb {
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
	void operator()(EffectCb::EventID id);

private:
	MenuButton		*mMenuButton;
	OverlayEffect	*mEffect;
};



inline
MenuButtonEffect::MenuButtonEffect(const MenuButtonEffect &other)
{
    mMenuButton = other.mMenuButton;
    mEffect = other.mEffect;
    if (mEffect != 0) mEffect->addCallback(this);
}
inline MenuButtonEffect &
MenuButtonEffect::operator=(const MenuButtonEffect &other)
{
    mMenuButton = other.mMenuButton;
    if (mEffect != 0) mEffect->removeCallback(this);
    mEffect = other.mEffect;
    if (mEffect != 0) mEffect->addCallback(this);
}

inline void MenuButtonEffect::setEffect(OverlayEffect *e)
{
	ASSERT(e);
	// if we had an old effect, first remove the callback
	if (mEffect != 0) mEffect->removeCallback(this);

	mEffect = e;
	mEffect->addCallback(this);
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
