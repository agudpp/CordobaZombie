/*
 * Menu.cpp
 *
 *  Created on: 17/12/2011
 *      Author: Emi
 */

#include "Menu.h"

Menu::Menu(CEGUI::WindowManager * w, CEGUI::String LayoutFileName, bool active) {
	wMgrPtr = w;
	layoutFileName = LayoutFileName;
	isMenuActive = active;
	animationsCreated = false;

}

Menu::~Menu() {

}


void Menu::unloadMenu()
{
	layout->destroy();
	isMenuActive=false;
}

void Menu::loadMenu()
{
	layout = CEGUI::WindowManager::getSingleton().loadWindowLayout(layoutFileName);
	name = layout->getName();
	wMgrPtr->getWindow("root")->addChildWindow(layout);
	layout->hide();

	createAnimations();
	RegisterForEvents();

}


void Menu::animateMenu()
{
	if(isMenuActive){

		affectorLeftPanelExit = animLeftPanelExit->createAffector("UnifiedAreaRect", "URect");
		affectorLeftPanelExit->createKeyFrame(0.0, "{{-0.00389999,0},{0.000752491,0},{0.149182,0},{1.21855,0}}");
		affectorLeftPanelExit->createKeyFrame(0.375, "{{-0.15389999,0},{0.000752491,0},{-0.000818,0},{1.21855,0}}", CEGUI::KeyFrame::P_QuadraticAccelerating);
		instanceLeftPanelExit = CEGUI::AnimationManager::getSingleton().instantiateAnimation(animLeftPanelExit);
		instanceLeftPanelExit->setTargetWindow(layout->getChild("BC"+name));


		affectorContainerExit = animContainerExit->createAffector("UnifiedAreaRect", "URect");
		affectorContainerExit->createKeyFrame(0.0, "{{0.150367,0},{0.000629028,0},{0.998019,0},{0.999056,0}}");
		affectorContainerExit->createKeyFrame(0.375, "{{0.998019,0},{0.000629028,0},{1.845671,0},{0.999056,0}}", CEGUI::KeyFrame::P_QuadraticAccelerating);
		instanceContainerExit = CEGUI::AnimationManager::getSingleton().instantiateAnimation(animContainerExit);
		instanceContainerExit->setTargetWindow(layout->getChild("WC"+name));


		instanceLeftPanelExit->start();
		instanceContainerExit->start();



		}
	else{

		affectorLeftPanelEnter = animLeftPanelEnter->createAffector("UnifiedAreaRect", "URect");
		affectorLeftPanelEnter->createKeyFrame(0.375, "{{-0.20389999,0},{0.000752491,0},{-0.000818,0},{1.21855,0}}");
		affectorLeftPanelEnter->createKeyFrame(0.75, "{{-0.00389999,0},{0.000752491,0},{0.149182,0},{1.21855,0}}", CEGUI::KeyFrame::P_QuadraticAccelerating);
		instanceLeftPanelEnter = CEGUI::AnimationManager::getSingleton().instantiateAnimation(animLeftPanelEnter);
		instanceLeftPanelEnter->setTargetWindow(layout->getChild("BC"+name));



		affectorContainerEnter = animContainerEnter->createAffector("UnifiedAreaRect", "URect");
		affectorContainerEnter->createKeyFrame(0.375, "{{0.998019,0},{0.000629028,0},{1.845671,0},{0.999056,0}}");
		affectorContainerEnter->createKeyFrame(0.75, "{{0.150367,0},{0.000629028,0},{0.998019,0},{0.999056,0}}", CEGUI::KeyFrame::P_QuadraticAccelerating);
		instanceContainerEnter = CEGUI::AnimationManager::getSingleton().instantiateAnimation(animContainerEnter);
		instanceContainerEnter->setTargetWindow(layout->getChild("WC"+name));

		layout->show();
		instanceLeftPanelEnter->start();
		instanceContainerEnter->start();


	}
}

void Menu::createAnimations()
{
	if(!animationsCreated){
		animLeftPanelEnter = CEGUI::AnimationManager::getSingleton().createAnimation(name +"PanelEnter");
		animLeftPanelEnter->setDuration(0.75f);
		animLeftPanelEnter->setReplayMode(CEGUI::Animation::RM_Once);


		animLeftPanelExit = CEGUI::AnimationManager::getSingleton().createAnimation(name +"PanelExit");
		animLeftPanelExit->setDuration(0.75f);
		animLeftPanelExit->setReplayMode(CEGUI::Animation::RM_Once);


		animContainerEnter = CEGUI::AnimationManager::getSingleton().createAnimation(name +"ContainerEnter");
		animContainerEnter->setDuration(0.75f);
		animContainerEnter->setReplayMode(CEGUI::Animation::RM_Once);


		animContainerExit = CEGUI::AnimationManager::getSingleton().createAnimation(name +"ContainerExit");
		animContainerExit->setDuration(0.75f);
		animContainerExit->setReplayMode(CEGUI::Animation::RM_Once);



		animationsCreated = true;
	}

}

void Menu::RegisterForEvents()
{
	layout->getChild("BC"+name)->subscribeEvent(CEGUI::AnimationInstance::EventAnimationEnded, CEGUI::Event::Subscriber(&Menu::imprimirTexto,this));

}

bool Menu::imprimirTexto(const CEGUI::EventArgs & e)
{

	if(isMenuActive){
		layout->hide();
		isMenuActive = false;
	}
	else{
		isMenuActive=true;

	}
	return true;
}

bool Menu::nombreLayout(const CEGUI::EventArgs & e)
{
	std::cout << "nombre: " << layout->getName() << std::endl;
	return true;
}


