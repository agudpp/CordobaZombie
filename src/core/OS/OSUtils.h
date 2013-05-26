/*
 * OSUtils.h
 *
 *  Created on: May 26, 2013
 *      Author: agustin
 */

#ifndef OSUTILS_H_
#define OSUTILS_H_

#include <string>

namespace core {

////////////////////////////////////////////////////////////////////////////////
// Here we will define all the methods that will be associated with filesystem
//
namespace fs {

/**
 * @brief Check if a file exists
 * @param filename  The filename (path) we want to check
 * @returns true if the file exists | false otherwise
 */
bool
fileExists(const char* name);

}

////////////////////////////////////////////////////////////////////////////////
// Environment methods
//
namespace env {

/**
 * Read a environment variable
 * @param   var     The varname
 * @param   result  The result of the environment variable
 * @return  True on success, false otherwise
 */
bool
readEnvVar(const std::string &var, std::string &result);

}



} /* namespace core */
#endif /* OSUTILS_H_ */
