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

#include <cstring>  // strncasecmp()

#include "OverlayEffectBuilder.h"
#include "OverlayEffect.h"
#include "Alpha.h"
#include "Slide.h"


/**
 * Supported names list:
 *		names of the supported OverlayEffects, as they are to be found
 *		inside the XML configuration files (case insensitive)
 */
namespace {
	/* Anonymous namespace provides internal linkage. */
	const char nameAlpha[] = "alpha";
	const char nameSlide[] = "slide";
}



namespace OvEff {

#define  OVEFF_XMLNAME		"OverlayEffect"
#define  MAX_OVEFFNAME_LEN	(1<<5)  // 32 chars

////////////////////////////////////////////////////////////////////////////////
OverlayEffect*
OverlayEffectBuilder::createOverlayEffect(const TiXmlElement& xmle,
										  Ogre::String* eName)
{
	OverlayEffect* e(0);
	const char* eNameFound(xmle.Attribute("name"));

	/* Error checking */

	if (std::strncmp(xmle.Value(), OVEFF_XMLNAME, strlen(OVEFF_XMLNAME)) != 0) {
		debugWARNING("Non-OverlayEffect XML element received (\"%s\")\n", xmle.Value());
		return e;

	} else if (eName) {
		/* OverlayEffect name specified. */
		if (strncasecmp(eName->c_str(), eNameFound, MAX_OVEFFNAME_LEN) != 0) {
			debugWARNING("Specified OverlayEffect name \"%s\" didn't match "
						"the name found in the XML element (\"%s\")\n",
						eName->c_str(), eNameFound);
			return e;
		}
	}

	/* Build requested OverlayEffect. */

	if (strncasecmp(eNameFound, nameAlpha, strlen(nameAlpha)) == 0) {
		/* Alpha-fading overlay effect. */
		e = buildAlpha(xmle);

	} else if (strncasecmp(eNameFound, nameSlide, strlen(nameSlide)) == 0) {
		/* Sliding panel overlay effect. */
		e = buildSlide(xmle);

	} else {
		debugWARNING("Unsupported effect: \"%s\"\n", eNameFound);
	}

	return e;
}


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
	const TiXmlElement* wxml(0);

	if (!mHelper.hasOpenFile()) {
		debugWARNING("No XML file open!%s", "\n");
		return e;
	} else {
		wxml = mHelper.findElement(eName.c_str());
	}

	if (!mHelper.getRootElement()) {
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

	if(std::strncmp(txe.Value(), OVEFF_XMLNAME, strlen(OVEFF_XMLNAME)) != 0) {
		debugERROR("Non-OverlayEffect element received (\"%s\")\n", txe.Value());

	} else if (strncasecmp(effectName, nameAlpha, 6) == 0) {
		/* Alpha-fading overlay effect. */
		e = buildAlpha(txe);

	} else if (strncasecmp(effectName, nameSlide, 6) == 0) {
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

	/* <OverlayEffect name="Alpha" type="FADE_OUT" time="2.0"/> */
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

	/* <OverlayEffect name="Slide" orig="-0.1 0.5" dest="1.1 0.5" duration="3.7"/> */
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
