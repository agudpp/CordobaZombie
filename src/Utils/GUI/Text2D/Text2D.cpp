/*
 * Text2D.cpp
 *
 *  Created on: 09/09/2012
 *      Author: agustin
 *
 */

#include "Text2D.h"



namespace gui_utils {

////////////////////////////////////////////////////////////////////////////////
void Text2D::truncateHorizontally(Ogre::String &input)
{
	ASSERT(mTextArea);
	const Ogre::Real width = mTextArea->getWidth();
	debugRED("Width container: %f\n", mTextArea->getWidth());
	ASSERT(Ogre::FontManager::getSingleton().getByName(
			mTextArea->getFontName()).get());
	const Ogre::Font *font = static_cast<Ogre::Font *>(
			Ogre::FontManager::getSingleton().getByName(
					mTextArea->getFontName()).get());

	// start to subdivide the text into lines
	const unsigned int size = input.length();
	unsigned int lastNewLine = 0,
			actualPosition = 0,
			lineCounts = 0,
			lastSpace = 0;
	Ogre::Real accumWidth = 0.0f;

	debugGREEN("%s\n", input.c_str());


	debugRED("Width container: %f, Width Text: %f\n", width, getStringWidth(input));
	for(unsigned int i = 0; i < size; ++i){
		if(input[i] == '\n'){
			++lineCounts;
			lastNewLine = i;
			lastSpace = actualPosition = i + 1;
			accumWidth = 0.0f;
		}
		accumWidth += getCharWidth(input[i], font);
		if(input[i] == ' ' || input[i] == '\t') {
			lastSpace = i;
			continue;
		}
		// check if we have exceed the width
		if (accumWidth > width) {
			// we have to add a \n
			input[lastSpace] = '\n';
			i = lastSpace + 1;
			++lineCounts;
			accumWidth = 0.0f;
			lastNewLine = i;
		}

	}
	debugRED("lineCounts: %d\n", lineCounts);
	debugBLUE("%s\n", input.c_str());
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Text2D::Text2D(Ogre::TextAreaOverlayElement *txtArea) :
		mTextArea(txtArea)
{
	ASSERT(txtArea);
	debugRED("Width container: %f\n", mTextArea->getWidth());
}

////////////////////////////////////////////////////////////////////////////////
Text2D::~Text2D()
{

}

////////////////////////////////////////////////////////////////////////////////
void Text2D::configure(const Ogre::String &str, ConfigType type)
{
	ASSERT(mTextArea);

	switch(type) {
	case NONE:
		debugRED("Width container: %f\n", mTextArea->getWidth());
		mTextArea->setCaption(str);
		break;
	case TRUNCATE_HORIZONTAL:
	{
		Ogre::String result = str;
		truncateHorizontally(result);
		mTextArea->setCaption(result);
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
