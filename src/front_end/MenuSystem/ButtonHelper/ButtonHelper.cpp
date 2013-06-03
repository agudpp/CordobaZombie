#include "ButtonHelper.h"
#include "XMLHelper.h"
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include "DebugUtil.h"
#include "CbMenuButton.h"
#include "tinyxml.h"
#include "MenuButtonEffect.h"
#include "OverlayEffectBuilder.h"


namespace CbBHelper{

////////////////////////////////////////////////////////////////////////////////
void parseCbMenuButton(	const TiXmlElement *xml,
						f_e::MenuButtonEffect &button)
{
	ASSERT(xml);

	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();

	// check if we have the overlay
	Ogre::OverlayContainer *cont = static_cast<Ogre::OverlayContainer *>(
			om.getOverlayElement(xml->Attribute("OverlayName")));

	// construct the button
	ASSERT(button.getButton() == 0);
	CbMenuButton *cbu = new CbMenuButton;

	// configure the button from the overlay
	cbu->configureAll(cont);

	button.setButton(cbu);

    // create the effect
	const TiXmlElement *effectXml = xml->FirstChildElement("OverlayEffect");
    if(!effectXml){
    	debugWARNING("No OverlayEffect for this button... Do we want this?\n");
    } else {
    	// accept any available effect
    	f_e::OverlayEffectBuilder oeb;
    	f_e::OverlayEffect* oe = oeb.createOverlayEffect(*effectXml, NULL);
    	if (oe) {
    		button.setEffect(oe);
    		oe->setElement(cont);
    	}
    }

    return;
}






////////////////////////////////////////////////////////////////////////////////

/* @ buildButtons(...)
 *
 * __FUNCION__: por cada nombre de botón en 'names': busca su descripción en
 * 'root' (bajo la sección "CbMenuButtons"), crea el boton a partir de la misma,
 * y lo coloca en 'buttons'.
 *
 */

void buildButtons(const TiXmlElement root,
				  const std::vector<Ogre::String> &names,
				  std::vector<f_e::MenuButtonEffect> &buttons)
{
	if(names.empty()) return;

	// get the CBMenuButtons config if we have
	const TiXmlElement *elem = root.FirstChildElement("CbMenuButtons");
	ASSERT(elem);

	// now find all the names and buttons
	;
	for(int i = 0, size = names.size(); i < size; ++i){
		const TiXmlElement *bElem =
				core::XMLHelper::findChild(elem, names[i].c_str());

#ifdef DEBUG
		if(!bElem){
			debugWARNING("The button %s couldn't be found\n", names[i].c_str());
			ASSERT(false);
		}
#endif

		// create the button
		f_e::MenuButtonEffect button;
		parseCbMenuButton(bElem, button);
		buttons.push_back(button);
	}
}


} //namespace CbBHelper
