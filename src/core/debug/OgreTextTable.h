/*
 * OgreTextTable.h
 *
 *  Created on: Aug 23, 2013
 *      Author: agustin
 */

#ifndef OGRETEXTTABLE_H_
#define OGRETEXTTABLE_H_

#include <OgreColourValue.h>
#include <OgreString.h>

#include <vector>
#include <memory>

#include <debug/DebugUtil.h>

#include "OgreText.h"

namespace core {

// @brief This class will represent a one column - multi row table, where each
//        row is an OgreText.
//

class OgreTextTable
{
public:
    OgreTextTable(){};
    ~OgreTextTable(){};

    // @brief Set the number of rows we want to have in the table
    // @param nr    The number of rows
    // @param ch    The char height for the text in each row
    // @param left  The starting x position (left)
    // @param top   The starting y position (top)
    // @param sbl   SpaceBetweenLines (= 0.005f)
    //
    inline void
    setNumRows(unsigned int nr,
               Ogre::Real ch,
               float left,
               float top,
               float sbl = 0.005f);

    // @brief Set the text for the row r, with an specific color.
    // @param r     The row number (that should be < count(rows)).
    // @param text  The text to show in row r.
    // @param color [optional] colour for this row
    //
    inline void
    setText(unsigned int r,
            const Ogre::String& text,
            const Ogre::ColourValue& color = Ogre::ColourValue::White);

    // @brief Change the color of an specific row r.
    // @param r     The row we want to change its color
    // @param c     The color we want to set
    //
    inline void
    setColor(unsigned int r, const Ogre::ColourValue& c);

    // @brief Return the number of rows / char height
    //
    inline unsigned int
    numberRows(void) const;
    inline Ogre::Real
    charHeight(void) const;

private:
    typedef std::shared_ptr<OgreText> OgreTextPtr; // we will use ptrs to avoid copying

    std::vector<OgreTextPtr> mTexts;
    Ogre::Real mCharHeight;
};


////////////////////////////////////////////////////////////////////////////////
// inline stuff
//

inline void
OgreTextTable::setNumRows(unsigned int nr,
                          Ogre::Real ch,
                          float left,
                          float top,
                          float sbl)
{
    ASSERT(ch > 0.f);
    mCharHeight = ch;

    // create the rows now
    mTexts.resize(nr);
    for (unsigned int i = 0; i < nr; ++i) {
        mTexts[i].reset(new OgreText());
        // set the position for this text
        mTexts[i]->setPos(left, top + ch * i + sbl);
    }
}

inline void
OgreTextTable::setText(unsigned int r,
                       const Ogre::String& text,
                       const Ogre::ColourValue& color)
{
    ASSERT(r < mTexts.size());
    ASSERT(mTexts[r].get() != 0);

    mTexts[r]->setColor(color);
    mTexts[r]->setText(text, mCharHeight);
}

inline void
OgreTextTable::setColor(unsigned int r, const Ogre::ColourValue& c)
{
    ASSERT(r < mTexts.size());
    ASSERT(mTexts[r].get() != 0);
    mTexts[r]->setColor(c);
}

inline unsigned int
OgreTextTable::numberRows(void) const
{
    return mTexts.size();
}
inline Ogre::Real
OgreTextTable::charHeight(void) const
{
    return mCharHeight;
}


} /* namespace core */
#endif /* OGRETEXTTABLE_H_ */
