/*
 * Text2D.cpp
 *
 *  Created on: 09/09/2012
 *      Author: agustin
 *
 */

#include "Text2D.h"



namespace gui_utils {

Text2D::Text2D(Ogre::TextAreaOverlayElement *txtArea) :
		mTextArea(txtArea)
{
	ASSERT(txtArea);
}

Text2D::~Text2D()
{

}

void Text2D::configure(const Ogre::String &str, ConfigType type)
{
	ASSERT(mTextArea);

	switch(type) {
	case NONE:
		mTextArea->setCaption(str);
		break;
	case TRUNCATE_HORIZONTAL:
	{

	}
	break;
	case ADJUST_TEXT_TO_CONTAINER:
	{

	}
	break;
	case ADJUST_CONTAINER_TO_TEXT:
	{

	}
	break;

	default:
		ASSERT(false);
	}
}

}
