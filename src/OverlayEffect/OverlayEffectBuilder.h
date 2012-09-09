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

namespace OvEff {

class OverlayEffectBuilder
{
public:
	/**
	 * @brief	Constructor/destructor
	 * @remark	Destructor closes any open XML file.
	 */
	inline OverlayEffectBuilder();
	inline ~OverlayEffectBuilder();

	/**
	 * @brief	Sets the filename where effects configurations are to be found.
	 * @remark	On sucess, the file is left open.
	 *			Any other file already in use gets closed.
	 *			The file is expected to have the effects arranged by names.
	 * @param	fname	File name
	 * @return	true	Specified filename exists, configuration sucessfull.
	 * 			false	No file by the name "fname" could be found.
	 */
	bool
	setFilename(const Ogre::String& fname);

	/**
	 * @brief	Creates a new OverlayEffect from given name
	 * @param	eName	Effect name
	 * @return	e		Newly allocated OverlayEffect (memory owned by caller)
	 * 					0 on error.
	 */
	OverlayEffect*
	createOverlayEffect(const Ogre::String &eName);

private:
	/**
	 * @brief	Creates a new OverlayEffect from an XML element.
	 * @param	txe		TinyXmlElement from which to extract config. details.
	 * @return	e		Newly allocated OverlayEffect (memory owned by caller)
	 * 					0 on error.
	 */
	OverlayEffect*
	overlayEffectBuilder(const TinyXmlElement* txe);

private:
	XMLHelper	mHelper;
};


/******************************************************************************/
/****************************     INLINES     *********************************/


////////////////////////////////////////////////////////////////////////////////
inline
OverlayEffectBuilder::OverlayEffectBuilder()
{
	/* Default constructor suffices. */
}

////////////////////////////////////////////////////////////////////////////////
inline
OverlayEffectBuilder::~OverlayEffectBuilder()
{
	/* Default destructor suffices. */
}


}


#endif /* OVERLAYEFFECTBUILDER_H_ */
