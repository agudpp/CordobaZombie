/*
 * MainMenuHelper.h
 *
 *  Created on: Nov 22, 2013
 *      Author: agustin
 */

#ifndef MAINMENUHELPER_H_
#define MAINMENUHELPER_H_


#include <OgreOverlay.h>

// forward
//
namespace ui {
class FESimpleButton;
}

namespace cz {

namespace MainMenuHelper
{

// @brief Configure a button instance from a given overlay and overlay name.
// @param button        The button to be configured.
// @param overlay       The overlay instance where the configuration is.
// @param name          The name of the overlay container to be used as the
//                      "configuration" of the button.
// @return true on success | false otherwise
//
bool
configureButton(ui::FESimpleButton& button,
                Ogre::Overlay* overlay,
                const char* name);

}

} /* namespace cz */
#endif /* MAINMENUHELPER_H_ */
