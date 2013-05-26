/*
 * PlayerStatus.cpp
 *
 *  Created on: 18/06/2012
 *      Author: agustin
 */

#include "PlayerStatus.h"

#include "DebugUtil.h"
#include "GUIHelper.h"
#include "LoadingBar.h"


namespace hud {


////////////////////////////////////////////////////////////////////////////////
PlayerStatus::BarUpdater::BarUpdater() :
unit(0),
bigBar(0)
{

}

////////////////////////////////////////////////////////////////////////////////
PlayerStatus::BarUpdater::~BarUpdater()
{

}

////////////////////////////////////////////////////////////////////////////////
void PlayerStatus::BarUpdater::operator()(int)
{
	ASSERT(unit);
	loadingBar.setActualValue(unit->getLife());

	// if we have the big bar then we will update it too
	if(bigBar){
		bigBar->setActualValue(unit->getLife());
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void PlayerStatus::rightPressed(void)
{
	if(mCallback) (*mCallback)(this);
}

////////////////////////////////////////////////////////////////////////////////
void PlayerStatus::leftPressed(void)
{
	debugWARNING("Por ahora no estamos soportando el boton derecho del mouse..."
			"capaz que sirva para mostrar info mas detallada del player?\n");
}

////////////////////////////////////////////////////////////////////////////////
PlayerStatus::PlayerStatus() :
mCallback(0)
{
}

////////////////////////////////////////////////////////////////////////////////
PlayerStatus::~PlayerStatus()
{
    f_e::GUIHelper::fullDestroyOverlayElement(mCont);
	if(mBarUpdater.unit)	mBarUpdater.unit->clearLifeCallback();
}

/////////////////////////////////////////////////////////////////////////////////
void PlayerStatus::configure(PlayerUnit *gu, Ogre::OverlayContainer *oc,
		Callback *cb, f_e::LoadingBar *bigB)
{
	ASSERT(gu);
	ASSERT(oc);
	ASSERT(cb);
	ASSERT(bigB);
	ASSERT(!mCont);

	mCallback = cb;

	// configure the Unit
	mBarUpdater.unit = gu;
	mBarUpdater.unit->setLifeCallback(&mBarUpdater);

	// Configure all the overlays
	// first we will configure the button using this oc
	configureAll(oc);

	// Configure the loading bar
	Ogre::OverlayContainer *cont = f_e::GUIHelper::findContainer(oc, "LifeBar");
	if(!cont){
		debugERROR("El contenedor no tiene un sub container nombre LifeBar\n");
		ASSERT(false);
	}
	// use this container as the loading bar
	mBarUpdater.loadingBar.setOverlayContainer(cont);

	debugWARNING("Aca estamos seteando la vida maxima del player cuando en "
			"realidad deberia ser una constante predefinida en un archivo x\n");
	mBarUpdater.loadingBar.setMaximumValue(mBarUpdater.unit->getLife());
	mBarUpdater.loadingBar.setActualValue(mBarUpdater.unit->getLife());

}

////////////////////////////////////////////////////////////////////////////////
void PlayerStatus::show(void)
{
	// enable button activity
	setVisible(true);
	setEnable(true);
}
////////////////////////////////////////////////////////////////////////////////
void PlayerStatus::hide(void)
{
	// remove the MenuButton from button manager
	setEnable(false);
	setVisible(false);
}

/**
 * Advise to this playerStatus that is the "one" to show the Main Life bar
 */
void PlayerStatus::useMainLifeBar(void)
{
	debugWARNING("Aca deberiamos setear el MaxmumValue para la bar, porque si "
			"tenemos distinta cantidad de vida para cada player esto no alcanza\n");
	ASSERT(mBarUpdater.bigBar);
	mBarUpdater.bigBar->setActualValue(mBarUpdater.unit->getLife());
}

}
