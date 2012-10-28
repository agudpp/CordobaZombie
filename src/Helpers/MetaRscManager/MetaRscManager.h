/*
 * MetaRscManager.h
 *
 *  Created on: 28/10/2012
 *      Author: agustin
 *
 */

#ifndef METARSCMANAGER_H_
#define METARSCMANAGER_H_


#include <OgreResourceManager.h>

#include <string>
#include <vector>
#include <map>



namespace helper {

class MetaRscManager {

    struct RscInfo {
        std::string filename;
        std::vector<std::string> groupNames;

        RscInfo &operator=(const RscInfo &o){
            filename = o.filename;
            groupNames = o.groupNames;
        }
    };

public:
    typedef int FileID;

public:

    static MetaRscManager &getInstance(void){
        static MetaRscManager instance;
        return instance;
    }
    ~MetaRscManager();


    /**
     * Load a resource file and track it to be unloaded later.
     * The filename will be prepended with the path where the resources are
     */
    FileID loadResourceFile(const std::string &filename);

    /**
     * Unload a resource file
     */
    void unloadResourceFile(FileID f);

    /**
     * Unload all resources files
     */
    void unloadAll(void);


private:
    MetaRscManager();
    MetaRscManager(const MetaRscManager &);
    MetaRscManager &operator=(const MetaRscManager&);

    /**
     * Returns the first FileID available
     */
    inline FileID getFreeId(const RscInfo &i);


private:


    typedef std::vector<RscInfo>   FileMap;


    FileMap mFiles;

};


inline MetaRscManager::FileID
MetaRscManager::getFreeId(const RscInfo &i)
{
    const size_t size = mFiles.size();
    for(size_t i = 0; i < size; ++i){
        if (mFiles[i].filename.empty()) {
            mFiles[i] = i;
            return i;
        }
    }
    mFiles.push_back(i);
    return size;
}

}

#endif /* METARSCMANAGER_H_ */
