/*
 * GUIHelper.cpp
 *
 *  Created on: 18/05/2012
 *      Author: agustin
 *
 */

#include <OgreMaterial.h>
#include <OgreTextureUnitState.h>
#include <OgreTechnique.h>
#include <OgreOverlayManager.h>
#include <OgreTextureManager.h>
#include <OgreStringConverter.h>
#include <OgreMaterialManager.h>

#include <queue>

#include "DebugUtil.h"

#include "GUIHelper.h"

namespace f_e {

/**
 * Ugly but fast recursively function used to reposition the overlaycontainer
 */
void GUIHelper::reposContainer(Ogre::OverlayContainer *o,
		Ogre::Real parentX, Ogre::Real parentY, Ogre::Real parentHeight,
		Ogre::Real parentWidth)
{
	ASSERT(o);
	ASSERT(parentX >= 0.0f && parentX <= 1.0f);
	ASSERT(parentY >= 0.0f && parentY <= 1.0f);
	ASSERT(parentHeight >= 0.0f && parentHeight <= 1.0f);
	ASSERT(parentWidth >= 0.0f && parentWidth <= 1.0f);

//	debugBLUE("Configuring new container: \n"
//			"Top: %f, Left:%f, Width:%f, Height:%f\n"
//			"Parent: %f,%f,%f,%f\n",
//			o->getTop(), o->getLeft(), o->getWidth(), o->getHeight(),
//			parentY, parentX, parentWidth, parentHeight);

	// configure this container
	// size
	ASSERT( o->getHeight() >= 0.0f &&  o->getHeight() <= 1.0f);
	ASSERT(o->getWidth() >= 0.0f && o->getWidth() <= 1.0f);
	o->setHeight(parentHeight * o->getHeight());
	o->setWidth(parentWidth * o->getWidth());

	// position
	ASSERT(o->getTop() >= 0.0f && o->getTop() <= 1.0f);
	ASSERT(o->getLeft() >= 0.0f && o->getLeft() <= 1.0f);
	o->setTop((o->getTop() * parentHeight));
	o->setLeft((o->getLeft() * parentWidth));


	Ogre::OverlayContainer::ChildIterator it = o->getChildIterator();
	Ogre::OverlayContainer *child = 0;
	while(it.hasMoreElements()){
		child = static_cast<Ogre::OverlayContainer *>(it.peekNextValue());
		if(!child){
			return;
		}
		// else.. we have to configure this child with the parent size and
		// position
		reposContainer(child, o->getLeft(), o->getTop(), o->getHeight(),
				o->getWidth());

		it.moveNext();
	}
}

void GUIHelper::reposContainer(Ogre::OverlayContainer *o)
{
	ASSERT(o);
	if(!o->getParent()){
		debugWARNING("This overlay has no parent (%s)\n", o->getName().c_str());
		return;
	}
	// if we have parent then we call the other function
	Ogre::OverlayContainer *parent = o->getParent();
	reposContainer(o, parent->getLeft(), parent->getTop(),parent->getHeight(),
			parent->getWidth());
}

/**
 * Get the absolute position of the container
 * @param	cont	The container
 * @param	top		The absolute top
 * @param	left	The absolute left
 * @param	w		The absolute width
 * @param	h		The absolute height
 */
void GUIHelper::getAbsoluteGeometry(Ogre::OverlayContainer *cont, Ogre::Real &top,
		Ogre::Real &left, Ogre::Real &right, Ogre::Real &bottom)
{
	ASSERT(cont);
	Ogre::OverlayContainer *parnet = cont->getParent();
	if(parnet){
		if(parnet->getParent()){
			debugWARNING("TODO: tenemos que contemplar este caso... en el que "
					"haya mas de un parnet... tendriamos que calcular recursivamente"
					" las posiciones...\n");
			ASSERT(false);
		}
		top = cont->getTop() + parnet->getTop();
		left = cont->getLeft() + parnet->getLeft();
		right = left + cont->getWidth();
		bottom = top + cont->getHeight();
	} else {
		top = cont->getTop();
		left = cont->getLeft();
		right = left + cont->getWidth();
		bottom = top + cont->getHeight();
	}

}


/**
 * Create a new PanelOverlayElement
 * @param	matName		The name of the material
 * @param	rSize		The resulting size of the texture (width)
 * @param	name		The name of the panel
 */
Ogre::PanelOverlayElement *GUIHelper::createPanel(const Ogre::String &matName,
		int &rSize, const Ogre::String &name)
{
	static int counter = 0;

	Ogre::PanelOverlayElement *result = 0;
	Ogre::String realName;

	if(name.empty()){
		realName = "pan" + Ogre::StringConverter::toString(counter);
		counter++;
	} else {
		realName = name;
	}

#ifdef DEBUG
	// ensures the material exists
	Ogre::MaterialPtr		material;
	material = Ogre::MaterialManager::getSingleton().getByName(matName);
	if(material.isNull()){
		debug("Material %s not found\n", matName.c_str());
	}
//	ASSERT(!material.isNull());
#endif

	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	result = static_cast<Ogre::PanelOverlayElement*>(
		overlayManager.createOverlayElement("Panel", realName));
	result->setMetricsMode(Ogre::GMM_RELATIVE);
	result->setMaterialName(matName); // Optional background material

	// get the texture size
	Ogre::String text = result->getMaterial()->getTechnique(0)->getPass(0)->
			getTextureUnitState(0)->getTextureName();
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().getByName(text);
	ASSERT(!texture.isNull());
	rSize = texture->getWidth();

	return result;
}

/**
 * Bugfix for the nested overlay containers position. This function will
 * get a overlay and will iterate over all the (child) containers and will
 * set the new "relative" position
 * @param	overlay		The overlay to fix the childs positions
 */
void GUIHelper::fixOverlayPosition(Ogre::Overlay *overlay)
{
	ASSERT(overlay);

	Ogre::Overlay::Overlay2DElementsIterator it = overlay->get2DElementsIterator();

	Ogre::OverlayContainer *parent = 0;
	while(it.hasMoreElements()){
		parent = it.peekNext();
		if(!parent){
			break;
		}

		// else we have the parent... get the top left and sizes
		Ogre::Real top = parent->getTop();
		Ogre::Real left = parent->getLeft();
		Ogre::Real height = parent->getHeight();
		Ogre::Real width = parent->getWidth();


		Ogre::OverlayContainer::ChildIterator it2 = parent->getChildIterator();
		Ogre::OverlayContainer *child = 0;
		while(it2.hasMoreElements()){
			child = static_cast<Ogre::OverlayContainer *>(it2.peekNextValue());
			if(!child){
				break;
			}
			// else.. we have to configure this child with the parent size and
			// position
			reposContainer(child, left, top, height, width);

			it2.moveNext();
		}


		it.moveNext();
	}
}


////////////////////////////////////////////////////////////////////////////////
void GUIHelper::fullDestroyOverlay(Ogre::Overlay *overlay)
{
	ASSERT(overlay);

	Ogre::Overlay::Overlay2DElementsIterator it = overlay->get2DElementsIterator();
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	std::vector<Ogre::OverlayElement *> elements;

	Ogre::OverlayContainer *parent = 0;
	while(it.hasMoreElements()){
		parent = it.peekNext();
		if(!parent){
			break;
		}

		Ogre::OverlayContainer::ChildIterator it2 = parent->getChildIterator();
		Ogre::OverlayContainer *child = 0;
		while(it2.hasMoreElements()){
			child = static_cast<Ogre::OverlayContainer *>(it2.peekNextValue());
			if(!child){
				break;
			}
			// else.. delete the container
			fullDestroyOverlayElement(child);

			it2.moveNext();
		}

		elements.push_back(parent);
		it.moveNext();
	}
	for(int i = 0; i < elements.size(); ++i){
		destroyMaterial(elements[i]->getMaterialName());
		om.destroyOverlayElement(elements[i]);
	}

	om.destroy(overlay);
}
////////////////////////////////////////////////////////////////////////////////
void GUIHelper::fullDestroyOverlayElement(Ogre::OverlayContainer *elem)
{
	ASSERT(elem);

	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();

	Ogre::OverlayContainer::ChildIterator it = elem->getChildIterator();
	Ogre::OverlayContainer *child = 0;

	// TODO: cuidado aca que podemos estar teniendo SIGSEV si tenemos
	// muchos overlays anidados, deberiamos guardarlos capaz en un container
	// y luego liberar todos de una como en la funcion de arriba
	while(it.hasMoreElements()){
		child = static_cast<Ogre::OverlayContainer *>(it.peekNextValue());
		if(!child){
			break;
		}
		// else.. we have to configure this child with the parent size and
		// position
		fullDestroyOverlayElement(child);
		it.moveNext();
	}

	destroyMaterial(elem->getMaterialName());
	om.destroyOverlayElement(elem);
}

////////////////////////////////////////////////////////////////////////////////
void GUIHelper::fullDestroyOverlaySingleE(Ogre::OverlayElement *elem)
{
	ASSERT(elem);
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	destroyMaterial(elem->getMaterialName());
	om.destroyOverlayElement(elem);
}


////////////////////////////////////////////////////////////////////////////////
Ogre::OverlayContainer *GUIHelper::findContainer(Ogre::Overlay *overlay,
		const Ogre::String subStrName)
{
	ASSERT(overlay);

	Ogre::Overlay::Overlay2DElementsIterator bit = overlay->get2DElementsIterator();
	std::queue<Ogre::OverlayContainer *> q;
	Ogre::OverlayContainer *parent = 0;
	while(bit.hasMoreElements()){
		parent = bit.peekNext();
		if(!parent){
			break;
		}
		q.push(parent);
		bit.moveNext();
	}

	while(!q.empty()){
		parent = q.back();
		q.pop();
		if(parent->getName().find(subStrName) != Ogre::String::npos){
			// we find it
			return parent;
		}
		// else we have to iterate over all the children an put it in the queue
		Ogre::OverlayContainer *child = 0;
		Ogre::OverlayContainer::ChildIterator it = parent->getChildIterator();
		while(it.hasMoreElements()){
			child = static_cast<Ogre::OverlayContainer *>(it.peekNextValue());
			if(!child){
				break;
			}
			// else we put it in the queue
			q.push(child);
			it.moveNext();
		}

	}

	return 0;
}

Ogre::OverlayContainer *GUIHelper::findContainer(Ogre::OverlayContainer *overlay,
			const Ogre::String subStrName)
{
	ASSERT(overlay);

	std::queue<Ogre::OverlayContainer *> q;
	q.push(overlay);

	Ogre::OverlayContainer *parent = 0;
	while(!q.empty()){
		parent = q.back();
		q.pop();
		if(parent->getName().find(subStrName) != Ogre::String::npos){
			// we find it
			return parent;
		}
		// else we have to iterate over all the children an put it in the queue
		Ogre::OverlayContainer *child = 0;
		Ogre::OverlayContainer::ChildIterator it = parent->getChildIterator();
		while(it.hasMoreElements()){
			child = static_cast<Ogre::OverlayContainer *>(it.peekNextValue());
			if(!child){
				break;
			}
			// else we put it in the queue
			q.push(child);
			it.moveNext();
		}

	}

	return 0;
}



/**
 * Destroy / unload the memory of a material
 * @param	mat		MaterialName
 */
void GUIHelper::destroyMaterial(const Ogre::String &matName)
{
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(matName);
	if(mat.isNull()) return;

	Ogre::Material::TechniqueIterator tit = mat->getTechniqueIterator();
	while(tit.hasMoreElements()){
		Ogre::Technique *t = tit.peekNext();
		ASSERT(t);
		Ogre::Technique::PassIterator pit = t->getPassIterator();
		while(pit.hasMoreElements()){
			Ogre::Pass *pass = pit.peekNext();
			ASSERT(pass);
			Ogre::Pass::TextureUnitStateIterator tuit =
					pass->getTextureUnitStateIterator();

			while(tuit.hasMoreElements()){
				Ogre::TextureUnitState *tus = tuit.peekNext();
				ASSERT(tus);
				const Ogre::String &textName = tus->getTextureName();
				Ogre::TextureManager::getSingleton().unload(textName);
				Ogre::TextureManager::getSingleton().remove(textName);

				tuit.moveNext();
			}
			pit.moveNext();
		}

		tit.moveNext();
	}

	Ogre::MaterialManager::getSingleton().unload(matName);
	Ogre::MaterialManager::getSingleton().remove(matName);
}



/**
 * Get all the childrens of an overlay
 * @param	ov		The overlay where we will retreive the childrens
 * @param	ch		Childrens
 */
void GUIHelper::getChildrens(Ogre::Overlay *ov,
		std::vector<Ogre::OverlayContainer *> &ch)
{
	ASSERT(ov);
	ch.clear();

	Ogre::Overlay::Overlay2DElementsIterator bit = ov->get2DElementsIterator();
	Ogre::OverlayContainer *parent = 0;
	while(bit.hasMoreElements()){
		parent = bit.peekNext();
		if(!parent){
			break;
		}
		ch.push_back(parent);
		bit.moveNext();
	}
}
void GUIHelper::getChildrens(Ogre::OverlayContainer *elem,
		std::vector<Ogre::OverlayContainer *> &ch)
{
	ASSERT(elem);
	ch.clear();

	Ogre::OverlayContainer::ChildIterator it = elem->getChildIterator();
	Ogre::OverlayContainer *child = 0;
	while(it.hasMoreElements()){
		child = static_cast<Ogre::OverlayContainer *>(it.peekNextValue());
		if(!child){
			break;
		}
		ch.push_back(child);
		it.moveNext();
	}

}

////////////////////////////////////////////////////////////////////////////
//							TEXTURES FUNCTIONS							  //
////////////////////////////////////////////////////////////////////////////

/**
 * Returns the texture width of a material or -1 if have not texture
 */
int GUIHelper::getTextureWidth(const Ogre::String &materialName)
{
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName);
	if(mat.isNull() || /*!mat->getTechnique(0) ||
			!mat->getTechnique(0)->getPass(0) ||*/
			!mat->getTechnique(0)->getPass(0)->getTextureUnitState(0))
		return -1;

