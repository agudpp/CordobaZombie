/*
 * TriggerMatrix.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#include "TriggerMatrix.h"

#include <debug/DebugUtil.h>


// Helper stuff
//
namespace {

// @brief Calculate the bounding box for a given list of trigger zones
// @para zones  The list of zones
// @return the bounding box that contains all the zones
//
core::AABB
calculateBoundingBox(const std::vector<core::TriggerZone>& zones)
{
    ASSERT(!zones.empty());
    core::AABB result = zones.front().zone();
    for (core::size_t i = 1, size = zones.size(); i < size; ++i) {
        result.increaseToContain(zones[i].zone());
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////
// coloring helpers methods

static core::TriggerColor_t COLOR_BITS[TS_NUM_COLORS] = {
    (1 << 0), (1 << 1), (1 << 2), (1 << 3), (1 << 4), (1 << 5), (1 << 6), (1 << 7),
    (1 << 8), (1 << 9), (1 << 10), (1 << 11), (1 << 12), (1 << 13), (1 << 14), (1 << 15),
    (1 << 16), (1 << 17), (1 << 18), (1 << 19), (1 << 20), (1 << 21), (1 << 22), (1 << 23),
    (1 << 24), (1 << 25), (1 << 26), (1 << 27), (1 << 28), (1 << 29), (1 << 30),
    static_cast<core::TriggerColor_t>(1 << 31)
};

}

namespace core {

TriggerMatrix::TriggerMatrix() :
    mZoneElements(0)
,   mZoneElementSize(0)
{
    ASSERT(sizeof(COLOR_BITS)/sizeof(TriggerColor_t) == TS_NUM_COLORS);
}

TriggerMatrix::~TriggerMatrix()
{
    delete []mZoneElements;
}

bool
TriggerMatrix::build(const std::vector<TriggerZone>& zones,
                     std::vector<core::uint16_t>& ids,
                     core::size_t numRows,
                     core::size_t numCols)
{
    if (zones.empty()) {
        debugERROR("No zones given for constructing the matrix\n");
        return false;
    }

    // destroy the previous matrix if was already built
    destroy();

    // calculate the bounding box for this new set of zones and construct the
    // matrix
    const AABB boundingBox = calculateBoundingBox(zones);

    debugOPTIMIZATION("We can do a better effort here to save memory using a matrix "
        "of indices for example... or calculating the best size for the matrix\n");
    mMatrix.construct(numCols, numRows, boundingBox);

    ASSERT(mZoneElements == 0);

    mZoneElementSize = zones.size();
    mZoneElements = new ZoneElement[mZoneElementSize];

    // assign the zones for each element
    ids.clear();
    ids.resize(mZoneElementSize);
    for (core::size_t i = 0, size = zones.size(); i < size; ++i) {
        mZoneElements[i].zone = zones[i];
        mZoneElements[i].id = static_cast<core::uint16_t>(i);
        ids[i] = static_cast<core::uint16_t>(i);
    }

    // now set the ids for each cell
    const core::size_t numCells = numCols * numRows;
    for (core::size_t i = 0; i < numCells; ++i) {
        mMatrix.getCell(i).id = static_cast<core::uint16_t>(i);
    }

    // now we will put each zone in the matrix where should be
    std::vector<Cell *> cells;
    cells.reserve(mZoneElementSize); // just in case
    for (core::size_t i = 0; i < mZoneElementSize; ++i) {
        ZoneElement& zoneElement = mZoneElements[i];

        // get the cells that intersect with the current zone
        mMatrix.getCells(zoneElement.zone.zone(), cells);
        for (core::size_t j = 0, count = cells.size(); j < count; ++j) {
            ASSERT(cells[j]);
            // we will set the color later
            cells[j]->elements.push_back(CellElement(zoneElement));
        }
    }

    // now... for each cell, again, set a correct color (different) for each zone
    // in a particular cell
    for (core::size_t i = 0; i < numCells; ++i) {
        CellElementVec& elements = mMatrix.getCell(i).elements;
        if (elements.size() > TS_NUM_COLORS) {
            debugERROR("There are more zones in a cell than the number we support. "
                "We have %ul zones in the cell %ul and we only support %d\n",
                elements.size(), i, TS_NUM_COLORS);

            ids.clear();
            destroy();
            return false;
        }

        // if we are here then we can set a color to each zone
        for (core::size_t j = 0, count = elements.size(); j < count; ++j) {
            elements[j].color = COLOR_BITS[j];
        }
    }

    // everything ok
    return true;
}

} /* namespace core */
