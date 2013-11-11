/*
 * OSHelper.h
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#ifndef OSHELPER_H_
#define OSHELPER_H_

#include <string>

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

// @brief Add the end path var for a given path (using the current operative system)
// @param path      The path to add the var at the end
//
inline void
addEndPathVar(std::string& path);










////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
addEndPathVar(std::string& path)
{
    int last = path.size() - 1;
#ifdef _WIN32
    if(last >= 0 && path[last] != '\\') {
        path.append("\\");
    }
#else
    if (last >= 0 && path[last] != '/') {
        path.append("/");
    }
#endif
}

}



} /* namespace core */
#endif /* OSHELPER_H_ */
