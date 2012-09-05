/*
 * CreditosGui.h
 *
 *  Created on: 17/12/2011
 *      Author: Emi
 */

#ifndef CREDITOSGUI_H_
#define CREDITOSGUI_H_

#include "Menu.h"

class CreditosGui: public Menu {

public:
	CreditosGui(CEGUI::WindowManager * w, CEGUI::String LayoutFileName, bool active): Menu(w,LayoutFileName, active){};
	virtual ~CreditosGui();
	void RegisterForSpecificEvents(){};
};

#endif /* CREDITOSGUI_H_ */
