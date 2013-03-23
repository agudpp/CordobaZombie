/*
 * CollObjExporter.cpp
 *
 *  Created on: 23/05/2012
 *      Author: santiago
 */

//TODO check the includes
#include <Ogre.h>
#include <OIS\OIS.h>

#include "DotSceneLoader.h"
#include "CollObjExporter.h"

#include <iostream>
#include <fstream>

class MyApplication {

	private:
		Ogre::SceneManager* _sceneManager;
		Ogre::RenderWindow* _window;
		Ogre::Root* _root;
		Ogre::DotSceneLoader Loader;


	public:
	
		int startup(){
			_root = new Ogre::Root("plugins_d.cfg");

			if(!_root->showConfigDialog()){
				return -1;
			}

			_window = _root->initialise(true,"Collision Object Exporter");
			_sceneManager = _root->createSceneManager(Ogre::ST_GENERIC);
	
			loadResources();
			createScene();
			_root->startRendering();
			return 0;
		}

		void loadResources(){
			Ogre::ConfigFile cf;
			cf.load("resources_d.cfg");

			Ogre::ConfigFile::SectionIterator sectionIter = cf.getSectionIterator();
			Ogre::String sectionName, typeName, dataname;
			while (sectionIter.hasMoreElements()){
				sectionName = sectionIter.peekNextKey();
				Ogre::ConfigFile::SettingsMultiMap *settings = sectionIter.getNext();
				Ogre::ConfigFile::SettingsMultiMap::iterator i;
				for (i = settings->begin(); i != settings->end(); ++i){
					typeName = i->first;
					dataname = i->second;
					Ogre::ResourceGroupManager::getSingleton().addResourceLocation(dataname, typeName, sectionName);
				}
			}

			Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		}

		void createScene(){
			
			/////////////////////////////////////////////////////////////////////////////////
			//// HERE BE THE USEFUL CODE
			/////////////////////////////////////////////////////////////////////////////////
			
			std::ifstream in;
			std::ofstream out;
			std::string name;

			char actualScene[256];
			char size[256];
			unsigned int sfSize;
						
			in.open("scenefiles");
			if(!in.is_open()){
				std::cout << "Error trying to open the scene list file\n";
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
		
		MyApplication(){
			_sceneManager = NULL;
			_root = NULL;
		}

		~MyApplication(){
			delete _root;
		}
};

int main (void) {
	MyApplication app;
	app.startup();
	return 0;
}

