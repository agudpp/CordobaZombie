/*
 * Text2D.cpp
 *
 *  Created on: 09/09/2012
 *      Author: agustin
 *
 */

#include "Text2D.h"



namespace f_e {

////////////////////////////////////////////////////////////////////////////////
void Text2D::truncateHorizontally(Ogre::String &input,
		Ogre::Real w,
		Ogre::Real height)
{
	ASSERT(mTextArea);
	const Ogre::Real width = (w > 0.0f) ? w : mTextArea->getWidth();
	debugRED("Width container: %f\n", mTextArea->getWidth());
	ASSERT(Ogre::FontManager::getSingleton().getByName(
			mTextArea->getFontName()).get());
	Ogre::Font *font = static_cast<Ogre::Font *>(
			Ogre::FontManager::getSingleton().getByName(
					mTextArea->getFontName()).get());
	const float viewportAspectCoef =
				Ogre::OverlayManager::getSingleton().getViewportAspectRatio();


	mTextArea->_update();

	// start to subdivide the text into lines
	const unsigned int size = input.length();
	unsigned int lastNewLine = 0,
			actualPosition = 0,
			lineCounts = 0,
			lastSpace = 0;
	Ogre::Real accumWidth = 0.0f;

	debugGREEN("%s\n", input.c_str());


	debugRED("Width container: %f, Width Text: %f, aspectRatio: %f\n",
			width,
			getStringWidth(input) * width,
			viewportAspectCoef);
	for(unsigned int i = 0; i < size; ++i){
		if(input[i] == '\n'){
			++lineCounts;
			lastNewLine = i;
			lastSpace = actualPosition = i + 1;
			accumWidth = 0.0f;
			continue;
		}
		accumWidth += (getCharWidth(input[i], font, viewportAspectCoef) * width);
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

	// set the width
	mTextArea->setWidth(width);
}

// Given the dimension (wxh), truncate the text to the width and scale the font to fit the height
void Text2D::adjustTextToContainer(Ogre::String &input, Ogre::Real w, Ogre::Real h)
{
	ASSERT(mTextArea);

	const Ogre::Real width  = (w > 0.0f) ? w : mTextArea->getWidth();
        const Ogre::Real height = (h > 0.0f) ? h : mTextArea->getHeight();

	debugRED("Width container: %f\n", mTextArea->getWidth());

	ASSERT(
               Ogre::FontManager::getSingleton().getByName(mTextArea->getFontName()).get()
              );

	Ogre::Font *font = static_cast<Ogre::Font *>(
           Ogre::FontManager::getSingleton().getByName(
	        mTextArea->getFontName()
                                                      ).get()
                                                    );

	const float viewportAspectCoef =
	    Ogre::OverlayManager::getSingleton().getViewportAspectRatio();

	mTextArea->_update();

	// start to subdivide the text into lines
	const unsigned int size = input.length();

	unsigned int lastNewLine       = 0,
			actualPosition = 0,
			lineCounts     = 0,
			lastSpace      = 0;

	Ogre::Real accumWidth = 0.0f;

	debugGREEN("%s\n", input.c_str());

	debugRED("Width container: %f, Width Text: %f, Height container: %f, Height Text: %f, aspectRatio: %f\n",
			width,
			getStringWidth(input) * width,
                        height,
                        mTextArea->getCharHeight(),
			viewportAspectCoef);

	for(unsigned int i = 0; i < size; ++i)
        {
		if(input[i] == '\n')
                {
			++lineCounts;
			lastNewLine = i;
			lastSpace = actualPosition = i + 1;
			accumWidth = 0.0f;
			continue;
		} 

		accumWidth += (getCharWidth(input[i], font, viewportAspectCoef) * width);

		if(input[i] == ' ' || input[i] == '\t')
                {
			lastSpace = i;
			continue;
		}

                // Check if we have exceeded or gone under the height
                if((lineCounts*mTextArea->getCharHeight()) != height && lineCounts != 0)
                {
                    // Scale text
                    // First find lineCounts to height ratio
                    Ogre::Real lineHeightRatio = height/static_cast<Ogre::Real>(lineCounts);

                    debugGREEN("lineHeightRatio: %f", lineHeightRatio);


                    // Second, set the height to the difference of the height and how much it is bigger
                    mTextArea->setCharHeight(lineHeightRatio); 

                    //This changes all of our previous calculations (some text might go over the width!)
                    // We need to restart the loop to account for these changes
                    i = 0;
                    lineCounts = lastNewLine = lastSpace = actualPosition = 0;
                    accumWidth = 0.0f;
                    continue; // Loop!
                }

		// check if we have exceed the width
		if(accumWidth > width) 
                {
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

	// set the width
	mTextArea->setWidth(width);

        // Set the height
        mTextArea->setHeight(height);
}

// Given the text (and accounting for line breaks) we find the max width and max height 
// of the input and use those dimensions to shape the Container.
void Text2D::adjustContainerToText(Ogre::String &input, Ogre::Real w, Ogre::Real h)
{
	ASSERT(mTextArea);

	const Ogre::Real width  = (w > 0.0f) ? w : mTextArea->getWidth();
        const Ogre::Real height = (h > 0.0f) ? h : mTextArea->getHeight();

	debugRED("Width container: %f\n", mTextArea->getWidth());

	ASSERT(
               Ogre::FontManager::getSingleton().getByName(mTextArea->getFontName()).get()
              );

	Ogre::Font *font = static_cast<Ogre::Font *>(
           Ogre::FontManager::getSingleton().getByName(
	        mTextArea->getFontName()
                                                      ).get()
                                                    );

	const float viewportAspectCoef =
	    Ogre::OverlayManager::getSingleton().getViewportAspectRatio();

	mTextArea->_update();

	// start to subdivide the text into lines
	const unsigned int size = input.length();

	unsigned int lastNewLine       = 0,
			actualPosition = 0,
			lineCounts     = 0,
			lastSpace      = 0;

	Ogre::Real accumWidth = 0.0f, maxAccumWidth;

	debugGREEN("%s\n", input.c_str());

	debugRED("Width container: %f, Width Text: %f, Height container: %f, Height Text: %f, aspectRatio: %f\n",
			width,
			getStringWidth(input) * width,
                        height,
                        mTextArea->getCharHeight(),
			viewportAspectCoef);

	for(unsigned int i = 0; i < size; ++i)
        {
		if(input[i] == '\n')
                {
			++lineCounts;
			lastNewLine = i;
			lastSpace = actualPosition = i + 1;
			accumWidth = 0.0f;
			continue;
		} 

		accumWidth += (getCharWidth(input[i], font, viewportAspectCoef) * width);

                // Find our biggest width so far...
                if(accumWidth > maxAccumWidth)
                    maxAccumWidth = accumWidth; 

		if(input[i] == ' ' || input[i] == '\t')
                {
			lastSpace = i;
			continue;
		}
	}

	debugRED("lineCounts: %d\n", lineCounts);
	debugBLUE("%s\n", input.c_str());

        // Our calculated dimensions are (wxh respectively) maxAccumWidth and lineCounts*charHeight

	// set the width
	mTextArea->setWidth(maxAccumWidth);

        // Set the height
        const Ogre::Real maxHeight = lineCounts*mTextArea->getCharHeight();
        mTextArea->setHeight(maxHeight);
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
void Text2D::configure(const Ogre::String &str,
						ConfigType type,
						Ogre::Real width,
						Ogre::Real height)
{
	ASSERT(mTextArea);

	switch(type) {
	case NONE:
		mTextArea->setCaption(str);
		// resize
		mTextArea->setHeight((height > 0.0f) ? height : mTextArea->getHeight());
		break;
	case TRUNCATE_HORIZONTAL:
	{
		Ogre::String result = str;
		truncateHorizontally(result, width, height);
		mTextArea->setCaption(result);
	}
	break;
	case ADJUST_TEXT_TO_CONTAINER:
	{
                Ogre::String result = str;
                adjustTextToContainer(result, width, height);
                mTextArea->setCaption(result);         
	}
	break;
	case ADJUST_CONTAINER_TO_TEXT:
	{
                Ogre::String result = str;
                adjustContainerToText(result, width, height);
                mTextArea->setCaption(result);     
	}
	break;

	default:
		ASSERT(false);
	}
}

}
