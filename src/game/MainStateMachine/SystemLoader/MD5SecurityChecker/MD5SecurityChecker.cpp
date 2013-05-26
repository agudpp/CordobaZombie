/*
 * MD5SecurityChecker.cpp
 *
 *  Created on: 24/11/2011
 *      Author: agustin
 */

#include <fstream>
#include <stack>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "md5Lib/md5.h"
#include "md5Lib/md5_utils.h"
#include "FileManager.h"


#include "MD5SecurityChecker.h"


/* Generate the encrypted file from a path and a list of filters */
bool MD5SecurityChecker::generateEncryptedFile(const std::string &path,
		const std::list<std::string> &filters,
		std::string &result)
{
	std::list<std::string> fileList;
	std::cout << "Getting all files from: " << path << std::endl;
	if(!getAllFiles(path, fileList)){
		std::cout << "Error getting files from " << path << std::endl;
		return false;
	}

	// filter the files
	std::cout << "Filtering files" << std::endl;
	std::list<std::string> filteredFiles;
	filterFiles(fileList,filters,filteredFiles);

	// we will create the result file
	std::cout << "Creating the result file" << std::endl;
	result.clear();
	for(std::list<std::string>::iterator it = filteredFiles.begin();
			it != filteredFiles.end(); ++it){
		// get the file path without the path
		std::string file = *it;
		size_t bpos = file.find(path);
		file.erase(bpos, path.size());

		result.append(file);
		result.append("\t");
		std::string code;
		if(!generateCode(*it, code)){
			std::cout << "Error generating code for " << *it << std::endl;
			return false;
		}
		result.append(code);
		// TODO: Check if windows change here.
		result.append("\n");
	}
	std::cout << "File generated: \n" << result <<  std::endl;

	// encrypt the file
	std::cout << "Encrypting" << std::endl;
	std::string deco(result);
	result.clear();
	encryptFile(deco, result);

	return true;
}

/* Check encrypted file, this will check relative paths, not absolute.
 * Returns false on error, true on success */
bool MD5SecurityChecker::checkFile(const std::string &path,
		const std::string &prependPath)
{
	std::string encryptedFile;
	std::cout << "Reading file content: " << path << std::endl;
	if(!readFileContent(path,encryptedFile)){
		std::cout << "Error opening file " << path << std::endl;
		return false;
	}

	// decrypt the file
	std::string decryptedFile;
	std::cout << "Decrypting file " << std::endl;
	decryptFile(encryptedFile, decryptedFile);

	// get all the entries
	std::cout << "Getting entries " << std::endl;
	std::list<Entry> entries;
	if(!getAllEntries(decryptedFile, entries)){
		std::cout << "Error parsing the decripted file " << path << std::endl;
		return false;
	}

	for(std::list<Entry>::iterator it = entries.begin(); it != entries.end(); ++it){
		std::string code;
		if(!generateCode(prependPath + (*it).filename, code)){
			std::cout << "Error getting code from " << prependPath + it->filename << std::endl;
			return false;
		}

		// we have the code, check that is the right
		if((*it).code != code) {
			std::cout << "Invalid code from file " << it->filename << " with code: "
					<< it->code << " and correct code is: " << code << std::endl;
			return false;
		}
	}

	// if we are here, then all the codes are ok.
	return true;
}

/* Open and read file content. */
bool MD5SecurityChecker::readFileContent(const std::string &path, std::string &content)
{
	char *buffer;
	std::ifstream is;
	int fLength = 0;

	/* abrimos el archivo y lo leemos completo */
	is.open(path.c_str(), std::ios::binary );
	if (!is.good()) {
		return false;
	}

	// obtenemos el tamaño y lemos todo
	is.seekg (0, std::ios::end);
	fLength = is.tellg();
	is.seekg (0, std::ios::beg);

	// pedimos memoria
	buffer = new char [fLength + 1];
	buffer[fLength] = '\0';
	// leemos
	is.read (buffer,fLength);

	if (is.fail()) {
			delete[] buffer;
			is.close();
			return false;
	}


	is.close();
	/*! FIXME: ineficiente! */
	content = buffer;
	delete[] buffer;

	return true;
}

