/*
 * MainMenuHelper.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: agustin
 */

#include "MainMenuHelper.h"

#include <OgreOverlayContainer.h>
#include <OgrePanelOverlayElement.h>

#include <debug/DebugUtil.h>
#include <math/Vec2.h>
#include <ogre_utils/OgreUtil.h>
#include <frontend/element/button/FESimpleButton.h>
#include <frontend/FEInfo.h>
#include <types/StackVector.h>


namespace cz {

namespace MainMenuHelper
{
////////////////////////////////////////////////////////////////////////////////
bool
configureButton(ui::FESimpleButton& button,
                Ogre::Overlay* overlay,
                const char* name)
{
    if (overlay == 0 || name == 0) {
        debugERROR("Invalid argument passed. Overlay or name null\n");
        return false;
    }

    // get the overlay container from that name
    Ogre::PanelOverlayElement* panel =
        static_cast<Ogre::PanelOverlayElement*>(overlay->getChild(name));
    if (panel == 0) {
        debugERROR("No panel found with name %s\n", name);
        return false;
    }

    // get the size of the texture
    core::Vector2 texSize;
    if (!core::OgreUtil::getTextureSize(panel->getMaterial().get(), texSize)) {
        debugERROR("Error getting the texture size for panel %s\n", name);
        return false;
    }

    // create the coordinates now needed for the buttons
    core::StackVector<ui::UVCoord, ui::FESimpleButton::ATLAS_SIZE> atlas;
    const float hstep = 1.f/4.f;
    for (unsigned int i = 0; i < ui::FESimpleButton::ATLAS_SIZE; ++i) {
        const float fi = static_cast<float>(i);
        atlas.push_back(ui::UVCoord(0, 1.f, fi*hstep, (fi+1.f)*hstep));
    }

    // configure the button
    button.build(panel, atlas);

    // everything fine
    return true;
}

}

} /* namespace cz */