	Ogre::TexturePtr text = Ogre::TextureManager::getSingleton().getByName(
			mat->getTechnique(0)->getPass(0)->getTextureUnitState(
					0)->getTextureName());
	if(text.isNull()) return -1;
	return text->getWidth();
}
int GUIHelper::getTextureHeight(const Ogre::String &materialName)
{
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName);
	if(mat.isNull() || /*!mat->getTechnique(0) ||
			!mat->getTechnique(0)->getPass(0) ||*/
			!mat->getTechnique(0)->getPass(0)->getTextureUnitState(0))
		return -1;

	Ogre::TexturePtr text = Ogre::TextureManager::getSingleton().getByName(
			mat->getTechnique(0)->getPass(0)->getTextureUnitState(
					0)->getTextureName());
	if(text.isNull()) return -1;
	return text->getHeight();
}
void GUIHelper::getTexture(const Ogre::String &matName, Ogre::TexturePtr &result)
{
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(matName);
	if(mat.isNull() || /*!mat->getTechnique(0) ||
			!mat->getTechnique(0)->getPass(0) ||*/
			!mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)){
		result.setNull();
		return;
	}

	result = Ogre::TextureManager::getSingleton().getByName(
			mat->getTechnique(0)->getPass(0)->getTextureUnitState(
					0)->getTextureName());
}

}
