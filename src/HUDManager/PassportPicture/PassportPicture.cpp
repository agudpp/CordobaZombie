/*
 * PassportPicture.cpp
 *
 *  Created on: 16/06/2012
 *      Author: agustin
 */

#include <OgreOverlayManager.h>
#include <OgreTextureManager.h>
#include <OgreTechnique.h>
#include <OgrePass.h>

#include "DebugUtil.h"
#include "GUIHelper.h"
#include "PassportPicture.h"

PassportPicture::PassportPicture() :
mPanel(0)
{
	debugWARNING("TODO: Tenemos que agregar el nombre que figura al costado de la "
			"foto indicando quien se esta mostrando, por ahora hasta que no "
			"tengamos definida ni la fuente ni los tamanios ni esas cosas no "
			"vamos a mostrar nada\n");
}

PassportPicture::~PassportPicture()
{
	unload();
}


/**
 * Load/Unload all the resources of the HudElement
 */
void
PassportPicture::load(void)
{
	ASSERT(!mPanel);
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	Ogre::Overlay *overlay = om.getByName(PASSPORT_PICTURE_OVERLAY);
	ASSERT(overlay);
	overlay->show();

	mPanel = static_cast<Ogre::PanelOverlayElement *> (
			om.getOverlayElement(PASSPORT_PICTURE_CONT));
	ASSERT(mPanel);
	mAtlas.setPanel(mPanel);
	mPanel->show();
}
void
PassportPicture::unload(void)
{
	if(!mPanel) return;
	GUIHelper::fullDestroyOverlayElement(mPanel);
	mPanel = 0;
}

/**
 * Hide/Show the element
 */
void
PassportPicture::show(void)
{
	if(mPanel) mPanel->show();
}
void
PassportPicture::hide(void)
{
	if(mPanel) mPanel->hide();
}


void
PassportPicture::changeAtlasPicture(const Ogre::String &textName,
                                    size_t numColumn,
                                    size_t numRows)
{
    // first change the material
    ASSERT(mPanel);
    // Get texture
    Ogre::MaterialPtr matptr = mPanel->getMaterial();
    ASSERT(!matptr.isNull());
    ASSERT(matptr->getTechnique(0));
    ASSERT(matptr->getTechnique(0)->getPass(0));
    ASSERT(matptr->getTechnique(0)->getPass(0)->getTextureUnitState(0));
    matptr->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(
        textName);

    // configure the atlas
    mAtlas.setNumberImages(numColumn, numRows);
}

