/* @info This class will generate a new fresh name everytime we call getFreshName()
 *
 * OgreNameGen.h
 *
 *  Created on: Jun 25, 2013
 *      Author: agustin
 */

#ifndef OGRENAMEGEN_H_
#define OGRENAMEGEN_H_

#include <OgreString.h>

namespace core {

namespace OgreNameGen
{

// @brief Return a new "unique" fresh name
// @returns the new fresh name
//
Ogre::String
getFreshName(void);


};

} /* namespace core */
#endif /* OGRENAMEGEN_H_ */
