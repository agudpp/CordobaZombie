/*
 * MainGui.h
 *
 *  Created on: 17/12/2011
 *      Author: Emi
 */

#ifndef MAINGUI_H_
#define MAINGUI_H_

#include "Menu.h"
#include "HistoriaGui.h"
#include "OpcionesGui.h"
#include "CreditosGui.h"
#include <map>
#include <iostream>
#include <stdlib.h>

using namespace std;

class MainGui: public Menu {

private:
	Menu *historia, *opciones, *creditos;
	map <CEGUI::String, Menu *> mapaMenus;


public:

	MainGui(CEGUI::WindowManager * w, CEGUI::String LayoutFileName, bool active);
	virtual ~MainGui();
	void loadMenu();
	void unloadMenu();
	void RegisterForSpecificEvents();
	bool changeMenu(const CEGUI::EventArgs& e);


};

#endif /* MAINGUI_H_ */
