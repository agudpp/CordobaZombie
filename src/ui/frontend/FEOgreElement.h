/*
 * FEOgreElement.h
 *
 *  Created on: Oct 28, 2013
 *      Author: agustin
 */

#ifndef FEOGREELEMENT_H_
#define FEOGREELEMENT_H_

#include <OgrePanelOverlayElement.h>

#include <debug/DebugUtil.h>
#include <types/StackVector.h>

#include "FEElement.h"


namespace ui {

// Forward
//
class FEManager;

template <unsigned int NUM_ATLAS = 1>
class FEOgreElement : public FEElement
{
public:
    static const unsigned int ATLAS_SIZE = NUM_ATLAS;
public:
    FEOgreElement(FEManager* manager = 0, const core::AABB& aabb = core::AABB()) :
        FEElement(manager, aabb)
    ,   mContainer(0)
    {}

    virtual
    ~FEOgreElement()
    {}

    // @brief Build it from an overlay container and a texture uv coords elements.
    // @param cont      The overlay container to be used. Note that this will not
    //                  be removed by this class, only will be used.
    // @param coords    The uv coords to be used for this element.
    // @note This method will update the bounding box associated using the
    //       container dimensions. But if you change the container dimensions
    //       from outside this class you should call the setDimension method.
    //       The conversions needed for the AABB will be made to match the graphical
    //       representation of the element.
    //
    inline void
    build(Ogre::PanelOverlayElement* cont,
          const core::StackVector<UVCoord, NUM_ATLAS>& coords);

    // @brief Return the associated Overlay Container.
    //
    inline Ogre::PanelOverlayElement*
    overlayContainer(void);
    inline const Ogre::PanelOverlayElement*
    overlayContainer(void) const;

    // @brief Return the number of coordinates we use for this element.
    //
    inline unsigned int
    numAtlas(void) const;

    // @brief Set a new atlas to be used (using the pre-set uv coords).
    // @param index     The atlas (uvcoords) index that should be:
    //                  0 <= index < NUM_ATLAS
    //
    inline void
    setAtlasCoords(unsigned int index);

    // @brief Set this element to be visible or invisible.
    //        Note that this will also call setActive(visible).
    // @param visible    If it is visible
    //
    void
    setVisible(bool visible);
    inline bool
    isVisible(void) const;


    ////////////////////////////////////////////////////////////////////////////
    // Events
    //

    // @brief This event is called whenever the mouse get inside over this element.
    // @param info      The information of the mouse and other stuff.
    //
    virtual void
    mouseIn(const FEInfo& info) = 0;

    // @brief This event is called when the mouse gets out from the element (in
    //        the past call the element was over this element).
    // @param info      The information of the mouse and other stuff.
    //
    virtual void
    mouseOut(const FEInfo& info) = 0;

    // @brief Event called all the frames that the mouse is over the element.
    // @param info      The information about the mouse buttons and other stuff.
    //
    virtual void
    mouseOver(const FEInfo& info) = 0;

    // @brief Method called to catch general events like key pressing and other
    //        stuff.
    // @param info      The associated information
    //
    virtual void
    onEvent(const FEInfo& info) = 0;

protected:
    Ogre::PanelOverlayElement* mContainer;
    core::StackVector<UVCoord, NUM_ATLAS> mUVCoords;
};






////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

template <unsigned int NUM_ATLAS>
inline void
FEOgreElement<NUM_ATLAS>::build(Ogre::PanelOverlayElement* cont,
                                const core::StackVector<UVCoord, NUM_ATLAS>& coords)
{
    ASSERT(cont);
    ASSERT(coords.size() == NUM_ATLAS);

    mContainer = cont;
    mUVCoords = coords;

    // set the new dimensions
    ASSERT(cont->getTop() <= 1.f);
    ASSERT(cont->getLeft() <= 1.f);
    ASSERT(cont->getHeight() <= 1.f);
    ASSERT(cont->getWidth() <= 1.f);
    core::AABB dims(1.f - cont->getTop(),
                    cont->getLeft(),
                    1.f - cont->getTop() - cont->getHeight(),
                    cont->getLeft() + cont->getWidth());
    setDimensions(dims);

    // set the first atlas
    setAtlasCoords(0);
}

template <unsigned int NUM_ATLAS>
inline Ogre::PanelOverlayElement*
FEOgreElement<NUM_ATLAS>::overlayContainer(void)
{
    return mContainer;
}
template <unsigned int NUM_ATLAS>
inline const Ogre::PanelOverlayElement*
FEOgreElement<NUM_ATLAS>::overlayContainer(void) const
{
    return mContainer;
}

template <unsigned int NUM_ATLAS>
inline unsigned int
FEOgreElement<NUM_ATLAS>::numAtlas(void) const
{
    return NUM_ATLAS;
}

template <unsigned int NUM_ATLAS>
inline void
FEOgreElement<NUM_ATLAS>::setAtlasCoords(unsigned int index)
{
    ASSERT(index < NUM_ATLAS);
    ASSERT(mContainer);
    UVCoord& coords = mUVCoords[index];
    mContainer->setUV(coords.u0, coords.v0, coords.u1, coords.v1);
}

template <unsigned int NUM_ATLAS>
void
FEOgreElement<NUM_ATLAS>::setVisible(bool visible)
{
    ASSERT(mContainer);
    if (visible) {
        mContainer->show();
    } else {
        mContainer->hide();
    }
    setActive(visible);
}
template <unsigned int NUM_ATLAS>
inline bool
FEOgreElement<NUM_ATLAS>::isVisible(void) const
{
    ASSERT(mContainer);
    return mContainer->isVisible();
}

} /* namespace ui */
#endif /* FEOGREELEMENT_H_ */
