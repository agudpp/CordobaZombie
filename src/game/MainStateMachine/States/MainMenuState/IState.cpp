/*
 * IState.cpp
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#include "IState.h"

#include <cstring>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>

#include "DebugUtil.h"
#include "CbMenuButton.h"
#include "tinyxml.h"
#include "MenuButtonEffect.h"
#include "OverlayEffectBuilder.h"


namespace mm_states {

	EventCallback *IState::sEventCb = 0;

////////////////////////////////////////////////////////////////////////////////
void IState::setStateMachineCb(EventCallback *cb)
{
	ASSERT(cb);
	sEventCb = cb;
}


////////////////////////////////////////////////////////////////////////////////
void IState::parseVideoRange(const TiXmlElement *xml, VideoRange &vr) const
{
	ASSERT(xml);
	XMLHelper::parseFloat(xml, "start", vr.start);
	XMLHelper::parseFloat(xml, "end", vr.end);
}

////////////////////////////////////////////////////////////////////////////////
void IState::parseCbMenuButton(const TiXmlElement *xml,
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

	// we will receive the events for this button
	cbu->setCallback(this);

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
void IState::getVideoRangesFromXML(std::vector<VideoRange> &vr) const
{
	// get the TiXmlElement from the document with the associated name
	const TiXmlElement *elem = XMLHelper::findChild(mRootElement, mName.c_str());

#ifdef DEBUG
	if (elem == 0) {
	    debugERROR("XML element not found %s\n", mName.c_str());
	}

#endif
	ASSERT(elem);

	vr.clear();
	const TiXmlElement *vrelem = elem->FirstChildElement("VideoRanges");
	ASSERT(vrelem);

	const TiXmlElement *auxElem = vrelem->FirstChildElement("Entering");
	ASSERT(auxElem);

	VideoRange auxVr;
	parseVideoRange(auxElem, auxVr);
	vr.push_back(auxVr);

	auxElem = vrelem->FirstChildElement("Looping");
	ASSERT(auxElem);
	parseVideoRange(auxElem, auxVr);
	vr.push_back(auxVr);

	auxElem = vrelem->FirstChildElement("Exiting");
	ASSERT(auxElem);
	parseVideoRange(auxElem, auxVr);
	vr.push_back(auxVr);
}



////////////////////////////////////////////////////////////////////////////////
void IState::getSoundsFromXML()
{
	const TiXmlElement *elem(getXmlElement());
	ASSERT(elem);
	elem = elem->FirstChildElement("Sounds");

	if(!elem) {
		debugWARNING("No sounds specified for the MainMenu state \"%s\"\n",
					mName.c_str());
		return;
	}

	// For each specified sound ...
	elem = elem->FirstChildElement("Sound");
	ASSERT(elem);
	while (elem) {
		// ... identify sound type ...
		Ogre::String name(elem->Attribute("name"));

		// ... and map it into the SoundFamilyTable.
		if (!strncmp(name.c_str(), "mouse_click", strlen("mouse_click"))) {
			name = elem->Attribute("filename");
			mSounds.addSounds(SS_MOUSE_CLICK, &name, 1);
			debugBLUE("Added mouse click (\"%s\") for %s\n",
					name.c_str(), mName.c_str());

		} else if (!strncmp(name.c_str(), "background", strlen("background"))) {
			name = elem->Attribute("filename");
			mSounds.addSounds(SS_BACKGROUND_MUSIC, &name, 1);
			debugBLUE("Added background music (\"%s\") for %s\n",
					name.c_str(), mName.c_str());
		}

		elem = elem->NextSiblingElement("Sound");
	}

	return;
}



////////////////////////////////////////////////////////////////////////////////
void IState::buildButtons(std::vector<f_e::MenuButtonEffect> &buttons,
		const std::vector<Ogre::String> &names)
{
	if(names.empty()) return;

	const TiXmlElement *elem = XMLHelper::findChild(mRootElement, mName.c_str());
	ASSERT(elem);

	// get the CBMenuButtons config if we have
	elem = elem->FirstChildElement("CbMenuButtons");
	ASSERT(elem);


	// now find all the names and buttons
	const int size = names.size();
	for(int i = 0; i < size; ++i){
		const TiXmlElement *bElem = XMLHelper::findChild(elem, names[i].c_str());

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

/**
 * Function which returns the actual TiXmlElement used by this IState.
 * Returns the tixmlelement which points to the root of this IState section
 * or 0 on error.
 */
const TiXmlElement *IState::getXmlElement(void) const
{
	return XMLHelper::findChild(
	        mRootElement,
	        mName.c_str(),
	        "name");
}


////////////////////////////////////////////////////////////////////////////////
void IState::stateFinish(Event e)
{
	ASSERT(sEventCb);
	(*sEventCb)(this, e);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

IState::IState(const Ogre::String &name) :
		mName(name),
		mRootElement(0)
{
	ASSERT(sEventCb != 0);
}

IState::~IState()
{

}


void IState::getVideoRanges(std::vector<VideoRange> &vr) const
{
	getVideoRangesFromXML(vr);
}


}
