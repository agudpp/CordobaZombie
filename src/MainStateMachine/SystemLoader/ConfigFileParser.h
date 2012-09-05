/*
 * ConfigFileParser.h
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#ifndef CONFIGFILEPARSER_H_
#define CONFIGFILEPARSER_H_

#include <map>
#include <string>
#include <list>

#include "FileManager.h"

class ConfigFileParser
{
	typedef std::map<std::string, std::list<std::string> >	Configuration;
public:
	ConfigFileParser();
	~ConfigFileParser();

	/**
	 * Parse a configuration file.
	 * KEY1=VALUE1|VALUE2....
	 */
	bool parseFile(const std::string &fname);

	/**
	 * Check if a key exists
	 */
	bool hasKey(const std::string &key);

	/**
	 * Get the value
	 */
	void getValue(const std::string &key, std::list<std::string> &value);

	/**
	 * Get the first value if we have multiple
	 */
	void getValue(const std::string &key, std::string &value);

#ifdef DEBUG
	/**
	 * Print all the parsed file
	 */
	void print(void);
#endif

private:
	/**
	 * Parse file content
	 */
	bool parseFileContent(const std::string &content);

private:
	Configuration			mConfig;
};

#endif /* CONFIGFILEPARSER_H_ */
