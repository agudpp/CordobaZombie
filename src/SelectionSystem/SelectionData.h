/*
 * SelectionData.h
 *
 *  Created on: 03/12/2012
 *      Author: agustin
 */

#ifndef SELECTIONDATA_H_
#define SELECTIONDATA_H_

#include <vector>

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


}

#endif /* SELECTIONDATA_H_ */
