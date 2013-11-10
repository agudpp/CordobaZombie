/*
 * CLIEntityInfoPrinter.h
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#ifndef CLIENTITYINFOPRINTER_H_
#define CLIENTITYINFOPRINTER_H_

#include <command_line/CommandLine.h>

namespace tool {

class CLIEntityInfoPrinter : public core::CommandLine
{
public:
    CLIEntityInfoPrinter();
    ~CLIEntityInfoPrinter();

    // @brief This method will be called with all the arguments read in the
    //        command line.
    // @param args      The argument list
    // @return the code ID of the tool (0 on success | errorCode otherwise)
    //
    virtual int
    execute(const std::vector<std::string>& args);
};

} /* namespace tool */
#endif /* CLIENTITYINFOPRINTER_H_ */
