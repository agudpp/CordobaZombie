/*
 * FEManager.h
 *
 *  Created on: Oct 28, 2013
 *      Author: agustin
 */

#ifndef FEMANAGER_H_
#define FEMANAGER_H_

#include <list>

#include <cursor/MouseCursor.h>
#include <debug/DebugUtil.h>
#include <input/InputHelper.h>
#include <space_partition/StaticMatrixPartition.h>
#include <types/StackVector.h>

#include "FEElement.h"
#include "FEInfo.h"


namespace ui {

class FEManager
{
    // We will use a limited number of element for now, this could be changed
    // later if we want, but it will be faster if we know the maximum number
    // of element at the beginning
    //
    static const unsigned int MAX_NUM_ELEMENTS = 50;

    // Matrix sizes definition
    //
    static const unsigned int NUM_ROWS = 4;
    static const unsigned int NUM_COLS = 4;

    // We will define the cell used to hold the elements
    //
    struct ElementsCell {
        // TODO: use StackList instead? but using a common allocator for all
        //       the lists of the different cells
        std::list<FEElement*> elements;
    };

public:
    // @brief To use this class we will need the inputHelper used by the engine
    //        as well as the MouseCursor.
    //
    FEManager(const input::InputHelper& inputHelper,
              const MouseCursor& cursor);
    ~FEManager();

    // @brief Add / check if exists / remove an element of this manager.
    // @param element       The element we want to add / check / remove.
    //
    void
    add(FEElement* element);
    inline bool
    contains(const FEElement* element) const;
    void
    remove(FEElement* element);

    // @brief This method is called when an element changes its dimension.
    // @param element       The element that this manager already have and
    //                      its dimensions had changed.
    //
    void
    updateDimensions(FEElement* element, const core::AABB& newDims);

    // @brief Set an object active / inactive.
    // @param element       The element we want to change its state.
    // @param active        The state we want to set {active, inactive}.
    //
    void
    setActive(FEElement* element, bool active);

    // @brief Update all the internal logic and call all the elements with the
    //        new associated events.
    //
    void
    update(void);

private:

    // @brief Get all the elements that intersect a certain point
    // @param point         The point to check against
    // @param elements      The list of all the elements that intersect the point
    // @return true if some element intersects | false otherwise
    //
    inline bool
    getAllIntersectingElements(const core::Vector2& point,
                               core::StackVector<FEElement*, NUM_ROWS*NUM_COLS>& elements);


private:
    FEInfo mEventInfo;
    const MouseCursor& mMouseCursor;
    core::StaticMatrixPartition<ElementsCell, NUM_COLS, NUM_ROWS> mMatrix;
    core::StackVector<FEElement*, MAX_NUM_ELEMENTS> mElements;
    core::StackVector<FEElement*, MAX_NUM_ELEMENTS> mLastElements;
    core::Vector2 mLastMousePos;
};






////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//


inline bool
FEManager::getAllIntersectingElements(const core::Vector2& point,
                                      core::StackVector<FEElement*,
                                                        NUM_ROWS*NUM_COLS>& elements)
{
    ASSERT(mMatrix.isPositionInside(point));

    elements.clear();

    const ElementsCell& cell = mMatrix.getCell(point);
    for (auto it = cell.elements.begin(), end = cell.elements.end(); it != end; ++it) {
        if ((*it)->checkPointInside(point)) {
            elements.push_back(*it);
        }
    }
    return !elements.empty();
}


inline bool
FEManager::contains(const FEElement* element) const
{
    ASSERT(element);
    return (element->_elemIndex < mElements.size()) &&
        (mElements[element->_elemIndex] == element);
}

} /* namespace ui */
#endif /* FEMANAGER_H_ */
