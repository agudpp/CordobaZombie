/*
 * Test.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreSceneNode.h>
#include <iostream>
#include <assert.h>
#include <OgreSceneQuery.h>
#include <OgreRay.h>
#include <OgreManualObject.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreString.h>

#include <auto_ptr.h>

#include "MenuButton.h"
#include "GlobalObjects.h"
#include "Test.h"
#include "Util.h"


#include "IMenu.h"

class TestMenu : public IMenu {
public:
	TestMenu(Ogre::PanelOverlayElement *panel)
	{
		p = panel;
	}
	~TestMenu()
	{

	}

	/**
	 * Function called when the mouse is inside of this menu
	 */
	void mouseInside()
	{
//		std::cout << "MouseInside: " << name << std::endl;
		p->setUV(0.5,0,1,1);
	}

	/**
	 * Function called when the mouse goes outside from the menu
	 */
	void mouseOutside()
	{
		p->setUV(0,0,0.5,1);
//		std::cout << "mouseOutside: " << name << std::endl;
	}


	/**
	 * Function called when the mouse is moving inside of this menu
	 */
	void mouseMoving()
	{
//		std::cout << "mouseMoving: " << name << std::endl;
	}



	std::string 	name;
	Ogre::PanelOverlayElement *p;
};


// create overlay uv test
void Test::createOverlay(void)
{
	// load the fade
	mFader = 0;
	Ogre::MaterialPtr		mFaderMaterial;
	Ogre::TextureUnitState 	*mTexture = 0;

	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

	mFader = static_cast<Ogre::PanelOverlayElement*>(
		overlayManager.createOverlayElement("Panel", "Fader"));
	mFader->setMetricsMode(Ogre::GMM_RELATIVE);
	mFader->setPosition(0, 0);
	mFader->setDimensions(0.25f, 0.25f);
	mFader->setMaterialName("BotonTest"); // Optional background material

	// Ensures that the material exists
	mFader->setUV(0,0,0.5,1);

	// show the fade
	mFader->show();

	// Create an overlay, and add the panel
	Ogre::Overlay			*mOverlay = 0;
	mOverlay = overlayManager.create("FaderOverlay");
	mOverlay->add2D(mFader);
	mOverlay->show();
}


Test::Test() :
		mGuiManager(&mMenuManager)
{
	mMouseCursor.setImage("cursor.png");
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight());

	mMenuManager.build(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight(),
			5,5);

}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}

// handle input
void Test::handleInput(void)
{

}


/* Load additional info */
void Test::loadAditionalData(void)
{
//	createOverlay();
	std::vector<GUIBuilder::GUIObjectsInfo> objs;

	std::auto_ptr<TiXmlDocument> doc(Common::Util::loadXmlDocument("GUIInfo.xml"));
	ASSERT(doc.get());

	ASSERT(mGUIBuilder.load(doc.get()->RootElement(), objs));
	ASSERT(objs.size() >  0);

	for(int i = 0; i < objs.size(); ++i){
		mGuiManager.addObject(objs[i].obj, objs[i].type == "Static");
	}



}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	handleInput();
	mMenuManager.update();


}
