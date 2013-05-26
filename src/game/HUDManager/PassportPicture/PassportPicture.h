/*
 * PassportPicture.h
 *
 *  Created on: 16/06/2012
 *      Author: agustin
 */

#ifndef PASSPORTPICTURE_H_
#define PASSPORTPICTURE_H_

#include <OgreOverlay.h>
#include <OgrePanelOverlayElement.h>
#include <OgreString.h>

#include <GUI/AtlasOverlay/MultiAtlasOverlay.h>

#include "HudElement.h"


#define PASSPORT_PICTURE_CONT		"HUD/PSP/Passport"
#define PASSPORT_PICTURE_OVERLAY	"HUD/PassportPicture"

namespace hud {

class PassportPicture : public HudElement
{
public:
	PassportPicture();
	virtual ~PassportPicture();

	// Inherited functions
	//

	/**
	 * Load/Unload all the resources of the HudElement
	 */
	void load(void);
	void unload(void);

	/**
	 * Hide/Show the element
	 */
	void show(void);
	void hide(void);

    /**
     * @brief Hihglight the element
     * @param highlight If true then the element should highlight, if false
     *                  it turn off the highlight effect
     */
    virtual void highlight(bool highlight);

    // Own functions

	/**
	 * @brief Change the passport picture (set the texture name) and the number
	 *        of images per row and columns
	 * @param textName  The texture name
	 * @param numColumn The number of columns that the texture has
	 * @param numRows   The number of rows that the texture has
	 */
	void changeAtlasPicture(const Ogre::String &textName,
	                        size_t numColumn,
	                        size_t numRows);

	/**
	 * @brief Select new image from the atlas
	 * @param colNum    The columnNumber of the image we want to select
	 * @param rowNum    The row number of the image we want to select
	 */
	inline void selectPicture(size_t numColumn, size_t numRow);

	/**
	 * @brief Select a new image from a index where the indices are given this way
	 *        0   1   2   3   4
	 *        5   6   7   8   9
	 *        10  11  12  13  14..
	 * @param index     The index of the image we want to use
	 */
	inline void selectPicture(size_t index);

private:

	Ogre::PanelOverlayElement	*mPanel;
	f_e::MultiAtlasOverlay mAtlas;
};

// Inline implementations
//
inline void
PassportPicture::selectPicture(size_t numColumn, size_t numRow)
{
    mAtlas.changeAtlas(numRow, numColumn);
}
inline void
PassportPicture::selectPicture(size_t index)
{
    mAtlas.changeAtlas(index);
}

}
#endif /* PASSPORTPICTURE_H_ */
