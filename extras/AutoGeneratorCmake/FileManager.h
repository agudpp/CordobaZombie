/* 
 * File:   FileManager.h
 * Author: adrian
 *
 * Created on 10 de agosto de 2011, 17:25
 */

#ifndef FILEMANAGER_H
#define	FILEMANAGER_H

#include <list>
#include <string>


class FileManager {
public:

         /* Returns the instance */
        static FileManager *getInstance(void);
    
        /* Function to get all the Folders that exist in "rootFolder".
         * This do a Recursive search
         * RETURNS:
         *              true            if success
         *              false           otherwise
         */
        bool getAllFolders(const std::string &rootFolder, std::list<std::string> &result);

        /* Read a file.
         * RETURNS:
         *              true            if success
         *              false           otherwise
         */
        bool readFileContent(const std::string &fName, std::string &contents);
        
        /* Returns all the folders in a path */
        bool getFoldersList(const std::string &path, std::list<std::string> &folders);
        
        /* Get all the files that exists in some folder.
         * REQUIRES:
         *              folderPath              the path of the folder
         *              extensions              this is a filter (if is empty returns all the files)
         * RETURNS:
         *              true                    on success
         *              false                   otherwise
         */
        bool getAllFiles(const std::string &folderPath,  
                         std::list<std::string> &result,  
                         const std::list<std::string> &extensions);

         /* Writes a file. If the file already exists this ovterwrites the file
         * if and only if the flag overwrite is true TODO this functionality
         * RETURNS:
         *              true            if success
         *              false           otherwise
         */
        bool writeFile(const std::string &fName, const std::string &content,
                        bool overwrite = true);


        

private:
        FileManager(){};
        ~FileManager(){};


private:
        static FileManager *mInstance;

};

#endif	/* FILEMANAGER_H */

