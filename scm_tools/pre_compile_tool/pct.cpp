/*
 * pct.cpp
 *
 *  Created on: Jun 6, 2013
 *      Author: agustin
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>


// Define the structures needed for this and the typedefs
//

typedef std::vector<std::string> StringVec;
typedef std::vector<boost::filesystem::path> PathVec;

namespace {
inline void
printVec(const StringVec& vec)
{
    std::cout << "\n";
    for (std::size_t i = 0, size = vec.size(); i < size; ++i) {
        std::cout << vec[i] << std::endl;
    }
    std::cout << std::endl;
}
}


// Library / module struct

struct Library {
    std::string name;
    StringVec sources;
    StringVec headers;
    StringVec localDeps;
    StringVec thirdPartyDeps;
    StringVec parentList;



    // for debug
    void print() const
    {
        std::cout << "name: " << name << std::endl;
        std::cout << "sources: " << std::endl; printVec(sources);
        std::cout << "headers: " << std::endl; printVec(headers);
        std::cout << "localDeps: " << std::endl; printVec(localDeps);
        std::cout << "thirdPartyDeps: " << std::endl; printVec(thirdPartyDeps);
        std::cout << "parentList: " << std::endl; printVec(parentList);

    }
};



// Methods
//

// list all the folders recursively that we have in a specific location
// and returns all of them in absolute mode (not relative)
//
static bool
getAllFolders(const std::string& path, PathVec& result)
{
    if (!boost::filesystem::is_directory(path)) {
        std::cerr << "[getAllFolders] Error, path " << path << " is not a directory!\n";
        return false;
    }

    result.clear();
    result.reserve(256);
    for ( boost::filesystem::recursive_directory_iterator end, dir(path);
           dir != end; ++dir ) {
        if (!boost::filesystem::is_directory(*dir)) {
            continue;
        }
        result.push_back(boost::filesystem::canonical(*dir));
    }

    return true;
}

// Get a list of files inside a folder recursively without taking into account
// the folders (excluding the folder).
//
static bool
getFilesFromFolderRec(const std::string& path, PathVec& result)
{
    if (!boost::filesystem::is_directory(path)) {
        std::cerr << "[getFilesFromFolder] Error, path " << path << " is not a directory!\n";
        return false;
    }

    // get all the files in this path
    result.clear();
    result.reserve(256);
    for ( boost::filesystem::recursive_directory_iterator end, dir(path);
           dir != end; ++dir ) {
        if (boost::filesystem::is_directory(*dir)) {
            continue;
        }
        result.push_back(boost::filesystem::canonical(*dir));
    }

    return true;
}

// apply filter to a list of paths (all the files/folders that matchs with the
// filters are returned in result, all the others that doesn't match are ignored)
// We will just find a specific filter in all the path, if we found it then we
// can assume that we need to include that path in the results.
//
static void
applyIncludeFilter(const PathVec& paths, const StringVec& filters, PathVec& result)
{
    result.reserve(paths.size());
    if (filters.empty()) {
        result = paths;
        return;
    }

    // check for all the paths and filters
    for (std::size_t i = 0, size = paths.size(); i < size; ++i) {
        const std::string p = paths[i].string();
        for (std::size_t j = 0, fcount = filters.size(); j < fcount; ++j) {
            if (p.find(filters[j]) != std::string::npos) {
                // we need to put the path in the result
                result.push_back(paths[i]);
            }
        }
    }
}

// Parse a Library from a lconf file path
//
static bool
parseLibraryFile(const std::string& path, Library& result)
{
    const boost::filesystem::path lconf(path);
    if (!boost::filesystem::is_regular_file(lconf)) {
        std::cerr << "[parseLibraryFile] Error, path is not an lconf file: " <<
            path << std::endl;
        return false;
    }

    // read the file content line by line and put each of these lines in a map
    typedef std::map<std::string, std::string> StrStrMap;
    StrStrMap values;
    std::ifstream inFile;
    inFile.open(path.c_str(), std::ifstream::in);

    char line[1024];
    StringVec splitted;
    splitted.reserve(2);
    while (inFile.getline(line, 1024)) {
        splitted.clear();
        boost::split(splitted, line, boost::is_any_of("="));
        if (splitted.size() != 2) {
            std::cerr << "Some problem parsing the file, we couldn't find the = "
                "or there are more than one?: " << splitted.size() << std::endl;
            continue;
        }

        // we have the key and value
        boost::trim(splitted[0]);
        boost::trim(splitted[1]);
        values[splitted[0]] = splitted[1];
        std::cout << "parsed: " << splitted[0] << " and: " << splitted[1] << std::endl;
    }

    // now we have the values check for the keys
    // name
    StrStrMap::iterator it = values.find("name"),
                        endIt = values.end();
    if (it == endIt) {
        std::cerr << "NO NAME FOR THE lconf: " << path << std::endl;
    } else {
        result.name = it->second;
    }

    // local dependencies
    it = values.find("local_deps");
    if (it == endIt) {
        std::cerr << "NO local_dep FOR THE lconf: " << path << std::endl;
    } else {
        // split the values
        boost::split(result.localDeps, it->second, boost::is_any_of(" "));
    }

    // external deps
    it = values.find("external_deps");
    if (it == endIt) {
        std::cerr << "NO external_deps FOR THE lconf: " << path << std::endl;
    } else {
        // split the values
        boost::split(result.thirdPartyDeps, it->second, boost::is_any_of(" "));
    }

    return true;
}

int main(int argc, char *args[])
{
    if (argc < 2) {
        std::cerr << "Error args\n";
        return 1;
    }

    PathVec dirs;
    std::cout << "getFilesFromFolderRec: " << getFilesFromFolderRec(args[1], dirs) << std::endl;
    for (std::size_t i = 0, size = dirs.size(); i < size; ++i) {
        std::cout << "File[" << i << "]: " << dirs[i] << std::endl;
    }

    std::cout << "\nApplying filters\n\n";
    StringVec filters;
    filters.push_back(".lconf");
    PathVec filteredFiles;
    applyIncludeFilter(dirs, filters, filteredFiles);
    for (std::size_t i = 0, size = filteredFiles.size(); i < size; ++i) {
        std::cout << "File[" << i << "]: " << filteredFiles[i] << std::endl;
    }

    if (filteredFiles.empty()) {
        std::cerr << "Empty filtered files\n";
        return 1;
    }
    Library lib;
    parseLibraryFile(filteredFiles[0].string(), lib);

    std::cout << "library parsed: " << std::endl;
    lib.print();


    return 0;
}

