/*
 * OpcionesGui.h
 *
 *  Created on: Jan 7, 2012
 *      Author: Emi
 */

#ifndef OPCIONESGUI_H_
#define OPCIONESGUI_H_

#include "Menu.h"

class OpcionesGui: public Menu {

public:
	OpcionesGui(CEGUI::WindowManager * w, CEGUI::String LayoutFileName, bool active): Menu(w,LayoutFileName, active){};
	virtual ~OpcionesGui();
	void RegisterForSpecificEvents(){};
};

#endif /* OPCIONESGUI_H_ */