/* Encrypt data */
void MD5SecurityChecker::encryptFile(const std::string &data, std::string &result)
{
	char *dataAux = new char[data.size()+1];

	memcpy(dataAux, data.c_str(), data.size());
	dataAux[data.size()] = '\0';
	for( int i=0; dataAux[i] != '\0'; ++i )
		++dataAux[i];
	result = dataAux;
	delete[] dataAux;
}

/* Decrypt data */
void MD5SecurityChecker::decryptFile(const std::string &data, std::string &result)
{
	char *dataAux = new char[data.size()+1];

	std::cout << "Data.size " <<data.size() << std::endl;
	memcpy(dataAux, data.c_str(), data.size());
	dataAux[data.size()] = '\0';
	 for(int i = 0 ; dataAux[i] != '\0'; ++i){
		 --(dataAux[i]);
	 }

	result.assign(dataAux, data.size());
	delete[] dataAux;
}

/* Get all file entries from a decrypted file */
bool MD5SecurityChecker::getAllEntries(const std::string &data, std::list<Entry> &result)
{
	size_t bPos = 0;
	size_t ePos = 0;

	result.clear();
	while(ePos != std::string::npos && bPos != std::string::npos){
		// get the filename
		ePos = data.find('\t', bPos);
		if(ePos == std::string::npos){
			break;
		}
		Entry e;
		e.filename = data.substr(bPos, ePos - bPos);
		bPos = ePos + 1;
		ePos = data.find('\n', bPos);
		if(ePos == std::string::npos){
			break;
		}
		e.code = data.substr(bPos, ePos - bPos);
		bPos = ePos + 1;

		result.push_back(e);

	}

	return true;
}

/* Get all the files from a directory recursively */
bool MD5SecurityChecker::getAllFiles(const std::string &path, std::list<std::string> &fileList)
{

	// LINUX VERSION
	FileManager *fm = FileManager::getInstance();

	std::list<std::string> folders;
	// get all the folders;
	if(!fm->getAllFolders(path, folders, true)){
		std::cout << "Error getting folders\n";
		return false;
	}

	// now get all the files
	fileList.clear();
	std::list<std::string> auxFiles;
	std::list<std::string> ext;
	for(std::list<std::string>::iterator it = folders.begin(); it != folders.end();
			++it){
		std::cout << "Analyzing folder: " << *it << std::endl;
		if(!fm->getAllFiles(*it, auxFiles, ext, true)){
			std::cout << "Error getting files from path " << *it << std::endl;
			return false;
		}

		// conncat the list
		fileList.insert(fileList.begin(), auxFiles.begin(), auxFiles.end());
	}


	return true;
}

/* Filter files from a list using a list of filters. Returns the new file
 * list */
void MD5SecurityChecker::filterFiles(const std::list<std::string> &files,
		const std::list<std::string> &filters,
		std::list<std::string> &result)
{
	std::list<std::string>::const_iterator fit;

	result.clear();
	for(std::list<std::string>::const_iterator it = files.begin(); it != files.end(); ++it){
		// add it to the last, and if we find some filter, then we remove it
		result.push_back(*it);

		// find some filter
		for(fit = filters.begin(); fit != filters.end(); ++fit){
			if(it->find(*fit) != std::string::npos){
				// we find some filter, remove the last and continue
				result.pop_back();
				break;
			}
		}
	}

}



/* Generate code from file */
bool MD5SecurityChecker::generateCode(const std::string &path, std::string &code)
{
	std::string pathAux = path;
	size_t fileSize;
	char *hexaHash = 0;
	std::ifstream inputFStream;

	inputFStream.open(pathAux.c_str());
	if(!inputFStream.good()){
		return false;
	}
	inputFStream.seekg(0, std::ifstream::end);
	fileSize = inputFStream.tellg();
	inputFStream.close();

	hexaHash = slice_md5(pathAux.c_str(), 0, fileSize);
	code.assign(hexaHash, 32);
	free(hexaHash);


	return true;
}




