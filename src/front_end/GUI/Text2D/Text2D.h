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
#include <OgreOverlayManager.h>
#include <OgreMath.h>

#include "DebugUtil.h"

namespace f_e {

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

		// Adjust the size of the text only horizontally, so the text will be
		// adjusted horizontally but not vertically (the size of the font will
		// be increased or decreased but the text will not change -no \n will be
		// added for example-)
		ADJUST_TEXT_TO_CONTAINER_H,

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
	 * @param	width	The width that we want to be used (-1 to not change)
	 * @param	height	The height to be used (-1 to avoid it)
	 *
	 * @note	The TextArea must be configured before calling this function.
	 * 			We will use all the configuration established before in the
	 * 			TextArea
	 */
	void configure(const Ogre::String &str,
					ConfigType type,
					Ogre::Real width = -1.0f,
					Ogre::Real height = -1.0f);


private:
	/**
	 * Get the width of a string
	 */
	inline Ogre::Real getStringWidth(const Ogre::String &str) const;
	inline Ogre::Real getSubStringWidth(const Ogre::String &str,
			unsigned int from,	// string index (begin position)
			unsigned int to,	// string index (end position)
			const Ogre::Font *font) const;

	inline Ogre::Real getCharWidth(uint32_t c, const Ogre::Font *font,
				                   const Ogre::Real aspectRatio) const;

	/**
	 * Truncate horizontally the text (the text result will fits in the container
	 * horizontally after the call of this function).
	 */
	void truncateHorizontally(Ogre::String &text, Ogre::Real width, Ogre::Real height);

	/**
	 * Given a container's width and height, truncate the text to fit the new width
         * and scales the font to fit the new height.
	 */
	void adjustTextToContainer(Ogre::String &text, Ogre::Real width, Ogre::Real height);

	/**
	 * Given the text, mold the Container to the resulting dimensions of the text.
         * The dimensions are calculated from line breaks (increasing height) and length of 
         * the longest line's subtext.
	 */
	void adjustContainerToText(Ogre::String &text, Ogre::Real width, Ogre::Real height);


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
					mTextArea->getFontName()).get());
	ASSERT(font != 0);

	Ogre::Real result = 0.0f;
	const float viewportAspectCoef =
			Ogre::OverlayManager::getSingleton().getViewportAspectRatio();
/*	const Ogre::Real space = font->getGlyphAspectRatio(0x0020) * viewportAspectCoef;

	const int len = str.length();

	for(unsigned int i = 0; i < len; i++) {
		if (str[i] == 0x0020)   result += space;
		else result += font->getGlyphAspectRatio(str[i]);
	}

	result *= viewportAspectCoef;
	return result;*/

	Ogre::Real fHeight, fAspectCoef, fWidth = 0;
	fHeight = mTextArea->getCharHeight();
	Ogre::Real fAspectRatio;
	for (unsigned int iChar=0; iChar < str.length(); ++iChar) {
	  fAspectRatio = font->getGlyphAspectRatio(str[iChar]);
	  fWidth += fAspectRatio*fHeight;
	}
	return fWidth;
}

inline Ogre::Real Text2D::getSubStringWidth(const Ogre::String &str,
		unsigned int from,	// string index (begin position)
		unsigned int to,	// string index
		const Ogre::Font *font) const
{
	ASSERT(font);

	Ogre::Real result = 0.0f;
	const float viewportAspectCoef =
			Ogre::OverlayManager::getSingleton().getViewportAspectRatio();
	const Ogre::Real space = font->getGlyphAspectRatio(0x0020) * viewportAspectCoef;

	ASSERT(from <= to);
	ASSERT(to < str.length());

	for(; from < to; from++) {
		if (str[from] == 0x0020)   result += space;
		else result += font->getGlyphAspectRatio(str[from]);
	}

	result *= viewportAspectCoef;
	return result;
}

inline Ogre::Real Text2D::getCharWidth(uint32_t c,
										const Ogre::Font *font,
										const Ogre::Real aspectRatio) const
{
//	debugBLUE("CharWidth: %c -> %f\n", c, font->getGlyphAspectRatio(c) * aspectRatio);
//	return font->getGlyphAspectRatio(c) * aspectRatio;
	return mTextArea->getCharHeight() * font->getGlyphAspectRatio(c);
}

}

#endif /* TEXT2D_H_ */
