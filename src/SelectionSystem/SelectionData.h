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
    SelectionData(const std::vector<SelectableObject *> &objs) :
        selectedObjects(objs)
    {
    }


    const std::vector<SelectableObject *> &selectedObjects;
};


}

#endif /* SELECTIONDATA_H_ */
