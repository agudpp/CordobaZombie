/* Tool para generar un Cmake de la forma:
 *
 *
IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

SET(CP DEV_ROOTH_PATH/actualPath)

set(HDRS
	${HDRS}
	$(<CP>)/...
)

set(SRCS
	${SRCS}
	$(<CP>)/...
)

set(ACTUAL_DIRS
	$(<CP>)/...
)

include_directories(${ACTUAL_DIRS})


 *
 *
 * autogenCmake.cpp
 *
 *  Created on: 29/08/2011
 *      Author: agustin
 */

#include <list>
#include <map>
#include <string>
#include <cassert>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>


#include "FileManager.h"


#define	DEV_ROOT_PATH_STR		"DEV_ROOT_PATH"

#define DEV_PATH_ENV_NAME		"ESTRATEGIA2012_DEV_PATH"


		// TODO: aca ponemos los folders que queremos evitar
static std::string folderFilters[] = {
		"UnitTest",
		"build",
		"..",
		".svn",
		".settings",
		""
};


static std::string actualPath = "";
static std::string envPath = "";


static void getPathsWithoutTests(const char *path, std::list<std::string> &result)
{
	assert(path);

	result.clear();
	assert(FileManager::getInstance()->getAllFolders(path, result));

	std::list<std::string> aux;
	int i = 0;
	for(std::list<std::string>::iterator it = result.begin(); it != result.end();
			++it){
		i = 0;
		while(folderFilters[i] != ""){
			if(it->find(folderFilters[i]) != std::string::npos){
				aux.push_back(*it);
				break;
			}
			++i;
		}
		std::cout << "Folder: " << *it << "\n";
	}

	for(std::list<std::string>::iterator it = aux.begin(); it != aux.end();
			++it){
		result.remove(*it);
	}
}


static void getSourcesAndHeaders(const std::list<std::string> &paths,
								std::list<std::string> &headers,
								std::list<std::string> &sources)
{
	std::list<std::string> exts;
	std::list<std::string> aux;


	std::cout << "Obteniendo Headers\n";

	exts.push_back(".h");
	headers.clear();
	for(std::list<std::string>::const_iterator it = paths.begin(); it != paths.end(); ++it){
		assert(FileManager::getInstance()->getAllFiles(*it, aux, exts));

		headers.insert(headers.begin(), aux.begin(), aux.end());
	}


	std::cout << "Obteniendo Sources\n";

	exts.clear();
	aux.clear();
	exts.push_back(".cpp");
	sources.clear();
	for(std::list<std::string>::const_iterator it = paths.begin(); it != paths.end(); ++it){
		assert(FileManager::getInstance()->getAllFiles(*it, aux, exts));

		sources.insert(sources.begin(), aux.begin(), aux.end());
	}

}


static void replaceListPaths(std::list<std::string> &l)
{
	size_t pos = 0;
	for(std::list<std::string>::iterator it = l.begin(); it != l.end(); ++it){
		pos = it->find(envPath);
		if(pos != std::string::npos) {
			it->replace(pos, envPath.size(), "");
		}
		pos = it->find("./");
		if(pos != std::string::npos) {
			it->replace(pos, 2, "/");
		}
	}
}

static void constructOutput(std::string &out,
		const std::list<std::string> &sources,
		const std::list<std::string> &headers,
		const std::list<std::string> &folders)
{

	std::cout << "Construyendo CMAKE\n";

	out = "IF(NOT DEV_ROOT_PATH)\n"
			"\tmessage(SEND_ERROR \"No esta seteado DEV_ROOT_PATH\")\n"
			"endif()\n"
			"include(${DEV_ROOT_PATH}/DevPaths.cmake)\n\n";

	out.append("set(CP " + actualPath + ")\n\n");
	out += "set(SRCS\n";
	out += "\t${SRCS}\n";
	for(std::list<std::string>::const_iterator it = sources.begin(); it != sources.end(); ++it){
		out += "\t${" DEV_ROOT_PATH_STR  "}" + actualPath;
		out += *it;
		out += "\n";
	}
	out += ")\n\n";

	// headers
	out += "set(HDRS\n";
	out += "\t${HDRS}\n";
	for(std::list<std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it){
		out += "\t${" DEV_ROOT_PATH_STR "}" + actualPath;
		out += *it;
		out += "\n";
	}
	out += ")\n\n";

	// directories
	out += "set(ACTUAL_DIRS\n";
	for(std::list<std::string>::const_iterator it = folders.begin(); it != folders.end(); ++it){
		out += "\t${" DEV_ROOT_PATH_STR "}" + actualPath;
		if(*it != ".") out += *it;
		out += "\n";
	}
	out += ")\n\n";
	out += "include_directories(${ACTUAL_DIRS})\n";

}

int main(int argc, char **args)
{
	if(argc < 2){
		std::cout << "Hace falta especificar el path sobre el cual se quiere "
				"realizar el make.\n";
		return 1;
	}

	// get the actual path
	char *env = getenv(DEV_PATH_ENV_NAME);
	if(env == 0){
		std::cout << "La variable de entorno DEV_PATH_ENV_NAME no fue encontrada\n";
		return 1;
	}

	actualPath = get_current_dir_name();
	// TODO: windows cambiar / por
	std::cout << "actual Path: " << actualPath << std::endl;

	envPath = env;
	if((envPath[envPath.size()-1] == '/') || (envPath[envPath.size()-1] == '\\')){
		envPath.resize(envPath.size()-1);
	}

	std::cout << "envPath Path: " << envPath << std::endl;

	size_t pos = actualPath.find(envPath);
	if(pos == std::string::npos){
		std::cout << "Estamos intentando crear un cmake fuera del repo!\n";
		return 1;
	}

	actualPath.replace(pos, envPath.size(), "");

	std::cout << "Obteniendo directorios\n";
	std::list<std::string> folders;
	// get the paths from the folder
	getPathsWithoutTests(args[1], folders);


	std::cout << "Directorios obtenidos\n";
	// get the files inside of that paths
	std::list<std::string> headers;
	std::list<std::string> sources;

	getSourcesAndHeaders(folders, headers, sources);

	replaceListPaths(folders);
	replaceListPaths(headers);
	replaceListPaths(sources);

	std::string out;
	constructOutput(out,sources,headers,folders);

	std::cout << "Escribiendo archivo\n";
	assert(FileManager::getInstance()->writeFile("AutoGen.cmake", out, true));

	return 0;
}
