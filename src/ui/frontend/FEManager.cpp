/*
 * FEManager.cpp
 *
 *  Created on: Oct 28, 2013
 *      Author: agustin
 */

#include "FEManager.h"

#include <bitset>

namespace ui {


////////////////////////////////////////////////////////////////////////////////
FEManager::FEManager(const input::InputHelper& inputHelper,
                     const MouseCursor& cursor) :
    mEventInfo(inputHelper)
,   mMouseCursor(cursor)
{
    // build the matrix size
    mMatrix.construct(core::AABB(1.f,0.f,0.f,1.f));
}
////////////////////////////////////////////////////////////////////////////////
FEManager::~FEManager()
{
}

////////////////////////////////////////////////////////////////////////////////
void
FEManager::add(FEElement* element)
{
    ASSERT(element);
    if (contains(element)) {
        return; // nothing to do
    }

    // add it
    element->_elemIndex = mElements.size();
    mElements.push_back(element);

    // put it into the matrix, note that we are duplicating the pointer
    // since we hold the element in the mElements and in the matrix too.
    //
    core::StackVector<ElementsCell*, NUM_ROWS*NUM_COLS> cells;
    mMatrix.getCells(element->dimensions(), cells);
    for (ElementsCell** beg = cells.begin(), **end = cells.end(); beg != end; ++beg) {
        (*beg)->elements.push_back(element);
    }

    // set this instance as the manager
    element->setManager(this);
}
////////////////////////////////////////////////////////////////////////////////
void
FEManager::remove(FEElement* element)
{
    ASSERT(element);
    if (!contains(element)) {
        return; // nothing to do
    }

    // remove it from the current elements vector and the matrix
    mElements.disorder_remove(element->_elemIndex);
    if (!mElements.empty()) {
        mElements[element->_elemIndex]->_elemIndex = element->_elemIndex;
    }

    // from matrix
    core::StackVector<ElementsCell*, NUM_ROWS*NUM_COLS> cells;
    mMatrix.getCells(element->dimensions(), cells);
    for (ElementsCell** beg = cells.begin(), **end = cells.end(); beg != end; ++beg) {
        (*beg)->elements.remove(element);
    }

    // check if the element is in the lastVector, this could be causing some
    // nasty problems! but the probability is very low.
    //
    bool found = false;
    for (unsigned int i = 0; i < mLastElements.size() && !found; ++i) {
        if (mLastElements[i] == element) {
            mLastElements.disorder_remove(i);
            found = true;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void
FEManager::updateDimensions(FEElement* element, const core::AABB& newDims)
{
    ASSERT(element);
    ASSERT(contains(element));

    // we can improve this using a bitset getting the "common" indices for the
    // cells we want to remove the element and the cells that we will put the
    // element. Check the Collision 2D system that we are doing almost the same
    // than here
    debugOPTIMIZATION("We can improve this method using the above commented method\n");
    core::StackVector<ElementsCell*, NUM_ROWS*NUM_COLS> cells;
    mMatrix.getCells(element->dimensions(), cells);
    for (ElementsCell** beg = cells.begin(), **end = cells.end(); beg != end; ++beg) {
        (*beg)->elements.remove(element);
    }
    element->mAABB = newDims;
    mMatrix.getCells(element->dimensions(), cells);
    for (ElementsCell** beg = cells.begin(), **end = cells.end(); beg != end; ++beg) {
        (*beg)->elements.push_back(element);
    }

    // do some ugly thing here, to check if the element was in the last vector,
    // and in such case we have to verify if the object is still inside, if not
    // we have to remove it.
    //
    bool found = false;
    for (unsigned int i = 0; i < mLastElements.size() && !found; ++i) {
        if (mLastElements[i] == element &&
            !element->checkPointInside(mLastMousePos)) {
            mLastElements.disorder_remove(i);
            found = true;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void
FEManager::setActive(FEElement* element, bool active)
{
    ASSERT(element);

    // for now we will just put the element or remove the element from the manager
    if (active) {
        add(element);
    } else {
        remove(element);
    }
    element->mIsActive = active;
}

////////////////////////////////////////////////////////////////////////////////
void
FEManager::update(void)
{
    // get the current mouse position
    mEventInfo.mousePos = core::Vector2(mMouseCursor.getXRelativePos(),
                                        1.f - mMouseCursor.getYRelativePos());

    // if the mouse is outside of the matrix we cannot do anything, the objects
    // will be maintained as selected
    //
    if (!mMatrix.isPositionInside(mEventInfo.mousePos)) {
        return;
    }

    // check if the mouse has moved
    if (mLastMousePos == mEventInfo.mousePos) {
        // no, it doesn't move
        // NOTE: probably if some of the element has changed its dimensions we are
        //       not detecting here that condition

        // call all the last elements to update its mouseOver state
        for (FEElement** beg = mLastElements.begin(), **end = mLastElements.end();
                beg != end; ++beg) {
            FEElement* elem = *beg;
            ASSERT(elem);
            elem->mouseOver(mEventInfo);
        }
        return;
    }

    // yes the mouse has moved, update the lastMousePos.
    mLastMousePos = mEventInfo.mousePos;

    // now we have to check if some of the last element is outside of the
    // mouse position
    //
    std::bitset<MAX_NUM_ELEMENTS> elementsInside;
    for (unsigned int i = 0; i < mLastElements.size(); ++i) {
        FEElement* elem = mLastElements[i];
        ASSERT(elem);
        if (!elem->checkPointInside(mEventInfo.mousePos)) {
            elem->mouseOut(mEventInfo);

            // remove it from the mLastElement
            mLastElements.disorder_remove(i);
            --i; // decrease one to check it again
        } else {
            // the element is still inside
            elementsInside[elem->_elemIndex] = true;
        }
    }

    // now we have to get all the elements that are in the current mouse position
    //
    core::StackVector<FEElement*, NUM_ROWS*NUM_COLS> elements;
    if (!getAllIntersectingElements(mEventInfo.mousePos, elements)) {
        // nothing to do.
        return;
    }

    // else we have some elements and we need to check if there are new or
    // if are old elements
    for (FEElement** beg = elements.begin(), **end = elements.end();
            beg != end; ++beg) {
        FEElement* elem = *beg;
        ASSERT(elem);

        // check if it is a new one
        if (!elementsInside[elem->_elemIndex]) {
            // is a new one
            elem->mouseIn(mEventInfo);
            mLastElements.push_back(elem);
        } else {
            // the element was already "selected"
            elem->mouseOver(mEventInfo);
        }
    }
}

} /* namespace ui */
