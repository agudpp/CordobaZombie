/*
 * Gui.h
 *
 *  Created on: 28/05/2011
 *      Author: Emi
 */

#ifndef GUI_H_
#define GUI_H_

#include "CEGUI.h"
#include "CEGUIEvent.h"
#include "CEGUIOgreRenderer.h"
#include "CEGUIWindowManager.h"
#include "OIS.h"
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include <time.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include "MainGui.h"


using namespace std;

class Gui {

	private:
		CEGUI::Window* myRoot;
		CEGUI::Window *principal;
		CEGUI::System * sys;
		CEGUI::OgreRenderer * myRendererPtr;
		CEGUI::DefaultResourceProvider* rp;
		CEGUI::FrameWindow* fWnd;
		CEGUI::WindowManager* wMgrPtr;

		Menu *menuPrincipal;



		OIS::Mouse * mouse;


		int num;
		int w, h;
		bool menu2, presionado;
		void RegisterForEvents();



	public:
		Gui(OIS::Mouse * m);
		~Gui();
		int load();
		void execute();
		void unload();
		void useMouse();
		void cargarImagen(const string &imagen, const string &nombre);
		void descargarImagen(const string &nombre);

};

#endif /* GUI_H_ */
