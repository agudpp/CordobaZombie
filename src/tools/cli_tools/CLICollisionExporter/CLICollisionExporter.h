/*
 * CLICollisionExporter.h
 *
 *  Created on: Nov 13, 2013
 *      Author: agustin
 */

#ifndef CLICOLLISIONEXPORTER_H_
#define CLICOLLISIONEXPORTER_H_


#include <command_line/CommandLine.h>
#include <cli_tools/common/XMLResourceParser.h>

namespace tool {

class CLICollisionExporter  : public core::CommandLine
{
private:

    // Helper structure to hold the arguments
    //
    struct ArgInfo {
        std::string rscPath;
        bool verbose;

        ArgInfo() : verbose(true) {}
    };

    struct TempObjectInfo {
        // the name of the asset
        std::string name;
        // the collision 2D object full path, if empty then means that the object
        // has no 2D representation (has an object .mesh probably that needs to
        // be converted into a .2DColl)
        std::string coll2DPath;
        // The same than above but for 3D.
        std::string coll3DPath;

        // @brief Clear all the data
        inline void clear(void)
        {
            name.clear();
            coll2DPath.clear();
            coll3DPath.clear();
        }
    };

    typedef std::vector<TempObjectInfo> TempObjectInfoVec;


public:
    CLICollisionExporter();
    virtual
    ~CLICollisionExporter();

    // @brief This method will be called with all the arguments read in the
    //        command line.
    // @param args      The argument list
    // @return the code ID of the tool (0 on success | errorCode otherwise)
    //
    virtual int
    execute(const std::vector<std::string>& args);

private:

    // @brief Print the help information of how to use the tool
    //
    void
    printHelp(void) const;

    // @brief Parse the arguments and fill the argument structure
    // @param args      The arguments to parse
    // @return true on success | false on error
    //
    bool
    parseArgs(const std::vector<std::string>& args);

    // @brief Load all the resources we want to try to create its collision
    //        data and put all its name in a list.
    //        We will only get the resources of type MESH
    // @param resourceNames     The resulting resources names
    // @return true on succes | false otherwise
    //
    bool
    getResourceNames(std::vector<std::string>& names);

    // @brief Get all the information related to a particular resource name.
    //        This class will fill the TempObjectInfo structure from a given
    //        resource name.
    // @param rscName       The resource name
    // @param tmp           Temporary object structure to be filled
    // @return true on success | false otherwise
    //
    bool
    getTemporaryInfo(const std::string& rscName, TempObjectInfo& tmp) const;

    // @brief Process the object to transform all the information we need to
    //        (2D and 3D).
    // @param tmp       The temporary object containing the information to proccess
    // @return true on success | false otherwise
    //
    bool
    processObject2D(const TempObjectInfo& tmp) const;
    bool
    processObject3D(const TempObjectInfo& tmp) const;

private:
    ArgInfo mArgInfo;
    XMLResourceParser mResourcesParser;
};

}

#endif /* CLICOLLISIONEXPORTER_H_ */
