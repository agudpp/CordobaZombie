/*
 * MD5SecurityChecker.h
 *
 *  Created on: 24/11/2011
 *      Author: agustin
 */

#ifndef MD5SECURITYCHECKER_H_
#define MD5SECURITYCHECKER_H_

#include <iostream>
#include <string>
#include <list>

class MD5SecurityChecker
{

	typedef struct {
		std::string		filename;
		std::string		code;
	} Entry;

public:

	/* Generate the encrypted file from a path and a list of filters */
	static bool generateEncryptedFile(const std::string &path,
			const std::list<std::string> &filters,
			std::string &result);

	/* Check encrypted file, this will check relative paths, not absolute.
	 * Returns false on error, true on success
	 * Params:
	 * 	@prependPath 	is the path to prepend to the files saved in the encryped file
	 */
	static bool checkFile(const std::string &file, const std::string &prependPath);

private:
	/* Open and read file content. */
	static bool readFileContent(const std::string &path, std::string &content);

	/* Encrypt data */
	static void encryptFile(const std::string &data, std::string &result);

	/* Decrypt data */
	static void decryptFile(const std::string &data, std::string &result);

	/* Get all file entries from a decrypted file */
	static bool getAllEntries(const std::string &data, std::list<Entry> &result);

	/* Get all the files from a directory recursively */
	static bool getAllFiles(const std::string &path, std::list<std::string> &fileList);

	/* Filter files from a list using a list of filters. Returns the new file
	 * list */
	static void filterFiles(const std::list<std::string> &files,
			const std::list<std::string> &filters,
			std::list<std::string> &result);

	/* Generate code from file */
	static bool generateCode(const std::string &path, std::string &code);

};

#endif /* MD5SECURITYCHECKER_H_ */
