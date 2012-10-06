/*
 * MenuButtonEffect.cpp
 *
 *  Created on: 23/08/2012
 *      Author: agustin
 *
 */

#include "MenuButtonEffect.h"

#include "MenuButton.h"

namespace OvEff {

MenuButtonEffect::MenuButtonEffect(MenuButton *button, OverlayEffect *effect) :
		mMenuButton(button),
		mEffect(effect)
{

}

MenuButtonEffect::~MenuButtonEffect()
{
    if (mEffect != 0) mEffect->removeCallback(this);
}

void MenuButtonEffect::operator()(EffectCb::EventID id)
{
	ASSERT(mMenuButton);

	if(id == EffectCb::STARTING){
		// we have to remove the menuButton from the Manager
		mMenuButton->setEnable(false);
	} else if(id == EffectCb::ENDING){
		// activate again
		mMenuButton->setEnable(true);
	}
#ifdef DEBUG
	else {
		ASSERT(false);
	}
#endif
}

}
