/* Wrapper to handle the atlas of a material (multi atlas).
 * MultiAtlasOverlay.h
 *
 *  Created on: 15/08/2012
 *      Author: agustin
 *
 */

#ifndef MULTIATLASOVERLAY_H_
#define MULTIATLASOVERLAY_H_


#include <OgrePanelOverlayElement.h>

#include "DebugUtil.h"
#include "GUIHelper.h"

class MultiAtlasOverlay {
public:
	MultiAtlasOverlay(Ogre::PanelOverlayElement *p = 0) :
	    mCont(p)
	,   mNumCols(0)
	,   mNumRows(0)
	{}
	~MultiAtlasOverlay()
	{}

	/**
	 * Set/Get the container to be wrapped
	 */
	inline Ogre::PanelOverlayElement *getPanel(void);
	inline void setPanel(Ogre::PanelOverlayElement *p);

	/**
	 * Set the number of images we have in the material associated (this is
	 * the number of atlas images we have)
	 * @param	iprow		Number of images per row
	 * @param	ipcolumn	Number of images per column
	 */
	inline void setNumberImages(int iprow, int ipcolumn);

	/**
	 * @brief Configure the Multiatlas to be used only for uv calculation
	 * @param materialName  The material name we want to use to configure this
     * @param   iprow       Number of images per row
     * @param   ipcolumn    Number of images per column
	 */
	inline void configureAtlasForCalculus(const Ogre::String &materialName,
	                                      size_t ipRow,
	                                      size_t ipColumn);

	/**
	 * Change the atlas position
	 * @param	row		The row to be selected
	 * @param	column	The column to be selected
	 */
	inline void changeAtlas(int row, int column);

    /**
     * @brief Select a new image from a index where the indices are given this way
     *        0   1   2   3   4
     *        5   6   7   8   9
     *        10  11  12  13  14..
     * @param index     The index of the image we want to use
     */
	inline void changeAtlas(size_t index);

	/**
	 * @brief Get the uv coordinates from an index or indices
	 * @param index | row,col   The glbal index or the row and column to get the
	 *                          uv
	 * @param u0,u1,v0,v1       The resulting uv coordinates
	 */
	inline void getUVs(size_t row, size_t column,
	                   Ogre::Real &u0, Ogre::Real &v0,
	                   Ogre::Real &u1, Ogre::Real &v1) const;
    inline void getUVs(size_t index,
                       Ogre::Real &u0, Ogre::Real &v0,
                       Ogre::Real &u1, Ogre::Real &v1) const;

private:
	Ogre::PanelOverlayElement		*mCont;
	float							mAtlasFactorX;
	float							mAtlasFactorY;
	size_t mNumRows;
	size_t mNumCols;
};


////////////////////////////////////////////////////////////////////////////////
inline Ogre::PanelOverlayElement *
MultiAtlasOverlay::getPanel(void)
{
	return mCont;
}

////////////////////////////////////////////////////////////////////////////////
inline void
MultiAtlasOverlay::setPanel(Ogre::PanelOverlayElement *p)
{
	ASSERT(p);
	mCont = p;
}

////////////////////////////////////////////////////////////////////////////////
inline void
MultiAtlasOverlay::setNumberImages(int iprow, int ipcolumn)
{
	ASSERT(mCont);
	ASSERT(iprow > 0);
	ASSERT(ipcolumn > 0);

	int textSizex = GUIHelper::getTextureWidth(mCont->getMaterialName());
	mAtlasFactorX = static_cast<Ogre::Real>(textSizex) /
	    static_cast<Ogre::Real>(ipcolumn);
	mAtlasFactorX = mAtlasFactorX / static_cast<Ogre::Real>(textSizex);

	int textSizey = GUIHelper::getTextureHeight(mCont->getMaterialName());
	mAtlasFactorY = static_cast<Ogre::Real>(textSizey) /
	    static_cast<Ogre::Real>(iprow);
	mAtlasFactorY = mAtlasFactorY / static_cast<Ogre::Real>(textSizey);

    mNumRows = iprow;
    mNumCols = ipcolumn;
}

////////////////////////////////////////////////////////////////////////////////
inline void
MultiAtlasOverlay::configureAtlasForCalculus(const Ogre::String &materialName,
                                             size_t iprow,
                                             size_t ipcolumn)
{
    ASSERT(mCont == 0);

    int textSizex = GUIHelper::getTextureWidth(materialName);
    mAtlasFactorX = static_cast<Ogre::Real>(textSizex) /
        static_cast<Ogre::Real>(ipcolumn);
    mAtlasFactorX = mAtlasFactorX / static_cast<Ogre::Real>(textSizex);

    int textSizey = GUIHelper::getTextureHeight(materialName);
    mAtlasFactorY = static_cast<Ogre::Real>(textSizey) /
        static_cast<Ogre::Real>(iprow);
    mAtlasFactorY = mAtlasFactorY / static_cast<Ogre::Real>(textSizey);

    mNumRows = iprow;
    mNumCols = ipcolumn;
}

////////////////////////////////////////////////////////////////////////////////
inline void
MultiAtlasOverlay::changeAtlas(int row, int column)
{
	ASSERT(mCont);
	ASSERT(column < mNumCols);
	ASSERT(row < mNumRows);
	const Ogre::Real x1 = (mAtlasFactorX * column);
	const Ogre::Real y1 = (mAtlasFactorY * row);
	mCont->setUV(x1, y1, x1+mAtlasFactorX, y1+mAtlasFactorY);
}

inline void
MultiAtlasOverlay::changeAtlas(size_t index)
{
    const std::size_t row = index / mNumCols;
    const std::size_t col = index - (mNumCols * row);
    changeAtlas(row, col);
}

inline void
MultiAtlasOverlay::getUVs(size_t row, size_t column,
                          Ogre::Real &u0, Ogre::Real &v0,
                          Ogre::Real &u1, Ogre::Real &v1) const
{
    ASSERT(column < mNumCols);
    ASSERT(row < mNumRows);
    u0 = (mAtlasFactorX * column);
    v0 = (mAtlasFactorY * row);
    u1 = u0 + mAtlasFactorX;
    v1 = v0 + mAtlasFactorY;
}
inline void
MultiAtlasOverlay::getUVs(size_t index,
                          Ogre::Real &u0, Ogre::Real &v0,
                          Ogre::Real &u1, Ogre::Real &v1) const
{
    const std::size_t row = index / mNumCols;
    const std::size_t col = index - (mNumCols * row);
    getUVs(row, col, u0, v0, u1, v1);
}


#endif /* MULTIATLASOVERLAY_H_ */
