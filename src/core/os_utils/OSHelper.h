/*
 * OSHelper.h
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#ifndef OSHELPER_H_
#define OSHELPER_H_

#include <string>

#include <types/basics.h>

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


////////////////////////////////////////////////////////////////////////////////
// File manipulation
//

// @brief Add the end path var for a given path (using the current operative system)
// @param path      The path to add the var at the end
//
inline void
addEndPathVar(std::string& path);

// @brief Extract the filename of a path.
// @param path      The path where the filename is
// @param result    The resulting filename
// @return true on succes | false otherwise
//
inline bool
extractFileName(const std::string& path, std::string& result);

// @brief Get the extension filename and the name of the file from a full file name
// @param fullName      The full name (name + extension)
// @param name          The extracted name
// @param ext           The extracted extension
// @return true on success | false otherwise
//
inline bool
extractFileExtension(const std::string& fullName,
                     std::string& name,
                     std::string& ext);

// @brief Extract the path of a full path (with filename). This method will
//        return the associated path with the last character = '/' | '\\'
// @param fullPath  The full path
// @param result    The resulting path without filename
// @return true on succes | false otherwise
//
inline bool
extractPath(const std::string& fullpath, std::string& result);








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

inline bool
extractFileName(const std::string& path, std::string& filename)
{
    // Get the path to the resource folder
    core::size_t lastBar = 0;
#ifdef _WIN32
    lastBar = path.rfind('\\')+1;
#else
    lastBar = path.rfind('/')+1;
#endif
    if (lastBar == std::string::npos) {
        return false;
    }

    filename = path.substr(lastBar, std::string::npos);
    return true;
}

inline bool
extractFileExtension(const std::string& fullName,
                     std::string& name,
                     std::string& ext)
{
    const core::size_t dotPos = fullName.rfind('.');
    if (dotPos == std::string::npos) {
        return false;
    }
    name = fullName.substr(0, dotPos);
    ext = fullName.substr(dotPos+1, fullName.size());
    return true;
}

inline bool
extractPath(const std::string& fullpath, std::string& result)
{
    // Get the path to the resource folder
    int lastBar = fullpath.size()-1;
    while (lastBar >= 0) {
    	if (fullpath[lastBar] == '\\' || fullpath[lastBar] == '/') break;
    	--lastBar;
    }
    if (lastBar <= 0) {
        return false;
    }

    result = fullpath.substr(0, lastBar);
    return true;
}


}

} /* namespace core */
#endif /* OSHELPER_H_ */
