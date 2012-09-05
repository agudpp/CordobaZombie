/*
 * HistoriaGui.h
 *
 *  Created on: Jan 7, 2012
 *      Author: Emi
 */

#ifndef HISTORIAGUI_H_
#define HISTORIAGUI_H_

#include "Menu.h"

class HistoriaGui: public Menu {

public:
	HistoriaGui(CEGUI::WindowManager * w, CEGUI::String LayoutFileName, bool active): Menu(w,LayoutFileName, active){};
	virtual ~HistoriaGui();
	void RegisterForSpecificEvents(){};
};

#endif /* HISTORIAGUI_H_ */
