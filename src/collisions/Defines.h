/*
 * Defines.h
 *
 *  Created on: Feb 17, 2013
 *      Author: agustin
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#include <vector>


namespace coll {
class Object;

typedef std::vector<const Object *> ConstObjectVec;
typedef std::vector<Object *> ObjectVec;

typedef unsigned int UnitType;

}


#endif /* DEFINES_H_ */
