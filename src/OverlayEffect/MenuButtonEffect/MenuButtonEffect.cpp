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
}

void MenuButtonEffect::operator()(OverlayEffect::EventID id)
{
	ASSERT(mMenuButton);

	if(id == OverlayEffect::STARTING){
		// we have to remove the menuButton from the Manager
		mMenuButton->setEnable(false);
	} else if(id == OverlayEffect::ENDING){
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
