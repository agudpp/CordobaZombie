/*
 * SelectionData.h
 *
 *  Created on: 03/12/2012
 *      Author: agustin
 */

#ifndef SELECTIONDATA_H_
#define SELECTIONDATA_H_

#include <vector>
#include <limits>

namespace selection {

// Forward declaration
//
class SelectableObject;


struct SelectionData {
    SelectionData(const std::vector<SelectableObject *> &objs, bool areSelected) :
        objects(objs)
    ,   selected(areSelected)
    {
    }


    const bool selected;    // true mean the objects are selected, false
                            // mean that the objects are being unselected
    const std::vector<SelectableObject *> &objects;
};


// Aditional info used by the SelectableObjects
//
struct Info {
    // The picture index used by the HUD to show the PassportPicture
    // If we have not a index this value will be -1
    unsigned short passportPicture;


    Info() :
        passportPicture(std::numeric_limits<unsgined short>::max())
    {}
};

}

#endif /* SELECTIONDATA_H_ */
