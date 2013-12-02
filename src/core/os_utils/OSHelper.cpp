/*
 * OSHelper.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#include "OSHelper.h"

#include <fstream>
#include <cstdlib>
#include <cstdio>

namespace core {
namespace OSHelper {

bool
fileExists(const char* path)
{
    std::ifstream file(path);
    return file;
}


bool
getEnvVar(const char* var, char* &val)
{
    val = std::getenv(var);
    return val;
}


}

} /* namespace core */
