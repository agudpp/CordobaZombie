/*
 * OverlayEffectBuilder.h
 *
 *  Created on: Sep 9, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef OVERLAYEFFECTBUILDER_H_
#define OVERLAYEFFECTBUILDER_H_

#include "XMLHelper.h"
#include "OverlayEffect.h"


namespace f_e {


/* FWD references to supported OverlayEffects.
 * Their names must be reflected in the "Supported names list" (see .cpp) */
class Alpha;
class Slide;


class OverlayEffectBuilder
{
	/**
	 * @remark	Default constructor/destructor are sufficient.
	 *			Destructor closes any open XML file.
	 */

public:
	/**
	 * @brief	Creates a new OverlayEffect from given XML element.
	 * @remark	Param eName matching is case-insensitive.
	 * @param	xmle	XML element from which to extract data.
	 * 			eName	If not NULL, 'eName' specifies the name of the effect
	 * 					detailed in 'xmle' (v.gr. Alpha, Slide, etc.)
	 * @return	e		Newly allocated OverlayEffect (memory owned by caller)
	 * 					0 on error.
	 */
	static OverlayEffect*
	createOverlayEffect(const TiXmlElement& xmle, Ogre::String* eName);

	/**
	 * @brief	Stub to createOverlayEffect(xmle, eName)
	 */
	inline OverlayEffect*
	operator()(const TiXmlElement& xmle, Ogre::String* eName);


/************************************************************************
 * NOTE: Following functionality opens an XML file for parsing.			*
 *		 If the XML file to extract data from is already open,			*
 *		 use the function createOverlayEffect( xmle, eName) instead.	*/
public:
	/**
	 * @brief	Sets the name of the XML file where the effects configurations
	 *			are to be extracted from.
	 * @remark	fname must be available to Ogre's ResourceGroupManager.
	 *			On sucess, the file is left open.
	 *			Any other file already in use gets closed.
	 * @param	fname	File name (must be an available resource)
	 * @return	true	Specified filename exists, configuration sucessfull.
	 * 			false	Errors occurred while trying to open file "fname".
	 */
	bool
	setFilename(const Ogre::String& fname);

	/**
	 * @brief	Does the instance have a currently open XML file?
	 * @return	true if there's an open file attached to the instance
	 *			false otherwise
	 */
	inline bool
	hasOpenFile() const;

	/**
	 * @brief	Creates a new OverlayEffect from given name
	 * @param	eName	Effect name (v.g. Alpha, Slide, etc.)
	 * @return	e		Newly allocated OverlayEffect (memory owned by caller)
	 * 					0 on error.
	 */
	OverlayEffect*
	createOverlayEffect(const Ogre::String& eName);

private:
	/**
	 * @brief	Creates a new OverlayEffect from an XML element.
	 * @param	txe		TiXmlElement from which to extract config. details.
	 * @return	e		Newly allocated OverlayEffect (memory owned by caller)
	 * 					0 on error.
	 */
	static OverlayEffect*
	buildOverlayEffect(const TiXmlElement& txe);

	/**
	 * @brief	Creates a new Alpha overlay effect from an XML element.
	 * @param	txe		TiXmlElement from which to extract config. details.
	 * @return	e		Newly allocated OverlayEffect (memory owned by caller)
	 * 					0 on error.
	 */
	static Alpha*
	buildAlpha(const TiXmlElement& txe);

	/**
	 * @brief	Creates a new Slide overlay effect from an XML element.
	 * @param	txe		TiXmlElement from which to extract config. details.
	 * @return	e		Newly allocated OverlayEffect (memory owned by caller)
	 * 					0 on error.
	 */
	static Slide*
	buildSlide(const TiXmlElement& txe);

private:
	core::XMLHelper mHelper;
};


/******************************************************************************/
/****************************     INLINES     *********************************/


////////////////////////////////////////////////////////////////////////////////
inline OverlayEffect*
OverlayEffectBuilder::operator()(const TiXmlElement& xmle, Ogre::String* eName)
{
	return createOverlayEffect(xmle, eName);
}


////////////////////////////////////////////////////////////////////////////////
inline bool OverlayEffectBuilder::hasOpenFile() const
{
	return mHelper.hasOpenFile();
}


}


#endif /* OVERLAYEFFECTBUILDER_H_ */
