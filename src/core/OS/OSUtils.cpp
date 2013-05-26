/*
 * OSUtils.cpp
 *
 *  Created on: May 26, 2013
 *      Author: agustin
 */

#include "OSUtils.h"

#include <cstdlib>
#include <boost/filesystem.hpp>

namespace core {

namespace fs {

////////////////////////////////////////////////////////////////////////////////
bool
fileExists(const char* name)
{
    if (name == 0) {
        return false;
    }
    return boost::filesystem::exists(name);
}

}

////////////////////////////////////////////////////////////////////////////////
//                                   Env                                      //
////////////////////////////////////////////////////////////////////////////////

namespace env {

////////////////////////////////////////////////////////////////////////////////
bool
readEnvVar(const std::string &var, std::string &result)
{
    // get the var
    const char* envVar = std::getenv(var.c_str());
    if (envVar == 0) {
        return false;
    }
    result = envVar;
    return true;
}

}
} /* namespace core */
