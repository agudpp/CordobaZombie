/*
 * OSHelper.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#include "OSHelper.h"

#include <fstream>

namespace core {
namespace OSHelper {

bool
fileExists(const char* path)
{
    std::ifstream file(path);
    return file;
}

}

} /* namespace core */
