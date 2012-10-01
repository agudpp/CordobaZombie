/*
 * IState.cpp
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#include "IState.h"

#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>

#include "DebugUtil.h"
#include "CbMenuButton.h"
#include "tinyxml.h"
#include "MenuButtonEffect.h"


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
		OvEff::MenuButtonEffect &button) const
{
	ASSERT(xml);

	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();

	// check if we have the overlay
	Ogre::OverlayContainer *cont = static_cast<Ogre::OverlayContainer *>(
			om.getOverlayElement(xml->Attribute("OverlayName")));
#ifdef DEBUG
	if(!cont){
		debug("OverlayName %s no esta\n", xml->Attribute("OverlayName"));
		ASSERT(false);
	}
#endif

	// construct the button
	ASSERT(button.getButton() == 0);
	CbMenuButton *cbu = new CbMenuButton;

	// configure the button from the overlay
	cbu->configureAll(cont);
	button.setButton(cbu);

	// create the effect
	const TiXmlElement *effectXml = xml->FirstChildElement("Effect");
	if(!effectXml){
		debugWARNING("No effect set... we want this?\n");
		return;
	}

	// else we have an effect, getit from the builder
	// button.setEffect(OvEff::EffectBuilder(effectXml, cont);
	debugERROR("TODO: USar el builder de carlox\n");

}


////////////////////////////////////////////////////////////////////////////////
void IState::getVideoRangesFromXML(std::vector<VideoRange> &vr) const
{
	// get the TiXmlElement from the document with the associated name
	const TiXmlElement *elem = mXMLHelper.findElement(mName.c_str());
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
void IState::buildButtons(std::vector<OvEff::MenuButtonEffect> &buttons,
		const std::vector<Ogre::String> &names) const
{
	if(names.empty()) return;

	const TiXmlElement *elem = mXMLHelper.findElement(mName.c_str());
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
		OvEff::MenuButtonEffect button;
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
	        mXMLHelper.getRootElement(),
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
		mName(name)
{
	ASSERT(sEventCb);
}

IState::~IState()
{

}


void IState::getVideoRanges(std::vector<VideoRange> &vr) const
{
	getVideoRangesFromXML(vr);
}


}
