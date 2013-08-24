/*
 * TriggerMatrix.h
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#ifndef TRIGGERMATRIX_H_
#define TRIGGERMATRIX_H_

#include <space_partition/MatrixPartition.h>
#include <types/basics.h>

#include "TriggerSystemDefines.h"
#include "TriggerZone.h"

namespace core {

class TriggerMatrix
{
public:
    // Define a cell to be used in the Matrix and the elements in the cell
    //
    struct ZoneElement {
        TriggerCallback::Signal callbacks;  // the associated callback of this element
        TriggerZone zone;                   // the associated zone to this element
        core::uint16_t id;                  // the zone id
    };

    struct CellElement {
        CellElement(const ZoneElement& z, TriggerColor_t c = 0) :
            zoneElement(z)
        ,   color(c)
        {}

        const ZoneElement& zoneElement;
        TriggerColor_t color;       // the associated color for this element
    };

    typedef std::vector<CellElement> CellElementVec;
    struct Cell {
        core::uint16_t id; // cell id
        CellElementVec elements;

        // @brief Helper method used to get all the elements that intersect
        //        a certain position
        // @param position  The position we will use to check the intersection
        // @param result  The resulting elements
        //
        inline void
        getElementsInPosition(const Vector2& position,
                              std::vector<const CellElement *>& result) const;

        // @brief look for a CellElement by zone.id. If we found it we return
        //        the pointer to the associated CellElement, otherwise 0
        //        is returned
        //
        inline const CellElement*
        findElementByZoneID(core::uint16_t zoneID) const;

    };

public:
    TriggerMatrix();
    ~TriggerMatrix();

    // @brief Construct the matrix from a given list of TriggerZones and
    //        the dimensions to use (if we want).
    //        TODO: probably here we should calculate the best sizes for the
    //        matrix automatically
    // @param zones     The zones to be used
    // @param ids       The ids for the given zones
    // @param numRows   [optional] The number of rows to be used
    // @param numCols   [optional] The number of columns to be used
    // @return  true on success | false otherwise
    // @note You can safely remove the zones after calling this method
    //
    bool
    build(const std::vector<TriggerZone>& zones,
          std::vector<core::uint16_t>& ids,
          core::size_t numRows = 10,
          core::size_t numCols = 10);

    // @brief Destroy everything
    //
    inline void
    destroy(void);

    // @brief Check if the matrix is already built
    //
    inline bool
    isBuilt(void) const;

    // @brief Get the bounding box used by this class (the bounding box that
    //        contains all the used zones)
    //
    inline const AABB&
    boundingBox(void) const;

    // @brief Get a cell from a given position.
    // @param pos   The position to be used
    // @return the associated MatrixCell
    //
    inline const Cell&
    getCell(const Vector2& pos) const;
    inline Cell&
    getCell(const Vector2& pos);

    // @brief Get a cell from a given id.
    // @param id   The index of the cell to be used
    // @return the associated MatrixCell
    //
    inline const Cell&
    getCell(core::uint16_t id) const;
    inline Cell&
    getCell(core::uint16_t id);

    // @brief Check if a cell id is valid
    // @param id   The index of the cell to be checked
    //
    inline bool
    isIndexValid(core::uint16_t id) const;

    // @brief Check if a position is inside the matrix or not
    // @param position  The position to be checked
    //
    inline bool
    isPositionInside(const Vector2& position) const;

    // @brief Get a ZoneElement from an ID (the id should be the same than
    //        the order given when built the matrix
    //
    inline ZoneElement&
    getZoneElement(core::uint16_t id);

    // @brief Get the memory used by this class
    //
    core::size_t
    getMemory(void) const;

private:

private:
    MatrixPartition<Cell> mMatrix;
    ZoneElement* mZoneElements;
    core::size_t mZoneElementSize;
};


////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
TriggerMatrix::Cell::getElementsInPosition(const Vector2& position,
                                           std::vector<const CellElement *>& result) const
{
    result.clear();
    for (core::size_t i = 0, size = elements.size(); i < size; ++i) {
        if (elements[i].zoneElement.zone.isPointInside(position)) {
            result.push_back(&(elements[i]));
        }
    }
}

inline const TriggerMatrix::CellElement*
TriggerMatrix::Cell::findElementByZoneID(core::uint16_t zoneID) const
{
    // TODO: improve here if we sort the elements by zone.id (issue #12 github)
    for (core::size_t i = 0, size = elements.size(); i < size; ++i) {
        if (elements[i].zoneElement.id == zoneID) {
            return &(elements[i]);
        }
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

inline void
TriggerMatrix::destroy(void)
{
    mMatrix.destroy();
    delete []mZoneElements; mZoneElements = 0;
    mZoneElementSize = 0;
}

inline bool
TriggerMatrix::isBuilt(void) const
{
    return mZoneElements != 0;
}


inline const AABB&
TriggerMatrix::boundingBox(void) const
{
    return mMatrix.boundingBox();
}

inline const TriggerMatrix::Cell&
TriggerMatrix::getCell(const Vector2& pos) const
{
    return mMatrix.getCell(pos);
}
inline TriggerMatrix::Cell&
TriggerMatrix::getCell(const Vector2& pos)
{
    return mMatrix.getCell(pos);
}
inline const TriggerMatrix::Cell&
TriggerMatrix::getCell(core::uint16_t id) const
{
    return mMatrix.getCell(id);
}
inline TriggerMatrix::Cell&
TriggerMatrix::getCell(core::uint16_t id)
{
    return mMatrix.getCell(id);
}
inline bool
TriggerMatrix::isIndexValid(core::uint16_t id) const
{
    return mMatrix.isIndexValid(id);
}

inline bool
TriggerMatrix::isPositionInside(const Vector2& position) const
{
    return mMatrix.isPositionInside(position);
}

inline TriggerMatrix::ZoneElement&
TriggerMatrix::getZoneElement(core::uint16_t id)
{
    ASSERT(id < mZoneElementSize);
    ASSERT(mZoneElements);
    return mZoneElements[id];
}

} /* namespace core */
#endif /* TRIGGERMATRIX_H_ */
