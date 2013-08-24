/*
 * OgreNameGen.cpp
 *
 *  Created on: Jun 25, 2013
 *      Author: agustin
 */

#include "OgreNameGen.h"

#include <OgreStringConverter.h>

namespace {
static unsigned int nameCounter = 0;
}

namespace core {

namespace OgreNameGen {

Ogre::String
getFreshName(void)
{
    return Ogre::String("n") + Ogre::StringConverter::toString(nameCounter++);
}

}

} /* namespace core */
