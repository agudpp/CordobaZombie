/*
 * Text2D.h
 *
 *  Created on: 09/09/2012
 *      Author: agustin
 *
 */

#ifndef TEXT2D_H_
#define TEXT2D_H_

#include <OgreTextAreaOverlayElement.h>
#include <OgreOverlay.h>
#include <OgreString.h>
#include <OgreFont.h>
#include <OgreFontManager.h>

#include "DebugUtil.h"

namespace gui_utils {

class Text2D {
public:
	// Enumerate all the configuration types
	enum ConfigType {
		// No configuration, this is by default
		NONE = 0,

		// Truncate the strings horizontally (if the string exceeds the width
		// of the container, then we will split in multiple lines (but this
		// not ensure that vertically we can show all the text
		TRUNCATE_HORIZONTAL,

		// Decrement the size of the characters (characters height) to show all
		// the text in the container. This will split the string also if necessary
		ADJUST_TEXT_TO_CONTAINER,

		// Adjust the container to the text size (we will increment the size
		// of the container to ensure that we can show all the text)
		ADJUST_CONTAINER_TO_TEXT,
	};
public:
	/**
	 * The constructor takes the textArea to be used and formated
	 */
	Text2D(Ogre::TextAreaOverlayElement *txtArea);

	/**
	 * The destructor of this class will not free the memory
	 */
	~Text2D();

	/**
	 * Returns the TextAreaOverlayElement associated
	 */
	inline Ogre::TextAreaOverlayElement *getElement(void);

	/**
	 * Set the string to be used (using all the configuration established
	 * before
	 * @param	str		The string to be used
	 * @param	type	The configuration type we want to use
	 * @note	The TextArea must be configured before calling this function.
	 * 			We will use all the configuration established before in the
	 * 			TextArea
	 */
	void configure(const Ogre::String &str, ConfigType type);


private:
	/**
	 * Get the width of a string
	 */
	inline Ogre::Real getStringWidth(const Ogre::String &str) const;


private:
	Ogre::TextAreaOverlayElement		*mTextArea;

};

inline Ogre::TextAreaOverlayElement *Text2D::getElement(void)
{
	return mTextArea;
}


// PRIVATE
inline Ogre::Real Text2D::getStringWidth(const Ogre::String &str) const
{
	ASSERT(mTextArea);
	ASSERT(Ogre::FontManager::getSingleton().getByName(
			mTextArea->getFontName()).get());
	const Ogre::Font *font = static_cast<Ogre::Font *>(
			Ogre::FontManager::getSingleton().getByName(
					mTextArea->getFontName().get()));
	Ogre::Real result = 0.0f;
	const Ogre::Real fontSize = font->getSize();
	const Ogre::Real space = font->getGlyphAspectRatio(0x0030) * fontSize;

	const int len = str.length();
	for(unsigned int i = 0; i < len; i++) {
		if (str[i] == 0x0020)   result += space;
		else result += font->getGlyphAspectRatio(str[i]) * fontSize;
	}
	return result;
}

}

#endif /* TEXT2D_H_ */
