/*
 * ConfigFileParser.cpp
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#include "DebugUtil.h"
#include "ConfigFileParser.h"


/**
 * Parse file content
 */
bool ConfigFileParser::parseFileContent(const std::string &content)
{
	mConfig.clear();

	size_t ep = 0;
	size_t bp = 0;

	while(ep != std::string::npos && bp != std::string::npos){
		ep = content.find('=', bp);
		if(ep == std::string::npos || bp == std::string::npos) break;
		//get the key
		std::string key = content.substr(bp, ep - bp);

		// get the value
		bp = ep + 1;
		ep = content.find('\n', bp);
		// TODO: el archivo debe terminar con un enter
		if(ep == std::string::npos || bp == std::string::npos) ASSERT(false);
		std::string value = content.substr(bp, ep - bp);
		if(value[value.size()-1] == '\r') value.erase(value.size()-1,1);

		// update positions
		bp = ep + 1;

		// parse the value into multiple strings
		std::list<std::string> lvalues;
		int size = value.size();
		lvalues.push_back("");
		for(int i = 0; i < size; ++i){
			if(value[i] != '|'){
				lvalues.back().push_back(value[i]);
			} else {
				// avoid this character, insert new string
				lvalues.push_back("");
			}
		}

		// add to the config
		Configuration::iterator it = mConfig.find(key);
		if(it != mConfig.end()){
			debugRED("ConfigFile with 2 equals keys %s\n", key.c_str());
			ASSERT(false);
			return false;
		}
		mConfig[key] = lvalues;
	}

	return true;
}



ConfigFileParser::ConfigFileParser()
{

}

ConfigFileParser::~ConfigFileParser()
{

}

/**
 * Parse a configuration file.
 * KEY1=VALUE1|VALUE2....
 */
bool ConfigFileParser::parseFile(const std::string &fname)
{
	std::string content;
	if(!FileManager::getInstance()->readFileContent(fname, content)){
		debugRED("Error reading the config file %s\n", fname.c_str());
		return false;
	}

	// parse the file
	if(!parseFileContent(content)){
		debugRED("Error parsing the file content\n");
		return false;
	}

	// Ready..
	return true;
}

/**
 * Check if a key exists
 */
bool ConfigFileParser::hasKey(const std::string &key)
{
	Configuration::iterator it = mConfig.find(key);
	return it != mConfig.end();
}

/**
 * Get the value
 */
void ConfigFileParser::getValue(const std::string &key, std::list<std::string> &value)
{
	if(!hasKey(key)){
		debugRED("Key %s not found\n", key.c_str());
		ASSERT(false);
	}
	value = mConfig[key];
}

void ConfigFileParser::getValue(const std::string &key, std::string &value)
{
	std::list<std::string> v;
	getValue(key, v);
	ASSERT(!v.empty());
	value = v.front();
}

#ifdef DEBUG
#include <iostream>
/**
 * Print all the parsed file
 */
void ConfigFileParser::print(void)
{
	Configuration::iterator it1;
	for(it1 = mConfig.begin(); it1 != mConfig.end(); ++it1){
		std::list<std::string>::iterator it = it1->second.begin();
		std::cout << "Key: " << it1->first << "\n";
		for(; it != it1->second.end(); ++it){
			 std::cout << "\tValue: " << *it << std::endl;
		}
		std::cout << "\n\n";
	}
}
#endif
