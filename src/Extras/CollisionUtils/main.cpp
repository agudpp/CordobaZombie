/*
 * CollObjExporter main
 *
 *  Created on: Mar 23, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */


#include <iostream>
#include <fstream>
#include <cstdlib>

#include "Ogre.h"
#include "OIS/OIS.h"

#include "DotSceneLoader.h"
#include "CollObjExporter.h"
#include "CollObjImporter.h"
#include "GlobalObjects.h"
#include "DebugUtil.h"


#define  ESTRATEGIA2012_RC_PATH  "ESTRATEGIA2012_RC_PATH"
#define  RESOURCES_CONF_FILE     "resources.cfg"
#define  PLUGINS_CONF_FILE       "plugins.cfg"



class MyApplication
{
public:
	MyApplication() {
		_root = new Ogre::Root(PLUGINS_CONF_FILE);
		if(!_root->showConfigDialog()){
			debugERROR("Couldn't create Ogre root from %s.\n", PLUGINS_CONF_FILE);
			exit(EXIT_FAILURE);
		}
		_window = _root->initialise(true,"Collision Object Exporter");
		_sceneManager = _root->createSceneManager(Ogre::ST_GENERIC);
	}

	~MyApplication() {}

	int startup() {
		Common::GlobalObjects::ogreRoot = _root;
		Common::GlobalObjects::window = _window;
		Common::GlobalObjects::sceneManager = _sceneManager;

		loadResources();
//		createScene();

		return 0;
	}

	void loadResources() {
	    std::string path, fname;

	    // Load CWD resources
		Ogre::ResourceGroupManager::getSingleton().
				addResourceLocation(".", "FileSystem", "Popular");

	    // Load aditional resources from Dropbox.
	    if(!Common::Util::readEnvVar(ESTRATEGIA2012_RC_PATH, path)){
	        debugERROR("Error reading the environmental variable \"%s\"\n"
	                "You have to configure your system... ask Agu Perez\n",
	                ESTRATEGIA2012_RC_PATH);
	        exit(EXIT_FAILURE);
	    } else {
			if(path.size()-1 >= 0 && path[path.size()-1] != '/'){
				path.append("/");
			}
	    	fname = path + RESOURCES_CONF_FILE;
	    }
	    loadResourcesFromFile(fname, path);

	    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}


	void loadResourcesFromFile(const Ogre::String &file, const Ogre::String &path)
	{
	    Ogre::ConfigFile cf;
	    Ogre::String secName, typeName, archName;

	    ASSERT(path.size()-1 >= 0 && path[path.size()-1] == '/');

		// Load resource paths from config file
	    try {
	        cf.load(file);
	    } catch(...) {
	        debugERROR("ERROR: config file \"%s\" not found\n", file.c_str());
	        return;
	    }

	    // Go through all sections & settings in the file
	    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	    Ogre::ResourceGroupManager &rgm(Ogre::ResourceGroupManager::getSingleton());

	    debugBLUE("Loading resources from file %s\n", file.c_str());
	    while (seci.hasMoreElements())
	    {
	        secName = seci.peekNextKey();
	        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
	        Ogre::ConfigFile::SettingsMultiMap::iterator i;
	        for (i = settings->begin(); i != settings->end(); ++i)
	        {
	            typeName = i->first;
	            archName = path + i->second;
	            //debugGREEN("DEBUG: %s -- %s -- %s\n",archName, typeName, secName);
	            rgm.addResourceLocation(archName, typeName, secName);
	        }
	    }
	}


	void createScene() {
		std::ifstream in;
		std::ofstream out;
		std::string name;

		char actualScene[256];
		char size[256];
		unsigned int sfSize;

		in.open("scenefiles");
		if (!in.is_open()) {
			debugERROR("Error trying to open the scene list file\n");
		}
		in.getline(size,256,'\n');
		sfSize = atoi(size);

		for (int i=0; i<sfSize ;i++){
			std::stringstream data;
			std::cout << "infor\n";
			in.getline(actualScene,256,'\n');
			//outfile
			name = "collobjConfig-";
			name += actualScene;
			std::cout << name << "\n";
			out.open(name.c_str());
			if(!out.is_open()){
				std::cout << "Error trying to open the config file\n";
			}
			// import .scene
			Ogre::SceneNode* _LoaderNode;
			_LoaderNode = _sceneManager->getRootSceneNode()->createChildSceneNode(name);
			Loader.parseDotScene(actualScene,Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, _sceneManager, _LoaderNode);

			std::cout << "cantidad de children de LoaderNode " << _LoaderNode->numChildren() << "\n";
			// show all the nodes
			// (if any node except _LoaderNode has more than 1 entity, the scene is fucked up)
			std::cout << "\n\n\n" << DumpNodes(_LoaderNode) << "\n\n\n";

			data << CollObjExporter::sceneExtractor(_LoaderNode);
			std::cout << "EXPORTER\n\n" << data.str();
			out << data.str();
			out.close();
			DestroyAllAttachedMovableObjects(_LoaderNode);
			_LoaderNode->removeAndDestroyAllChildren();
			std::cout << "\n\n\n" << DumpNodes(_LoaderNode) << "\n\n\n";
			_sceneManager->getRootSceneNode()->removeChild(name);
		}
		std::cout << "export finished \n";
	}

