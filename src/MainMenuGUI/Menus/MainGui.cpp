/*
 * MainGui.cpp
 *
 *  Created on: 17/12/2011
 *      Author: Emi
 */

#include "MainGui.h"



MainGui::MainGui(CEGUI::WindowManager *w, CEGUI::String LayoutFileName, bool active)
{
	wMgrPtr = w;
	layoutFileName = LayoutFileName;
	isMenuActive = active;
	animationsCreated = false;

	historia = new HistoriaGui(wMgrPtr, "historia.layout", false);
	opciones = new OpcionesGui(wMgrPtr, "opciones.layout", false);
	creditos = new CreditosGui(wMgrPtr, "creditos.layout", false);

}

MainGui::~MainGui() {
}

void MainGui::loadMenu()
{
	layout = CEGUI::WindowManager::getSingleton().loadWindowLayout(layoutFileName);
	name = layout->getName();
	wMgrPtr->getWindow("root")->addChildWindow(layout);
	mapaMenus[name]=this;



	historia->loadMenu();
	mapaMenus[historia->getName()]=historia;


	opciones->loadMenu();
	mapaMenus[opciones->getName()]=opciones;


	creditos->loadMenu();
	mapaMenus[creditos->getName()]=creditos;

	RegisterForSpecificEvents();

	createAnimations();
	RegisterForEvents();

}



void MainGui::RegisterForSpecificEvents()
{

	CEGUI::WindowManager::WindowIterator wIterator = CEGUI::WindowManager::getSingleton().getIterator();
	while(!wIterator.isAtEnd()){
		CEGUI::String st = wIterator.getCurrentKey();
		int pos = st.find_first_of("@",0);
		if(pos > -1){
			wIterator.getCurrentValue()->subscribeEvent(CEGUI::PushButton::EventMouseButtonDown,  CEGUI::Event::Subscriber(&MainGui::changeMenu,this));
		}
		wIterator++;
	}
}



void MainGui::unloadMenu()
{
	layout->destroy();
	isMenuActive=true;
}

bool MainGui::changeMenu(const CEGUI::EventArgs & e)
{
	Menu * aux;
	const CEGUI::MouseEventArgs& we = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::String senderID = we.window->getName();

	int pos = senderID.find_first_of("@",0);
	CEGUI::String currentMenu= senderID.substr(0,pos);
	CEGUI::String nextMenu= senderID.substr(pos+1,senderID.length());

	map<CEGUI::String, Menu*>::iterator it1, it2;

	it1 = mapaMenus.find(currentMenu);
	it2 = mapaMenus.find(nextMenu);

	aux = it1->second;
	aux->animateMenu();

	aux = it2->second;
	aux->animateMenu();

	return true;
}









