/*
 * Menu.h
 *
 *  Created on: 17/12/2011
 *      Author: Emi
 */

#ifndef MENU_H_
#define MENU_H_

#include "CEGUI.h"
#include <string.h>
#include <iostream>
#include <stdlib.h>

class Menu {

protected:
	CEGUI::String name;
	CEGUI::String layoutFileName;
	CEGUI::Window* layout;
	CEGUI::WindowManager * wMgrPtr;
	bool isMenuActive, animationsCreated;
	float comienzo, final;


	CEGUI::Animation *animLeftPanelEnter, *animLeftPanelExit, *animContainerEnter, *animContainerExit;
	CEGUI::Affector *affectorLeftPanelEnter, *affectorLeftPanelExit, *affectorContainerEnter, *affectorContainerExit;
	CEGUI::AnimationInstance *instanceLeftPanelEnter, *instanceLeftPanelExit, *instanceContainerEnter, *instanceContainerExit;




public:

	Menu(){};
	Menu(CEGUI::WindowManager * w, CEGUI::String LayoutFileName, bool active);
	virtual ~Menu();
	CEGUI::String getName(){return name;};
	CEGUI::String getLayoutFileName(){return layoutFileName;};
	CEGUI::Window * getLayoutPtr(){return layout;};
	virtual void loadMenu();
	virtual void animateMenu();
	virtual void unloadMenu();
	virtual void RegisterForEvents();
	virtual void RegisterForSpecificEvents() = 0;
	void createAnimations();
	bool imprimirTexto(const CEGUI::EventArgs& e);
	bool nombreLayout(const CEGUI::EventArgs& e);

};

#endif /* MENU_H_ */
