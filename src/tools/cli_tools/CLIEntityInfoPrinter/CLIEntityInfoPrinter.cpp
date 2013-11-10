/*
 * CLIEntityInfoPrinter.cpp
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#include "CLIEntityInfoPrinter.h"

#include <iostream>
#include <sstream>

#include <cli_tools/common/EntityInformation.h>

namespace tool {

CLIEntityInfoPrinter::CLIEntityInfoPrinter()
{

}

CLIEntityInfoPrinter::~CLIEntityInfoPrinter()
{

}


int
CLIEntityInfoPrinter::execute(const std::vector<std::string>& args)
{
    if (args.size() <= 1) {
        debugERROR("We need a entity name to be able to execute this tool\n");
        return -1;
    }

    // load the entity information and print it in the std cout
    std::stringstream ss;
    EntityInformation entityInfo;
    if (!entityInfo.parseResource(args[1], mSceneMgr)) {
        debugERROR("Error parsing the entity %s\n", args[0].c_str());
        return -1;
    }

    entityInfo.getInformation(ss);

    std::cout << "\n\nCLIEntityInfoPrinter tool\n\n";
    std::cout << ss.str() << "\n\n";
    return 0;
}

} /* namespace tool */
