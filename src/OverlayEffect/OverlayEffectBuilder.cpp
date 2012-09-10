/*
 * OverlayEffectBuilder.cpp
 *
 *  Helper module.
 *  Abstraction of an OverlayEffect creation from an XML file.
 *
 *  Created on: Sep 9, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */


#include "OverlayEffectBuilder.h"
#include "OverlayEffect.h"
#include "Alpha.h"
#include "Slide.h"


namespace OvEff {

////////////////////////////////////////////////////////////////////////////////
bool
OverlayEffectBuilder::setFilename(const Ogre::String& fname)
{
	if (mHelper.hasOpenFile()) {
		mHelper.closeXml();
	}

	mHelper.setFilename(fname);
	mHelper.openXml();

	if (mHelper.hasOpenFile()) {
		return true;
	} else {
		debugERROR("Error while trying to open file \"%s\"\n", fname.c_str());
		return false;
	}
}


////////////////////////////////////////////////////////////////////////////////
OverlayEffect*
OverlayEffectBuilder::createOverlayEffect(const Ogre::String &eName)
{
	OverlayEffect* e(0);
	const TiXmlElement* wxml(mHelper.findElement(eName.c_str()));

	if (mHelper.getRootElement()) {
		debugERROR("Couldn't get the XML root element.%s", "\n");
	} else if (!wxml) {
		debugERROR("OverlayEffect named \"%s\" couldn't be found in the current"
				" XML file.\n", eName.c_str());
	} else {
		e = buildOverlayEffect(*wxml);
	}

	return e;
}


////////////////////////////////////////////////////////////////////////////////
OverlayEffect*
OverlayEffectBuilder::buildOverlayEffect(const TiXmlElement& txe)
{
	OverlayEffect* e(0);
	const char* effectName(txe.Attribute("name"));

	if(std::strncmp(txe.Value(), "Effect", 7) != 0) {
		debugERROR("Non-effect element received (\"%s\")\n", txe.Value());

	} else if (std::strncmp(effectName, "Alpha", 6) == 0) {
		/* Alpha-fading overlay effect. */
		e = buildAlpha(txe);

	} else if (std::strncmp(effectName, "Slide", 6) == 0) {
		/* Sliding panel overlay effect. */
		e = buildSlide(txe);

	} else {
		debugERROR("Unsupported effect: \"%s\"\n", effectName);
	}

	return e;
}


////////////////////////////////////////////////////////////////////////////////
Alpha*
OverlayEffectBuilder::buildAlpha(const TiXmlElement& txe)
{
	Alpha* a(0);
	Alpha::Type type;

	/* <Effect name="Alpha" type="FADE_OUT" time="2.0"/> */
	ASSERT(txe.Attribute("type"));
	ASSERT(txe.Attribute("time"));

	if (!std::strncmp(txe.Attribute("type"), "FADE_IN", 8)) {
		type = Alpha::FADE_IN;
	} else if (!std::strncmp(txe.Attribute("type"), "FADE_OUT", 9)) {
		type = Alpha::FADE_OUT;
	} else {
		debugERROR("Unsupported alpha fading type: \"%s\"\n", txe.Attribute("type"));
		return a;
	}

	a = new Alpha;
	a->setType(type);
	a->setTime(Ogre::StringConverter::parseReal(txe.Attribute("time")));
	ASSERT(a);

	/* TODO: what about the attached OverlayElement?
	 *		 Is there a way to pass it through the XML file? */

	return a;
}


////////////////////////////////////////////////////////////////////////////////
Slide*
OverlayEffectBuilder::buildSlide(const TiXmlElement& txe)
{
	Slide* s(0);

	/* <Effect name="Slide" orig="-0.1 0.5" dest="1.1 0.5" duration="3.7"/> */
	ASSERT(txe.Attribute("orig"));
	ASSERT(txe.Attribute("dest"));
	ASSERT(txe.Attribute("duration"));

	s = new Slide;
	s->setTranslationPositions(
			Ogre::StringConverter::parseVector2(txe.Attribute("orig")),
			Ogre::StringConverter::parseVector2(txe.Attribute("dest")));
	s->setDuration(Ogre::StringConverter::parseReal(txe.Attribute("duration")));
	ASSERT(s);

	/* TODO: what about the transition velocity function?
	 *		 Is there a way to pass it through the XML file? */

	return s;
}



}
