/*
#include <iostream>
#include <string>
#include <list>
#include <fstream>

#include "MD5SecurityChecker.h"

int main(int argc, char **args)
{
	std::string encryptedFile;
	std::list<std::string> filters;

	filters.push_back(".encrypted");
	filters.push_back("main");

//	MD5SecurityChecker::generateEncryptedFile("/home/agustin/programacion/ogre/2012_zombieGame/svn/extras/MD5SecurityChecker",filters,encryptedFile);
//	std::ofstream out;
//	out.open("demo.encrypted", std::ios::out);
//	out.write(encryptedFile.c_str(), encryptedFile.size());
//	out.close();

	// decrypting
	if(MD5SecurityChecker::checkFile("/home/agustin/programacion/ogre/"
			"2012_zombieGame/svn/extras/MD5SecurityChecker/demo.encrypted",
			"/home/agustin/programacion/ogre/2012_zombieGame/svn/extras/MD5SecurityChecker")){
		std::cout << "FILE IS OK\n";
	} else {
		std::cout << "FILE IS CORRUPT\n";
	}

	return 0;
}
*/
