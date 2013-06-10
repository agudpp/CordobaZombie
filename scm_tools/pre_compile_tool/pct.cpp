/* To compile:
 * g++ -o pct pct.cpp -lboost_system -lboost_filesystem
 * pct.cpp
 *
 *  Created on: Jun 6, 2013
 *      Author: agustin
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <set>
#include <fstream>
#include <cassert>

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
    std::string lconfPath;
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

typedef std::map<std::string, Library> StrToLibMap;


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

    // remove multiple definitions
    PathVec::iterator uniqueIt = std::unique(result.begin(), result.end());
    result.resize(std::distance(result.begin(), uniqueIt));

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

    // set the lconf path
    result.lconfPath = lconf.parent_path().string();

    // load headers and sources related to this library
    PathVec childFiles;
    if (!getFilesFromFolderRec(result.lconfPath, childFiles)) {
        std::cerr<< "Error geting files for the library: " << path << std::endl;
        return false;
    }

    // parse for the headers
    StringVec filters;
    filters.push_back(".h");
    filters.push_back(".hpp");
    filters.push_back(".inl");

    PathVec filteredFiles;
    applyIncludeFilter(childFiles, filters, filteredFiles);
    result.headers.reserve(filteredFiles.size());
    for (std::size_t i = 0, size = filteredFiles.size(); i < size; ++i) {
        result.headers.push_back(filteredFiles[i].string());
    }

    // parse for sources
    filters.clear();
    filters.push_back(".c");
    filters.push_back(".cpp");
    filters.push_back(".cxx");
    filters.push_back(".cx");
    filteredFiles.clear();

    applyIncludeFilter(childFiles, filters, filteredFiles);
    result.sources.reserve(filteredFiles.size());
    for (std::size_t i = 0, size = filteredFiles.size(); i < size; ++i) {
        result.sources.push_back(filteredFiles[i].string());
    }

    return true;
}


// Get all the libraries in a certain path and put it inside a map
//
static bool
getLibraries(const std::string& path, StrToLibMap& result)
{
    // get all the lconf files
    PathVec files;
    if (!getFilesFromFolderRec(path, files)) {
        std::cerr << "Error getting the libraries\n";
        return false;
    }

    result.clear();
    // filter all the lconfs

    StringVec filters;
    filters.push_back(".lconf");
    PathVec filteredFiles;
    applyIncludeFilter(files, filters, filteredFiles);

    // now iterate over all the filtered files and get the associated libs
    for (std::size_t i = 0, size = filteredFiles.size(); i < size; ++i) {
        Library lib;
        if (!parseLibraryFile(filteredFiles[i].string(), lib)) {
            std::cerr << "Error parsing library " << filteredFiles[i].string() << std::endl;
            continue;
        }

        // add the library into the map, check if already exists
        StrToLibMap::iterator it = result.find(lib.name);
        if (it != result.end()) {
            std::cerr << "library " << lib.name << " already exists!!" <<
                "\nlib_path: " << lib.lconfPath << " and " << it->second.lconfPath << std::endl;
            continue;
        }
        // new lib, add it
        result[lib.name] = lib;
    }

    std::cout << "We found " << result.size() << " libraries\n";
    return true;
}

// This method returns all the local dependencies from a given library name.
// We will iterate over all the libraries and search for the dependencies
// of the given library (including the given library)
//
static bool
getDependencies(const StrToLibMap& allLibs,
                const std::string& libName,
                std::vector<const Library*>& result)
{
    StrToLibMap::const_iterator it = allLibs.find(libName),
                                endIt = allLibs.end();

    if (it == endIt) {
        std::cerr << "library " << libName << " wasn't found!\n";
        return false;
    }

    std::set<const Library*> markedLibs;
    std::vector<const Library*> toCheck;
    toCheck.reserve(512);
    result.reserve(512);
    toCheck.push_back(&(it->second));

    while (!toCheck.empty()) {
        const Library* currentLib = toCheck.back();
        result.push_back(currentLib);
        assert(currentLib != 0);

        // check that this lib wasn't already checked
        if (markedLibs.find(currentLib) != markedLibs.end()) {
            // we already check this lib!!! cycle dependencies??? probably not..
            // probably there are multi libs including the same library
            toCheck.pop_back();
            continue;
        }

        toCheck.pop_back();
        // we need to check now all the dependencies for this
        const StringVec& deps = currentLib->localDeps;
        for (std::size_t i = 0, size = deps.size(); i < size; ++i) {
            // get the library
            it = allLibs.find(deps[i]);
            if (it == endIt) {
                std::cerr << "Library " << deps[i] << " not found, this dep is "
                    << "in " << currentLib->name << " located in " << currentLib->lconfPath <<
                    std::endl;
                continue;
            }
            const Library* depLib = &(it->second);

            // add to the correspoding places
            toCheck.push_back(depLib);
        }
        markedLibs.insert(currentLib);
    }

    // remove multiple definitions
    std::vector<const Library*>::iterator uniqueIt =
        std::unique(result.begin(), result.end());
    result.resize(std::distance(result.begin(), uniqueIt));

    return true;
}

// Method used to extract all the headers / sources / dependencies from a list
// of unique libraries list
//
static bool
extractLibsInfo(const std::vector<const Library*>& dependencyLibs,
                std::set<std::string>& sourcesToUse,
                std::set<std::string>& headersToUse,
                std::set<std::string>& dependenciesToUse)
{
    sourcesToUse.clear();
    headersToUse.clear();
    dependenciesToUse.clear();

    for (std::size_t i = 0, size = dependencyLibs.size(); i < size; ++i) {
        assert(dependencyLibs[i]);
        const Library& lib = *(dependencyLibs[i]);

        // add headers
        headersToUse.insert(lib.headers.begin(), lib.headers.end());

        // add sources
        sourcesToUse.insert(lib.sources.begin(), lib.sources.end());

        // add deps
        dependenciesToUse.insert(lib.thirdPartyDeps.begin(), lib.thirdPartyDeps.end());
    }

    return true;
}

// Print the output cmake file
//
static bool
printOutputFile(const std::string& fileName,
                const std::set<std::string>& sourcesToUse,
                const std::set<std::string>& headersToUse,
                const std::set<std::string>& dependenciesToUse)
{
    std::ofstream file;

    file.open(fileName.c_str(), std::ifstream::out);
    if (!file) {
        std::cerr << "Error opening file " << fileName << std::endl;
        return false;
    }

    typedef std::set<std::string> StrSet;

    // sources
    file << "\nset (SRCS ${SRCS}\n";
    for (StrSet::const_iterator it = sourcesToUse.begin(), end = sourcesToUse.end();
            it != end; ++it) {
        file << "\t" << *it << "\n";
    }
    file << ")\n\n";

    // headers
    file << "\nset (HDRS ${HDRS}\n";
    for (StrSet::const_iterator it = headersToUse.begin(), end = headersToUse.end();
           it != end; ++it) {
       file << "\t" << *it << "\n";
    }
    file << ")\n\n";

    // third lib dependencies
    file << "set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS}\n";
    for (StrSet::const_iterator it = dependenciesToUse.begin(), end = dependenciesToUse.end();
           it != end; ++it) {
       file << "\t" << *it << "\n";
    }
    file << ")\n\n";

    return true;
}

int main(int argc, char *args[])
{
    if (argc < 3) {
        std::cerr << "Wrong arguments\n" <<
            "to run pct you have to call it like this:\n"
            "\tptc ENVIRONMENT_SRC_PATH_NAME libName [output]\n"
            "where:\n"
            "\tENVIRONMENT_SRC_PATH_NAME\tmeans the variable to be used \"ESTRATEGIA_2012_DEV_PATH\"\n"
            "\tlibName\t\t the library name we want to generate the dependencies tree\n"
            "\toutput\t\t\the output file, if is not specified AutoGen.cmake will be generated\n";
        return -1;
    }
    const char* envVar = std::getenv(args[1]);
    if (envVar == 0) {
        std::cerr << "Error, variable " << args[1] << " is not defined\n";
        return -2;
    }

    const std::string envVarName = envVar;
    const std::string libName(args[2]);
    const std::string output = (argc == 4) ? args[3] : "AutoGen.cmake";

    // get for all the libraries into a map that matches lib_name -> Library
    StrToLibMap libs;
    if (!getLibraries(envVarName, libs)) {
        std::cerr << "Error listing the libraries for " << envVarName << "\n";
        return -3;
    }

    // here we should check the dependencies / parent libs / etc / (all graph
    // checks should be here probably).

    // get all the libraries dependencies from an specific lib, we will use pointers
    // here to avoid all the memory copy! so don't remove the libs map!
    std::vector<const Library*> dependencyLibs;
    if (!getDependencies(libs, libName, dependencyLibs)) {
        std::cerr << "Error getting the dependencies for library " << libName << std::endl;
        return -4;
    }

    // now that we have all the libraries we should use, extract all the
    // headers, sources, and third_party libs in 3 set of strings (to get uniques
    // files and dependencies)
    std::set<std::string> sourcesToUse;
    std::set<std::string> headersToUse;
    std::set<std::string> dependenciesToUse;
    if (!extractLibsInfo(dependencyLibs, sourcesToUse, headersToUse, dependenciesToUse)) {
        std::cerr << "Error extracting the information we need\n";
        return -5;
    }

    // write the .cmake file
    if (!printOutputFile(output, sourcesToUse, headersToUse, dependenciesToUse)) {
        std::cerr << "Some error occurr when trying to write the " << output << " file\n";
        return -6;
    }

    std::cout << "File " << output << " wrote successfully!\n\n";

    return 0;
}

