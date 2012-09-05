/*
 * FileManager.cpp
 *
 *  Created on: 24/04/2011
 *      Author: agustin
 */



#ifndef _BSD_SOURCE
        #define _BSD_SOURCE
#endif

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stack>
#include <fstream>

#include "FileManager.h"


FileManager *FileManager::mInstance = 0;

FileManager *FileManager::getInstance(void)
{
        if(!mInstance){
                mInstance = new FileManager;
        }

        return mInstance;
}


/* Returns all the folders in a path */
/******************************************************************************/
bool FileManager::getFoldersList(const std::string &path,
                std::list<std::string> &folders)
{
        DIR *dp = NULL;
        struct dirent *dirp = NULL;
        std::string aux, absPath;


        if((dp  = opendir(path.c_str())) == NULL) {
        	printf("Trying to open the dir %s\n", path.c_str());
                return false;
        }

        absPath = path;
        if(absPath[absPath.size()-1] != '/'){
                absPath.append("/");
        }

        folders.clear();

        while ((dirp = readdir(dp)) != NULL) {
                if(dirp->d_type & DT_DIR){
                        // check if is . or .. folder, skip it
                        aux = dirp->d_name;
                        if((aux == ".") || (aux == "..")){
                                continue;
                        }
                        // put ABSOLUTE PATH
                        aux = absPath;
                        aux.append(dirp->d_name);
                        folders.push_back(aux);
                }
        }

        closedir(dp);

        return true;
}




/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
bool FileManager::getAllFolders(const std::string &rootFolder,
                std::list<std::string> &result)
{
        std::stack<std::string> folderStack;
        std::list<std::string> auxList;
        std::string actualFolder;

        result.clear();

        folderStack.push(rootFolder);

        /* we start the BFS */
        while(!folderStack.empty()) {
                actualFolder = folderStack.top();
                folderStack.pop();

                // add it to the result list
                result.push_back(actualFolder);

                if(!getFoldersList(actualFolder, auxList)){
                        return false;
                }

                // now put all the folders in the queue
                for(std::list<std::string>::iterator it = auxList.begin();
                                it != auxList.end(); ++it){
                        folderStack.push(*it);
                }

        }

        return true;

}

/* Read a file.
 * RETURNS:
 *              true            if success
 *              false           otherwise
 */
/******************************************************************************/
bool FileManager::readFileContent(const std::string &fName, std::string &contents)
{
        char *buffer;
        std::ifstream is;
        int fLength = 0;

        /* abrimos el archivo y lo leemos completo */
        is.open(fName.c_str(), std::ios::binary );
        if (!is.good()) {
        	printf("Error openning file %s\n", fName.c_str());
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
        	printf("error reading the file %s\n", fName.c_str());
                delete[] buffer;
                is.close();
                return false;
        }


        is.close();
        /*! FIXME: ineficiente! */
        contents = buffer;
        delete[] buffer;

        return true;
}

bool FileManager::getAllFiles(const std::string &folderPath, 
                              std::list<std::string> &result, 
                              const std::list<std::string> &extensions)
{
        DIR *dp = NULL;
        struct dirent *dirp = NULL;
        std::string aux, absPath;

        if((dp  = opendir(folderPath.c_str())) == NULL) {
                return false;
        }

        result.clear();

        absPath = folderPath;
        if(absPath[absPath.size()-1] != '/'){
                absPath.append("/");
        }

        if(extensions.empty()){
                while ((dirp = readdir(dp)) != NULL) {
                        aux = absPath;
                        aux.append(dirp->d_name);
                        result.push_back(aux);
                }

        } else {

                // we have to filter the files

                while ((dirp = readdir(dp)) != NULL) {
                        aux = dirp->d_name;
                        for(std::list<std::string>::const_iterator it = extensions.begin();
                                        it != extensions.end(); ++it){
                                if(aux.find(*it) != std::string::npos){
                                        // we have to add this file
                                        aux = absPath;
                                        aux.append(dirp->d_name);
                                        result.push_back(aux);
                                }
                        }
                }
        }

        closedir(dp);

        return true;
}

bool FileManager::writeFile(const std::string &fName, const std::string &data,
                bool overwrite)
{
        std::ofstream outfile (fName.c_str(), std::ofstream::binary);

        // TODO: overwrite functionality


        if(!outfile.good()){
        	printf("Error opening (to write) file %s\n", fName.c_str());
                return false;
        }


        // write to outfile
        outfile.write (data.c_str(),data.size());

        if (outfile.fail()){
        	printf("Error writing file %s\n", fName.c_str());
                outfile.close();
                return false;
        }

        outfile.close();

        return true;
}



