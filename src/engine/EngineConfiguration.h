/*
 * EngineConfiguration.h
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#ifndef ENGINECONFIGURATION_H_
#define ENGINECONFIGURATION_H_


#include <string>

#include <tinyxml/tinyxml.h>


// @brief Here we will put the configuration stuff (probably readed from an xml
//        or a config file or whatever).
//


namespace engine {

class EngineConfiguration
{
public:
    EngineConfiguration();
    ~EngineConfiguration();

    // @brief Load the configuration from a file path.
    // @param path      The path to the configuration file
    // @return true on success | false otherwise
    //
    bool
    load(const std::string& path);

    // @brief Get an specific value from a module name and the key value.
    // @param moduleName        The module name
    // @param key               The key (ID) of the value we want to get
    // @param val               The associated value we want
    // @return true if the value exists and could be parsed | false otherwise
    //
    bool
    getValue(const std::string& moduleName, const std::string& key, std::string& val) const;
    bool
    getValue(const std::string& moduleName, const std::string& key, unsigned int& val) const;
    bool
    getValue(const std::string& moduleName, const std::string& key, int& val) const;
    bool
    getValue(const std::string& moduleName, const std::string& key, float& val) const;

private:
    TiXmlDocument mDoc;
};

} /* namespace engine */
#endif /* ENGINECONFIGURATION_H_ */
