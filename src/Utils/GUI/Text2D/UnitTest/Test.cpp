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
#include <OgreResourceManager.h>

#include "GlobalObjects.h"
#include "Test.h"



static double gettimestamp(void)
{
	struct timeval now;

	if(gettimeofday(&now, 0) < 0)
		return 0.0;

	double nowDouble = static_cast<double>(now.tv_sec + (
						static_cast<double>(now.tv_usec)/1000000.0));

	return nowDouble;
}

bool Test::performMouseRay(Ogre::Vector3 &v)
{
	return false;
}

void Test::createLevelManager(void)
{

}



Test::Test()
{

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
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	Ogre::Overlay *ov = om.getByName("MainMenu/Credits");
	ASSERT(ov);
	ov->show();

	Ogre::OverlayElement *cont = static_cast<Ogre::OverlayElement *> (
			om.getOverlayElement("Credits/Background"));
	ASSERT(cont);
	cont->hide();

	// UGLY BUG FIX
	Ogre::ResourceManager::ResourceMapIterator iter = Ogre::FontManager::getSingleton().getResourceIterator();
	while (iter.hasMoreElements()) { iter.getNext()->load(); }

	mTextArea = reinterpret_cast<Ogre::TextAreaOverlayElement *>(
			ov->getChild("Credits/Text"));
	ASSERT(mTextArea);
	mTextArea->show();

	debugRED("Width container: %f\n", mTextArea->getWidth());

	Ogre::String str("Esto es una prueba de una cosa muy larrga pero sin enters, "
						"vamos a ver cuantos enters va a meter y donde chota los va a meter"
						" puede ser que los ponga bien o puede ser que los ponga como el chori"
						" lo que es capaz que mas probable");
	mTextArea->setCaption(str);

	debugRED("Width container: %f\n", mTextArea->getWidth());

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	handleInput();

	static bool kp = false;

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
		if (!kp){
			kp = true;
			debugRED("Width container: %f\n", mTextArea->getWidth());
			gui_utils::Text2D t2d(mTextArea);
			debugRED("Width container: %f\n", mTextArea->getWidth());
			Ogre::String str("Esto es una prueba de una cosa muy larrga pero sin enters, "
					"vamos a ver cuantos enters va a meter y donde chota los va a meter"
					" puede ser que los ponga bien o puede ser que los ponga como el chori"
					" lo que es capaz que mas probable");
			t2d.configure(str, gui_utils::Text2D::TRUNCATE_HORIZONTAL);

		}
	} else {
		kp = false;
	}


}
