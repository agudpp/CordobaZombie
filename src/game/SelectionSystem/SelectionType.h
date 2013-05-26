/*
 * SelectionType.h
 *
 *  Created on: 03/12/2012
 *      Author: agustin
 */

#ifndef SELECTIONTYPE_H_
#define SELECTIONTYPE_H_

namespace selection {
enum Type {
    SEL_TYPE_NONE           = 0,
    SEL_TYPE_PLAYER,
    SEL_TYPE_ZOMBIE,
    SEL_TYPE_CIVIL,
    SEL_TYPE_LVL_OBJECT,
    SEL_TYPE_BOMB,
    SEL_TYPE_SPECIAL_ACTION,
    SEL_TYPE_COL_OBJECT,        // Collectable object

    // this shouldn't be changed
    SIZE,

};
}

#endif /* SELECTIONTYPE_H_ */
