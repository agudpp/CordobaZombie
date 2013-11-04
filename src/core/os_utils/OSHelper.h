/*
 * OSHelper.h
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#ifndef OSHELPER_H_
#define OSHELPER_H_

namespace core {
namespace OSHelper {

// @brief Check if a file exists from a given path.
// @param path      The file path
// @return true if exists | false otherwise
//
bool
fileExists(const char* path);

// @brief Find out the enviroment var value
// @param var      The variable name
// @return true if the variable exists | false otherwise
//
bool
getEnvVar(const char* var, char* &val);

}

} /* namespace core */
#endif /* OSHELPER_H_ */