	/*dumper*/ /*TODO: humanize*/
	void DumpNodes(std::stringstream &ss, Ogre::Node *n, int level)
	   {
		  for(int i = 0; i < level; i++)
		  {
			 ss << " ";
		  }
		  ss << "SceneNode: " << n->getName() << " (parent: " << n->getParent()->getName() << ")" <<  std::endl;

		  Ogre::SceneNode::ObjectIterator object_it = ((Ogre::SceneNode *)n)->getAttachedObjectIterator();
		  Ogre::Node::ChildNodeIterator node_it = n->getChildIterator();

		  Ogre::MovableObject *m;
		  while(object_it.hasMoreElements())
		  {
			 for(int i = 0; i < level + 2; i++)
			 {
				ss << " ";
			 }
			 m = object_it.getNext();
			 ss << "parent(" << m->getParentSceneNode()->getName() << ") type " << m->getMovableType() << ": " << m->getName() << std::endl;
		  }
		  while(node_it.hasMoreElements())
		  {
			 DumpNodes(ss, node_it.getNext(), level + 2);
		  }
	   }

	   std::string DumpNodes(Ogre::Node *n)
	   {
		  std::stringstream ss;
		  ss << std::endl << "Node Hierarchy:" << std::endl;
		  DumpNodes(ss, n, 0);
		  return ss.str();
	   }


	void DestroyAllAttachedMovableObjects(Ogre::SceneNode* i_pSceneNode )
	{
		if (!i_pSceneNode )
			return;
		// Destroy all the attached objects
		Ogre::SceneNode::ObjectIterator itObject = i_pSceneNode->getAttachedObjectIterator();
		while (itObject.hasMoreElements() )
		{
			Ogre::MovableObject* pObject = static_cast<Ogre::MovableObject*>(itObject.getNext());
			i_pSceneNode->getCreator()->destroyMovableObject(pObject);
		}
		// Recurse to child SceneNodes
		Ogre::SceneNode::ChildNodeIterator itChild = i_pSceneNode->getChildIterator();
		while (itChild.hasMoreElements() )
		{
			Ogre::SceneNode* pChildNode = static_cast<Ogre::SceneNode*>(itChild.getNext());
			DestroyAllAttachedMovableObjects(pChildNode);
		}
	}

private:
	Ogre::SceneManager* _sceneManager;
	Ogre::RenderWindow* _window;
	Ogre::Root* _root;
	Ogre::DotSceneLoader Loader;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////                                //////////////////////////
//////////////////////          MAIN PROGRAM          //////////////////////////

int main (void)
{
	const char fname[] = "collision_objects.dat";  // OUTPUT FILENAME XXX
	std::ofstream outfile(fname);
	CollObjExporter exporter;
	CollObjImporter importer;
	MyApplication app;

	if (!outfile.is_open()) {
		debugERROR("Couldn't open output file.\n");
		return EXIT_FAILURE;
	}

	// Initialize Ogre environment, to have access to all resources.
	app.startup();

	// Call exporter on CWD
	std::string data = exporter.dirExtractor();
	if (data.empty()) {
		debugWARNING("\nData export produced no output. "
					"Are there any .scene files in around?\n");
	} else {
		outfile << data;
		outfile.close();
		std::cout << DEBUG_GREEN "\nCollision data exported into file \""
				  << fname << "\"\n" << DEBUG_NC << std::endl;
		importer.importFromFile(fname);
		std::vector<CollisionObject*> vec = importer.getCollisionObjects();
		ASSERT(vec.size() == 0);
	}

	return EXIT_SUCCESS;
}

